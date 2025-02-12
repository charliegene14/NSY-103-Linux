/**
 * @file Logs.c
 * @brief Implémente les fonctions de gestion des logs et des chemins des fichiers de log.
 *
 * Ce fichier d'implémentation fournit des fonctions pour :
 *  - Initialiser une file de messages IPC destinée à la gestion des logs.
 *  - Générer les chemins des fichiers de log pour les clients et le serveur.
 *  - Envoyer des messages de log depuis le client et le serveur via des files de messages IPC.
 *
 * Les fonctions définies dans ce fichier sont :
 *  - **initLogsQueue()** : Initialise une file de messages IPC pour les logs et retourne son identifiant.
 *  - **getClientInfoFilepath(pid_t pid)** : Construit et retourne le chemin du fichier de log associé à un client, basé sur son PID.
 *  - **getServerStateFilePath()** : Retourne le chemin complet du fichier de log du serveur.
 *  - **logClientInfo(int logsQueueId, char *message)** : Envoie un message de log dans la file de logs pour un client.
 *  - **logClientAction(int logsQueueId, Philosopher philosopher)** : Formate et envoie un message de log décrivant l'action d'un philosophe (penser ou manger).
 *  - **logServerState(int logsQueueId, const char *format, ...)** : Formate et envoie un message de log depuis le serveur à l'aide d'arguments variables.
 *
 * Les inclusions nécessaires dans ce fichier sont :
 *  - "../utils/files.h" pour la gestion des chemins de fichiers.
 *  - "../entities/Philosopher.h" pour la définition de la structure `Philosopher`.
 *  - "../entities/Logs.h" pour la définition de la structure `Log` et des constantes associées.
 *  - <sys/msg.h> pour la gestion des files de messages IPC.
 *  - <string.h>, <unistd.h>, <stdio.h>, <stdlib.h> et <stdarg.h> pour diverses fonctions utilitaires.
 *
 */
#ifndef LOGS_C
#define LOGS_C

#include "../utils/files.h"
#include "../entities/Philosopher.h"
#include "../entities/Logs.h"
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/**
 * @brief Initialise une file de messages IPC pour la gestion des logs.
 *
 * Cette fonction crée une file de messages privée (IPC_PRIVATE) avec les permissions en lecture/écriture
 * pour le propriétaire (0600) et retourne l'identifiant de la file créée.
 *
 * @return int L'identifiant de la file de messages IPC.
 */
int initLogsQueue() {
    return msgget(IPC_PRIVATE, IPC_CREAT | 0600);
}

/**
 * @brief Construit le chemin complet du fichier de log associé à un client.
 *
 * Cette fonction formate un nom de fichier en utilisant le préfixe `CLIENT_INFO_PREFIX`, le PID du client,
 * et l'extension `LOG_EXTENSION`, puis appelle la fonction `getFilePath` pour obtenir le chemin complet.
 *
 * @param pid Le PID du client.
 * @return char* Le chemin complet du fichier de log du client.
 */
char *getClientInfoFilepath(pid_t pid) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s%d%s", CLIENT_INFO_PREFIX, pid, LOG_EXTENSION);

    char *filePath = getFilePath(filename);

    return filePath;
}

/**
 * @brief Retourne le chemin complet du fichier de log du serveur.
 *
 * Cette fonction utilise la constante `SERVER_STATE_PATH` pour obtenir le chemin complet du fichier de log
 * du serveur via la fonction `getFilePath`.
 *
 * @return char* Le chemin complet du fichier de log du serveur.
 */
char *getServerStateFilePath() {
    char *filePath = getFilePath(SERVER_STATE_PATH);

    return filePath;
}

/**
 * @brief Envoie un message de log dans la file de logs pour un client.
 *
 * Cette fonction initialise une structure `Log`, définit son type avec le PID courant (afin que le bon thread
 * réceptionne le log) et copie le message dans le champ `text` en respectant la taille maximale. Le message est ensuite
 * envoyé via la file de messages IPC.
 * 
 *
 * @param logsQueueId L'identifiant de la file de messages IPC utilisée pour les logs.
 * @param message Le message de log à envoyer.
 */
void logClientInfo(int logsQueueId, char *message) {

    Log log;
    memset(&log, 0, sizeof(Log));
    log.type = (long) getpid(); // PID comme type pour que le bon thread récéptionne le log
    strncpy(log.text, message, LOG_BUFFER_SIZE - 1); 
    
    // Pas besoin de gérer l'erreur, si ça ne passe pas on essai de log au prochain
    msgsnd(logsQueueId, &log, LOG_BUFFER_SIZE, 0);
}

/**
 * @brief Formate et envoie un message de log décrivant l'action d'un philosophe.
 *
 * Cette fonction détermine l'action en cours du philosophe (penser ou manger) et lui associe une mise en forme
 * colorée pour la sortie console. Elle alloue dynamiquement un buffer pour formater le message, qui indique
 * l'identifiant du philosophe, l'action en cours et la durée restante de l'état. Le message est ensuite envoyé
 * en appelant `logClientInfo`, puis le buffer est libéré.
 *
 * @param logsQueueId L'identifiant de la file de messages IPC utilisée pour les logs.
 * @param philosopher La structure `Philosopher` contenant les informations sur le philosophe.
 */
void logClientAction(int logsQueueId, Philosopher philosopher) {
    const char *action;

    // "Le philophe X est en train de Y : Z"
    switch (philosopher.state) {
        case THINKING:
            action = "\x1B[1;34mpenser\x1B[0m :"; // mise en gras de l'action dans la même couleur
            break;

        case EATING:
            action = "\x1B[1;32mmanger\x1B[0m :"; // mise en gras de l'action dans la même couleur
            break;
            
        default:
            break;
    }

    char *message = (char *) malloc(512);

    snprintf(
        message,
        512,
        "%s %d %s %s %d%s",
        "Le philosophe", 
        philosopher.id, 
        "est en train de", 
        action,
        philosopher.stateTimer,
        " secondes.\n"
    );

    logClientInfo(logsQueueId, message);
    free(message);
}

/**
 * @brief Formate et envoie un message de log depuis le serveur.
 *
 * Cette fonction utilise un format de chaîne de caractères et des arguments variables pour construire le message
 * de log. Le message est formaté dans une structure `Log` avec le type défini par `SERVER_LOG_TYPE` et envoyé
 * via la file de messages IPC.
 *
 * @param logsQueueId L'identifiant de la file de messages IPC utilisée pour les logs.
 * @param format La chaîne de format utilisée pour le message de log.
 * @param ... Arguments variables correspondant au format.
 */
void logServerState(int logsQueueId, const char *format, ...) {

    Log log;
    memset(&log, 0, sizeof(Log));
    log.type = SERVER_LOG_TYPE;

    va_list args;
    va_start(args, format);
    vsnprintf(log.text, LOG_BUFFER_SIZE, format, args);
    va_end(args);

    msgsnd(logsQueueId, &log, LOG_BUFFER_SIZE, 0);
}

#endif