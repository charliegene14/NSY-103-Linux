/**
 * @file Logs.h
 * @brief Définit les constantes, types et structures pour la gestion des logs.
 *
 * Ce fichier d'en-tête regroupe l'ensemble des définitions nécessaires à la gestion des logs pour
 * l'application. Il définit plusieurs macros utilisées pour la configuration des fichiers de log
 * ainsi que deux structures permettant de représenter un message de log et les informations relatives
 * aux threads de gestion des logs.
 *
 * Les macros définies sont :
 *  - **SERVER_STATE_PATH** : Chemin d'accès au fichier de log du serveur.
 *  - **SERVER_LOG_TYPE** : Type de log associé au serveur.
 *  - **CLIENT_INFO_PREFIX** : Préfixe du chemin d'accès pour les fichiers de logs du client.
 *  - **LOG_EXTENSION** : Extension utilisée pour les fichiers de logs.
 *  - **LOG_BUFFER_SIZE** : Taille du tampon alloué pour le texte d'un log.
 *
 * Les structures définies dans ce fichier sont :
 *  - **Log** : Représente un message de log avec un type et un texte.
 *  - **LogThreadInfo** : Contient les informations nécessaires à la gestion du thread de logs,
 *    notamment l'identifiant de la file de logs et l'identifiant du processus enfant.
 *
 * L'inclusion de `<sys/types.h>` est requise pour la définition du type `pid_t`.
 *
 * Les directives de préprocesseur (#ifndef, #define, #endif) garantissent que ce fichier est inclus
 * une seule fois lors de la compilation.
 *
 * @note Ce fichier est essentiel pour assurer une gestion centralisée et cohérente des logs dans l'application.
 */

#ifndef LOGS_H
#define LOGS_H

#include <sys/types.h>

/**
 * @brief Chemin d'accès au fichier de log du serveur.
 */
#define SERVER_STATE_PATH "../logs/server.log"

/**
 * @brief Type de log associé au serveur.
 */
#define SERVER_LOG_TYPE 1

/**
 * @brief Préfixe du chemin d'accès pour les fichiers de logs du client.
 */
#define CLIENT_INFO_PREFIX "../logs/client/client_"

/**
 * @brief Extension utilisée pour les fichiers de logs.
 */
#define LOG_EXTENSION ".log"

/**
 * @brief Taille du tampon pour le texte d'un log.
 */
#define LOG_BUFFER_SIZE 256

/**
 * @brief Structure représentant un message de log dans une file de message IPC.
 *
 * Cette structure contient un champ `type` indiquant le type du log et un tableau de caractères
 * `text` servant à stocker le contenu du message de log. La taille du tableau est définie par la
 * macro `LOG_BUFFER_SIZE`.
 * 
 * Il s'agit de la même signature que `struct msgbuf`. 
 */
typedef struct {
    long type;
    char text[LOG_BUFFER_SIZE];
} Log;

/**
 * @brief Structure contenant les informations du thread de gestion des logs des processus de service.
 *
 * Cette structure regroupe :
 *  - l'identifiant de la file de logs (`logsQueueId`) utilisée pour la communication IPC,
 *  - l'identifiant du processus enfant (`childProcessId`) gérant la connexion d'un client.
 */
typedef struct {
    int logsQueueId; /**< Identifiant de la file de message IPC */
    pid_t childProcessId; /**< Identifiant du processus enfant associé */
} LogThreadInfo;


#endif