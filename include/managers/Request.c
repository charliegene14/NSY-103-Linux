/**
 * @file Request.c
 * @brief Implémente les fonctions de création des requêtes adressées au serveur.
 *
 * Ce fichier d'implémentation fournit deux fonctions pour faciliter la gestion des requêtes :
 *  - **createRequest()** : Crée et initialise une requête de type REQUEST_CREATE, utilisée pour demander
 *    la création d'un nouveau philosophe.
 *  - **updateRequest(Philosopher philosopher)** : Crée et initialise une requête de type REQUEST_UPDATE,
 *    en intégrant la structure `Philosopher` pour mettre à jour un philosophe existant.
 *
 * Pour chaque fonction, la structure `Request` est initialisée à zéro à l'aide de `memset` afin d'assurer
 * une initialisation propre, avant d'affecter le type de la requête et, le cas échéant, la structure du philosophe.
 *
 * Les inclusions nécessaires dans ce fichier sont :
 *  - "../entities/Request.h" pour la définition de la structure `Request` et de l'énumération `RequestType`.
 *  - "../entities/Philosopher.h" pour la définition de la structure `Philosopher`.
 *  - <string.h> pour l'utilisation de la fonction `memset`.
 *
 * Les directives de préprocesseur (#ifndef, #define, #endif) assurent que ce fichier d'implémentation est inclus
 * une seule fois lors de la compilation.
 */
#ifndef REQUEST_C
#define REQUEST_C

#include "../entities/Request.h"
#include "../entities/Philosopher.h"
#include <string.h>

/**
 * @brief Crée une requête de création d'un nouveau philosophe.
 *
 * Cette fonction initialise une structure `Request` à zéro et définit son type à REQUEST_CREATE.
 *
 * @return Request La requête initialisée pour créer un nouveau philosophe.
 */
Request createRequest() {

    Request request;
    memset(&request, 0, sizeof(request));

    request.type = REQUEST_CREATE;

    return request;
}

/**
 * @brief Crée une requête de mise à jour pour un philosophe existant.
 *
 * Cette fonction initialise une structure `Request` à zéro, définit son type à REQUEST_UPDATE,
 * et y affecte la structure `Philosopher` passée en paramètre.
 *
 * @param philosopher La structure `Philosopher` contenant les informations à mettre à jour.
 * @return Request La requête initialisée pour mettre à jour le philosophe.
 */
Request updateRequest(Philosopher philosopher) {
    Request request;
    memset(&request, 0, sizeof(request));

    request.type = REQUEST_UPDATE;
    request.philosopher = philosopher;

    return request;
}

#endif
