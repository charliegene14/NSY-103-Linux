/**
 * @file server.c
 * @brief Point d'entrée du serveur.
 *
 * Ce fichier implémente le serveur principal de l'application, chargé de gérer les connexions clients,
 * la synchronisation des ressources partagées et la communication inter-processuss. Le serveur utilise notamment :
 *  - Un segment de mémoire partagée pour stocker et partager l'état global des philosophes et des baguettes.
 *  - Des sémaphores pour la synchronisation de l'accès aux ressources partagées (création de philosophes, accès aux baguettes,
 *    et limitation du nombre de philosophes pouvant manger simultanément).
 *  - Une file de messages IPC pour la gestion asynchrone des logs (logs globaux du serveur et logs spécifiques aux clients).
 *
 * Les fonctionnalités principales de ce fichier comprennent :
 *  - L'initialisation des gestionnaires de signaux via initEndSignals() et le handler programEndHandler(), afin de
 *    détecter les demandes d'arrêt (SIGINT) ou les erreurs critiques (SIGSEGV) et d'activer un flag de shutdown global.
 *
 *  - La gestion des logs via deux threads dédiés :
 *      - serverStateLogsThread() : Lit les logs de type SERVER_LOG_TYPE depuis la file de messages et les écrit
 *        dans un fichier de log d'état du serveur, permettant de suivre l'activité globale.
 *      - clientInfoLogsThread() : Pour chaque client (processus fils), lit les logs spécifiques à ce client (identifiés
 *        par le PID) depuis la file de messages et les écrit dans un fichier dédié.
 *
 *  - Le traitement des requêtes clients :
 *      - manageCreateRequest() : Gère les requêtes de création de philosophes (REQUEST_CREATE) en créant un nouveau
 *        philosophe côté serveur et en renvoyant une réponse (RESPONSE_CREATE) au client.
 *      - manageUpdateRequest() : Gère les requêtes de mise à jour de l'état d'un philosophe (REQUEST_UPDATE) et envoie
 *        une réponse (RESPONSE_UPDATE) correspondante.
 *
 *  - La gestion d'un processus client via clientProcess(), qui :
 *      - Initialise le générateur de nombres aléatoires.
 *      - Attend et traite les requêtes envoyées par le client sur son socket de service.
 *      - Réagit aux différentes demandes (création ou mise à jour) et communique les réponses appropriées.
 *
 *  - La boucle principale du serveur dans main(), qui effectue les opérations suivantes :
 *      - Vérifie la compatibilité avec le nombre maximal de fichiers ouverts (FOPEN_MAX) et avertit si nécessaire.
 *      - Crée et attache un segment de mémoire partagée pour héberger les ressources partagées (philosophes, baguettes,
 *        logs, etc.).
 *      - Initialise et configure le socket serveur (création, binding, écoute).
 *      - Crée la file de messages pour la gestion des logs.
 *      - Configure le ServerContext pour centraliser la gestion des ressources (sockets, mémoire partagée, logs).
 *      - Lance un thread pour le log global du serveur.
 *      - Entre dans une boucle d'acceptation des connexions clients, et pour chaque connexion :
 *          - Accepte la connexion sur le socket de service.
 *          - Crée un processus fils via fork() pour gérer le client avec clientProcess().
 *          - Dans le processus parent, ouvre un thread de logs dédié pour le nouveau client et met à jour le contexte serveur.
 *      - Sur détection d'une demande d'arrêt (shutdownFlag), procède à un nettoyage global des ressources via cleanup()
 *        avant de terminer.
 *
 * Les modules utilisés dans ce fichier proviennent de divers fichiers d'en-tête et d'implémentation, notamment :
 *  - Utilitaires : sockets.h, print_message.h, random.h, files.h.
 *  - Gestion des ressources partagées et des logs : Logs.c, SharedResources.c, ServerPhilosopher.c, Request.c,
 *    Response.c, ServerContext.c.
 *
 * @note Ce fichier constitue le cœur du serveur, orchestrant la communication entre les clients et la gestion centralisée
 *       des ressources partagées, tout en assurant une synchronisation fine.
 */

#include "../include/utils/sockets.h"
#include "../include/utils/print_message.h"
#include "../include/utils/random.h"
#include "../include/managers/Logs.c"
#include "../include/managers/SharedResources.c"
#include "../include/managers/ServerPhilosopher.c"
#include "../include/managers/Request.c"
#include "../include/managers/Response.c"
#include "../include/managers/ServerContext.c"
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

/**
 * @brief Flag global indiquant la demande d'arrêt du serveur.
 *
 * Ce flag est modifié par le handler de signal pour stopper les boucles principales et les threads,
 * permettant ainsi un arrêt contrôlé du serveur.
 * Car impossible de donner ServerContext au handler du signal : https://stackoverflow.com/questions/6970224/providing-passing-argument-to-signal-handler

 */
volatile sig_atomic_t shutdownFlag = 0;

/**
 * @brief Handler de signal pour terminer le programme.
 *
 * Cette fonction est appelée lorsqu'un signal d'arrêt (SIGINT, SIGSEGV) est reçu.
 * Elle met à jour le flag global shutdownFlag pour indiquer qu'un arrêt est demandé.
 *
 * @param signum Numéro du signal reçu.
 */
void programEndHandler(int signum) {
    shutdownFlag = 1;
}

/**
 * @brief Initialise les handlers pour les signaux SIGINT et SIGSEGV.
 *
 * Configure le handler programEndHandler pour SIGINT et SIGSEGV. En cas d'échec de l'installation
 * du handler, un message d'erreur est affiché et le programme se termine.
 */
void initEndSignals() {

    struct sigaction sigalAction;
    sigalAction.sa_handler = programEndHandler;
    sigemptyset(&sigalAction.sa_mask);
    sigalAction.sa_flags = 0;

    if (sigaction(SIGINT, &sigalAction, NULL) == -1) {
        printMessage(ERROR, "Erreur lors de l'installation du handler pour le signal SIGINT.\n");
        exit(EXIT_FAILURE);
    }

    // A été nécéssaire pendant le dev, car les segmentation faults qui apparaissaient laissaient des ressources ouvertes
    if (sigaction(SIGSEGV, &sigalAction, NULL) == -1) {
        printMessage(ERROR, "Erreur lors de l'installation du handler pour le signal SIGSEGV.\n");
        exit(EXIT_FAILURE);
    }

}

/**
 * @brief Thread pour la gestion des logs globaux du serveur.
 *
 * Cette routine lit en boucle les messages de log de type SERVER_LOG_TYPE depuis la file de messages IPC
 * et les écrit dans un fichier de log d'état du serveur. La lecture continue tant que shutdownFlag n'est pas activé.
 *
 * @param arg Pointeur vers l'identifiant de la file de messages (logsQueueId).
 * @return void* Retourne toujours NULL.
 */
void *serverStateLogsThread(void *arg) {
    int *logsQueueId = (int *) arg;

    Log *log = malloc(sizeof(Log));
    char *serveStateLogsFilePath = getServerStateFilePath();
    FILE *logFile = fopen(serveStateLogsFilePath, "a");
    free(serveStateLogsFilePath);

    if (!logFile) {
        free(log);
        fclose(logFile);
        exit(EXIT_FAILURE);
    }

    while (!shutdownFlag) {
        // Pas besoin de gérer l'erreur, si ça ne passe pas on passe de nouveau en attente
        ssize_t receivedLogSize = msgrcv(*logsQueueId, log, LOG_BUFFER_SIZE, SERVER_LOG_TYPE, 0);

        if (receivedLogSize == -1) {
            continue;
        }

        if (receivedLogSize > 0) {
            fprintf(logFile, "%s", log->text);
            // On inscrit immédiatement le contenu dans le fichier, sinon il reste dans un buffer et tail ne peut pas voir le contenu
            fflush(logFile);
        }
    }

    fclose(logFile);
    free(log);
    return NULL;
}

/**
 * @brief Thread pour la gestion des logs spécifiques à un client.
 *
 * Cette routine lit en boucle les messages de log destinés à un client (identifiés par son PID)
 * depuis la file de messages IPC et les écrit dans un fichier de log dédié. La boucle s'exécute
 * tant que shutdownFlag n'est pas activé.
 *
 * @param arg Pointeur vers une structure LogThreadInfo contenant l'identifiant de la file de messages et le PID du client.
 * @return void* Retourne toujours NULL.
 */
void *clientInfoLogsThread(void *arg) {

    LogThreadInfo *logThreadInfo = (LogThreadInfo *) arg;
    Log *log = malloc(sizeof(Log));

    char *filePath = getClientInfoFilepath(logThreadInfo->childProcessId);
    FILE *logFile = fopen(filePath, "a");
    free(filePath);

    if (!logFile) {
        free(log);
        raise(SIGINT);
    }

    while (!shutdownFlag) {
        // Pas besoin de gérer l'erreur, si ça ne passe pas on passe de nouveau en attente
        ssize_t receivedLogSize = msgrcv(logThreadInfo->logsQueueId, log, LOG_BUFFER_SIZE, (long) logThreadInfo->childProcessId, 0);

        if (receivedLogSize == -1) {
            continue;
        }

        if (receivedLogSize > 0) {
            fprintf(logFile, "%s", log->text);
            // On inscrit immédiatement le contenu dans le fichier, sinon il reste dans un buffer et tail ne peut pas voir le contenu
            fflush(logFile);
        }
    }

    fclose(logFile);
    free(log);
    return NULL;
}

/**
 * @brief Gère une requête de création de philosophe.
 *
 * Lorsqu'une requête de création (REQUEST_CREATE) est reçue, cette fonction crée un nouveau philosophe côté serveur
 * en appelant createPhilosopher(), prépare une réponse (RESPONSE_CREATE) avec les informations du philosophe, et
 * envoie cette réponse au client via le socket de service. En cas d'erreur lors de l'envoi, le serveur déclenche
 * un arrêt contrôlé.
 *
 * @param request Requête de création reçue du client.
 * @param serviceSocket Socket de service associé à la connexion client.
 * @param sharedResources Pointeur vers les ressources partagées.
 */
void manageCreateRequest(Request request, int serviceSocket, SharedResources *sharedResources) {
    ServerPhilosopher created = createPhilosopher(sharedResources);
                
    // Renvoi du philosophe au client
    Response response = createResponse(created.base);
    ssize_t bytesSent = trySocketWrite(serviceSocket, &response, sizeof(response));

    if (bytesSent == -1) {
        logClientInfo(sharedResources->logsQueueId, "Erreur lors d'une tentative d'envoi d'une réponse de création.\n");
    } else if(bytesSent == 0) {
        logClientInfo(sharedResources->logsQueueId, "Le client a coupé la connexion.\n");
    }

    if (bytesSent == -1 || bytesSent == 0) {
        // En coupant le parent, on lance le mécanisme de cleanup centralisé.
        kill(getppid(), SIGINT);
        exit(EXIT_FAILURE);
    }

    logClientInfo(sharedResources->logsQueueId, "Philosophe connecté et ajouté à la table !\n");
}

/**
 * @brief Gère une requête de mise à jour de l'état d'un philosophe.
 *
 * Cette fonction traite une requête de mise à jour (REQUEST_UPDATE) en appelant updatePhilosopher() pour
 * mettre à jour l'état du philosophe côté serveur. Si la mise à jour aboutit, une réponse (RESPONSE_UPDATE)
 * est envoyée au client. En cas d'échec d'envoi, le serveur déclenche un arrêt contrôlé.
 *
 * @param request Requête de mise à jour reçue du client.
 * @param serviceSocket Socket de service associé à la connexion client.
 * @param sharedResources Pointeur vers les ressources partagées.
 */
void manageUpdateRequest(Request request, int serviceSocket, SharedResources *sharedResources) {

    ServerPhilosopher *serverPhilosopher = updatePhilosopher(request.philosopher, sharedResources);

    if (serverPhilosopher == NULL) {
        return;
    }

    Response response = updateResponse(serverPhilosopher->base);  
    ssize_t bytesSent = trySocketWrite(serviceSocket, &response, sizeof(response));

    if (bytesSent == -1) {
        logClientInfo(sharedResources->logsQueueId, "Erreur lors d'une tentative d'envoi d'une réponse pour autoriser le philosophe a manger.\n");
    } else if(bytesSent == 0) {
        logClientInfo(sharedResources->logsQueueId, "Le client a coupé la connexion.\n");
    }

    if (bytesSent == -1 || bytesSent == 0) {
        // En coupant le parent, on lance le mécanisme de cleanup centralisé.
        kill(getppid(), SIGINT);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Processus client dédié.
 *
 * Cette fonction est exécutée par le processus fils créé pour chaque client. Elle initialise le générateur
 * de nombres aléatoires, puis entre dans une boucle pour lire et traiter les requêtes envoyées par le client
 * via son socket de service. Selon le type de requête (création ou mise à jour), elle appelle la fonction
 * appropriée pour traiter la demande. En cas d'erreur ou de déconnexion du client, le processus termine
 * en déclenchant un arrêt contrôlé.
 *
 * @param serviceSocket Socket de service associé à la connexion client.
 * @param sharedResources Pointeur vers les ressources partagées.
 */
void clientProcess(int serviceSocket, SharedResources *sharedResources) {

    initRandom();
    logClientInfo(sharedResources->logsQueueId, "Processus serveur ouvert pour le client !\n");

    while (1) {

        Request request;

        // En cas d'erreur de récéption, étant déjà connecté, on peut tenter plusieurs essais avant d'abandonner
        ssize_t bytesReceived = trySocketRead(serviceSocket, &request, sizeof(request));

        if (bytesReceived == -1) {
            logClientInfo(sharedResources->logsQueueId, "Erreur à la lecture du socket\n");
                
        } else if(bytesReceived == 0) {
            logClientInfo(sharedResources->logsQueueId, "Le client a coupé la connexion.\n");
        }

        if (bytesReceived == -1 || bytesReceived == 0) {
            // En coupant le parent, on lance le mécanisme de cleanup centralisé.
            kill(getppid(), SIGINT);
            exit(EXIT_FAILURE);
        }

        switch (request.type) {

            case REQUEST_CREATE: {
                manageCreateRequest(request, serviceSocket, sharedResources);
                break;
            }

            case REQUEST_UPDATE:
                manageUpdateRequest(request, serviceSocket, sharedResources);
                break;
        }
    }
}

/**
 * @brief Fonction principale du serveur.
 *
 * La fonction main :
 * - initialise les signaux de fin, 
 * - crée la mémoire partagée,
 * - configure le socket serveur 
 * - configure la file de messages pour les logs
 * - centralise les ressources dans un ServerContext 
 * - lance ensuite un thread pour la gestion globale des logs du serveur
 * - entre dans une boucle d'acceptation des connexions clients.
 * 
 * Pour chaque connexion acceptée : 
 * - un processus fils est créé pour traiter les requêtes du client (via clientProcess()),
 * - le processus parent crée un thread dédié pour gérer les logs spécifiques au client. 
 * 
 * En cas d'arrêt (shutdownFlag activé),
 * le serveur procède au nettoyage global des ressources avant de terminer.
 *
 * @param argc Nombre d'arguments en ligne de commande.
 * @param argv Tableau des arguments en ligne de commande.
 * @return int Code de sortie (0 en cas de succès).
 */
int main(int argc, char *argv[]) {

    initEndSignals();

    if (MAX_PHILOSOPHERS > FOPEN_MAX) {
        printMessage(
            WARNING,
            "Un fichier de log par client (thread) sera ouvert par la processus principal, le programme permet d'en créer %d max, et votre système permet l'ouverture de %d max. \n",
            MAX_PHILOSOPHERS,
            FOPEN_MAX
        );
    }
    
    // Initialisation de la mémoire partagée
    int sharedResourcesMemoryId = shmget(IPC_PRIVATE, sizeof(SharedResources) * MAX_PHILOSOPHERS, IPC_CREAT | 0600); // 0600 car uniquement ce processus et ses fils peuvent lire et écrire dans le segment

    if (sharedResourcesMemoryId == -1) {
        printMessage(ERROR, "Erreur lors de la création de la mémoire partagé.\n");
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attachement de la mémoire partagée
    SharedResources *sharedResources = attachSharedResources(sharedResourcesMemoryId); 
    
    int serverSocket = getSocket();
    struct sockaddr_in socketAddress = getSocketAddress();

    if (bind(serverSocket, (struct sockaddr *) &socketAddress, sizeof(socketAddress)) == -1) {
        printMessage(ERROR, "L'adresse n'a pas pu être associée au socket.\n");
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printMessage(SUCCESS, "Socket initialisé avec succès !\n\n");

    if (listen(serverSocket, 3) == -1) {
        printMessage(ERROR, "Le serveur a échoué à se mettre en écoute.\n");
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Création de la file de message pour accueillir les logs
    sharedResources->logsQueueId = initLogsQueue();

    // Mise en contexte de toutes les ressources pour centraliser la gestion de la mémoire en cas de panne
    ServerContext serverContext = initServerContext();
    serverContext.serverSocket = serverSocket;
    serverContext.sharedResourcesMemoryId = sharedResourcesMemoryId;
    serverContext.sharedResources = sharedResources;

    // Ouverture d'un thread pour accueillir les log globaux
    pthread_t serverStateThread;

    if (pthread_create(&serverStateThread, NULL, serverStateLogsThread, &sharedResources->logsQueueId) == -1) {
        printMessage(ERROR, "Erreur lors de la création du thread de logs principal.\n");
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    char *serverStateLogsFilePath = getServerStateFilePath();
    printMessage(SUCCESS, "Le thread des logs du serveur a bien été ouvert, visionner les logs via la commande suivante dans un autre terminal: \n");
    printf("tail -f \"%s\"\n\n", serverStateLogsFilePath);
    free(serverStateLogsFilePath);

    logServerState(sharedResources->logsQueueId, "Adresse mémoire partagée : %p\n", sharedResources);
    logServerState(sharedResources->logsQueueId, "Adresse baguettes : %p\n", sharedResources->chopsticks);

    // Gestion de la prise des connexions dans un thread pour que les processus
    while (!shutdownFlag) {
        int serviceSocket;
        struct sockaddr clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);

        printMessage(INFO, "En écoute sur le socket de service...\n");

        // Ici on peut tenter d'accepter d'autres demandes de connexions, pas besoin de tout fermer
        if ((serviceSocket = accept(serverSocket, &clientAddress, &clientAddressLength)) == -1) {
            printMessage(ERROR, "Le serveur a abdonné une connexion.\n");
            perror("accept");
            close(serviceSocket);
            continue;
        }

        printMessage(SUCCESS, "Connexion de client reçue et acceptée ! \n\n");

        int childProcessId;

        if ((childProcessId = fork()) == -1) {
            printMessage(ERROR, "Le serveur n'a pas pu créer le processus fils pour le client.\n");
            perror("fork");
            close(serviceSocket);
            continue;
        }

        if (childProcessId == 0) {
            clientProcess(serviceSocket, sharedResources);
            // Le processus fils ne doit pas process la boucle du père
            exit(EXIT_SUCCESS);

        } else {
            
            char *logFilePath = getClientInfoFilepath(childProcessId);
            FILE *logFile = fopen(logFilePath, "w");

            // Ouverture d'un thread pour accueillir les logs du nouveau processus fils et de son fichier de logs
            // S'il échoue, on ferme la connexion, on termine le processus fils et on annule

            pthread_t clientLogsThread;
            LogThreadInfo *logThreadInfo = malloc(sizeof(LogThreadInfo));
            if (logThreadInfo == NULL) {
                perror("malloc");
                free(logFilePath);
                fclose(logFile);
                exit(EXIT_FAILURE);
            }

            logThreadInfo->logsQueueId = sharedResources->logsQueueId;
            logThreadInfo->childProcessId = childProcessId;
            
            if (pthread_create(&clientLogsThread, NULL, clientInfoLogsThread, logThreadInfo) == -1) {
                printMessage(ERROR, "Erreur lors de la création du thread de logs.\n");
                perror("pthread_create");
                close(serviceSocket);
                free(logFilePath);
                fclose(logFile);
                break;
            }

            printMessage(SUCCESS, "Le thread de log a bien été ouvert, visionner les logs via la commande suivante dans un autre terminal: \n");
            printf("tail -f \"%s\"\n\n", logFilePath);
            
            free(logFilePath);
            fclose(logFile);

            serverContext.serviceSockets[serverContext.numberServiceSockets] = serviceSocket;
            serverContext.clientsProcessIds[serverContext.numberClients] = childProcessId;
            serverContext.numberServiceSockets += 1;
            serverContext.numberClients += 1;
            
        }

    }
    
    // On procède au nettoyage global avant de quitter
    cleanup(&serverContext);
    printMessage(INFO, "Fin du process serveur principal.\n");

    return 0;
}
