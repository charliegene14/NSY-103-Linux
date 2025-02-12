/**
 * @file ServerContext.c
 * @brief Implémente les fonctions de gestion du contexte serveur.
 *
 * Ce fichier d'implémentation fournit deux fonctions essentielles pour la gestion des ressources du serveur :
 *  - **initServerContext()** : Initialise une structure `ServerContext` en mettant à zéro ses champs et en
 *    définissant des valeurs initiales par défaut.
 *  - **cleanup(ServerContext *serverContext)** : Libère et nettoie toutes les ressources utilisées par le serveur,
 *    incluant les sockets, la mémoire partagée, les sémaphores, et la file de messages IPC pour les logs.
 *
 * Les inclusions nécessaires dans ce fichier sont :
 *  - "../maxmin_philosophers.h" pour la définition des constantes globales.
 *  - "../entities/ServerContext.h" pour la définition de la structure `ServerContext`.
 *  - "../utils/print_message.h" pour l'affichage de messages d'information et de succès.
 *  - <unistd.h>, <sys/shm.h>, <sys/msg.h>, <signal.h>, et <string.h> pour diverses fonctions systèmes.
 *
 * @note Ces fonctions sont essentielles pour assurer une gestion propre des ressources lors du démarrage et de l'arrêt
 * du serveur.
 */

#ifndef SERVERCONTEXT_C
#define SERVERCONTEXT_C

#include "../maxmin_philosophers.h"
#include "../entities/ServerContext.h"
#include "../utils/print_message.h"
#include <unistd.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <string.h>

/**
 * @brief Initialise le contexte serveur.
 *
 * Cette fonction crée et initialise une structure `ServerContext` en mettant à zéro l'ensemble de ses champs à
 * l'aide de `memset`. Les valeurs initiales suivantes sont définies :
 *  - `serverSocket` est initialisé à -1 car socket() retourne -1 en cas d'erreur
 *  - `sharedResourcesMemoryId` est initialisé à -1 car shmget() retourne -1 en cas d'erreur
 *  - `numberServiceSockets` et `numberClients` sont initialisés à 0.
 *
 * @return ServerContext Le contexte serveur initialisé.
 */
ServerContext initServerContext() {
    ServerContext serverContext;
    memset(&serverContext, 0, sizeof(ServerContext));
    
    serverContext.serverSocket = -1;
    serverContext.sharedResourcesMemoryId = -1;
    serverContext.numberServiceSockets = 0;
    serverContext.numberClients = 0;

    return serverContext;
    
}

/**
 * @brief Nettoie et libère les ressources associées au serveur.
 *
 * Cette fonction effectue les opérations de nettoyage suivantes :
 *  - Affiche un message indiquant le début du nettoyage.
 *  - Termine tous les processus de service en envoyant un signal SIGKILL à chacun des PID stockés.
 *  - Ferme le socket principal du serveur s'il est ouvert.
 *  - Ferme tous les sockets de service.
 *  - Supprime la file de messages IPC utilisée pour les logs.
 *  - Détruit les sémaphores utilisés pour la synchronisation dans la mémoire partagée, y compris ceux des baguettes.
 *  - Détache la mémoire partagée et libère le segment de mémoire.
 *
 * @param serverContext Pointeur vers la structure `ServerContext` contenant les ressources à nettoyer.
 */
void cleanup(ServerContext *serverContext) {
    printMessage(INFO, "Nettoyage des ressources...\n");

    // Termine tout les processus service
    for (int i = 0; i < serverContext->numberClients; i++) {
        kill(serverContext->clientsProcessIds[i], SIGKILL);
        printMessage(SUCCESS, "Processus %d tué correctement. \n", serverContext->clientsProcessIds[i]);
    }

    // Ferme le socket serveur
    if (serverContext->serverSocket != -1) {
        close(serverContext->serverSocket);
        printMessage(SUCCESS, "Socket principal (%d) fermé correctement.\n", serverContext->serverSocket);
    }

    // Ferme tous les sockets de service
    for (int i = 0; i < serverContext->numberServiceSockets; i++) {
        close(serverContext->serviceSockets[i]);
        printMessage(SUCCESS, "Socket de service (%d) fermé correctement.\n", serverContext->serviceSockets[i]);
    }

    // Supprime la file de message IPC pour des logs
    if (serverContext->sharedResources->logsQueueId) {
        msgctl(serverContext->sharedResources->logsQueueId, IPC_RMID, NULL);
        printMessage(SUCCESS, "File de message des logs (%d) correctement supprimée du système.\n", serverContext->sharedResources->logsQueueId);
    }

    // Détruit les sémaphores
    sem_destroy(&serverContext->sharedResources->maxAllowedEating);
    sem_destroy(&serverContext->sharedResources->philosopherCreationProcess);

    for (int i = 0; i < serverContext->sharedResources->numberChopsticks; i++) {
        sem_destroy(&serverContext->sharedResources->chopsticks[i].usage);
    }
    printMessage(SUCCESS, "Sémaphores détruits correctement.\n");

    // Détache la mémoire partagée
    shmdt(serverContext->sharedResources);
    printMessage(SUCCESS, "Ressources en mémoire partagée détachées correctement.\n");


    // Libère le segment de mémoire partagée
    if (serverContext->sharedResourcesMemoryId != -1) {
        shmctl(serverContext->sharedResourcesMemoryId, IPC_RMID, NULL);
        printMessage(SUCCESS, "Mémoire partagée %d correctement supprimée du système.\n", serverContext->sharedResourcesMemoryId);
    }
}

#endif
