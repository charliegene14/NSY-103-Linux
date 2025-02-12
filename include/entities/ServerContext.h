/**
 * @file ServerContext.h
 * @brief Définit la structure de contexte du serveur.
 *
 * Ce fichier d'en-tête définit la structure `ServerContext` qui regroupe l'ensemble des ressources et informations
 * nécessaires au fonctionnement du serveur dans l'application. Leur centralisation facilite leur gestion notamment lors de la destruction.
 *
 * La structure `ServerContext` contient les champs suivants :
 *  - **serverSocket** : Socket principal du serveur.
 *  - **sharedResourcesMemoryId** : Identifiant de la mémoire partagée utilisée pour stocker les ressources communes.
 *  - **sharedResources** : Pointeur vers la structure `SharedResources` regroupant les ressources partagées (baguettes,
 *    philosophes, file de messages de logs, etc).
 *  - **serviceSockets** : Tableau des sockets de service. Sa taille maximale est définie par la constante
 *    `MAX_PHILOSOPHERS` (définie dans "maxmin_philosophers.h").
 *  - **numberServiceSockets** : Nombre actuel de sockets de service utilisés.
 *  - **clientsProcessIds** : Tableau contenant les PID des processus clients connectés. Sa taille maximale est également
 *    définie par `MAX_PHILOSOPHERS`.
 *  - **numberClients** : Nombre actuel de clients connectés.
 *
 * Les inclusions nécessaires sont :
 *  - "../maxmin_philosophers.h" pour la définition de la constante `MAX_PHILOSOPHERS`.
 *  - "SharedResources.h" pour la définition de la structure `SharedResources`.
 *  - `<sys/types.h>` pour la définition du type `pid_t`.
 *  - `<stdlib.h>` pour les fonctions et définitions de la bibliothèque standard.
 *
 * Les directives de préprocesseur (#ifndef, #define, #endif) garantissent que ce fichier est inclus une seule fois lors
 * de la compilation.
 */

#ifndef SERVERRESOURCES_H
#define SERVERRESOURCES_H

#include "../maxmin_philosophers.h"
#include "SharedResources.h"
#include <sys/types.h>
#include <stdlib.h>

/**
 * @brief Structure regroupant les ressources et informations du serveur.
 *
 * Cette structure permet de centraliser l'ensemble des éléments nécessaires au fonctionnement du serveur,
 * notamment la gestion des sockets, de la mémoire partagée et le suivi des processus clients.
 */
typedef struct {

    /**
     * @brief Socket principal du serveur.
     *
     * Ce socket est utilisé pour accepter les connexions entrantes des clients.
     */
    int serverSocket;

    /**
     * @brief Identifiant de la mémoire partagée.
     *
     * Cet identifiant permet d'accéder à la mémoire partagée qui contient les ressources communes.
     */
    int sharedResourcesMemoryId;

    /**
     * @brief Ressources partagées.
     *
     * Pointeur vers la structure `SharedResources` qui regroupe les ressources partagées.
     */
    SharedResources *sharedResources;

    /**
     * @brief Tableau des sockets de service.
     *
     * Chaque socket de service est associé à un client.
     * La taille maximale de ce tableau est définie par la constante `MAX_PHILOSOPHERS`.
     */
    int serviceSockets[MAX_PHILOSOPHERS];

    /**
     * @brief Nombre de sockets de services utilisés.
     */
    int numberServiceSockets;

    /**
     * @brief Tableau des PID des processus clients.
     *
     * Ce tableau contient l'identifiant des processus clients connectés.
     * Sa taille maximale est définie par `MAX_PHILOSOPHERS`.
     */
    pid_t clientsProcessIds[MAX_PHILOSOPHERS];

    /**
     * @brief Nombre de clients connectés.
     */
    size_t numberClients;

} ServerContext;

#endif