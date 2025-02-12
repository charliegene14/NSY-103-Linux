/**
 * @file Response.c
 * @brief Implémente les fonctions de création des réponses envoyées par le serveur.
 *
 * Ce fichier d'implémentation fournit deux fonctions permettant de générer des réponses en fonction des informations
 * relatives à un philosophe. Ces réponses sont ensuite envoyées par le serveur au client pour indiquer l'état ou
 * les actions effectuées.
 *
 * Les fonctions définies sont :
 *  - **createResponse(Philosopher philosopher)** : Crée une réponse de type RESPONSE_CREATE, initialisée à zéro,
 *    puis enregistre les informations du philosophe.
 *  - **updateResponse(Philosopher philosopher)** : Crée une réponse de type RESPONSE_UPDATE, initialisée à zéro,
 *    puis enregistre les informations mises à jour du philosophe.
 *
 * Dans chaque fonction, la structure `Response` est initialisée à zéro à l'aide de `memset` afin d'assurer
 * une initialisation propre, avant d'affecter les valeurs correspondantes.
 *
 * Les inclusions nécessaires dans ce fichier sont :
 *  - "../entities/Response.h" pour la définition de la structure `Response` et de l'énumération `ResponseType`.
 *  - "../entities/Philosopher.h" pour la définition de la structure `Philosopher`.
 *  - <string.h> pour l'utilisation de la fonction `memset`.
 *
 * Les directives de préprocesseur (#ifndef, #define, #endif) garantissent que ce fichier est inclus une seule fois lors
 * de la compilation.
 */

#ifndef RESPONSE_C
#define RESPONSE_C

#include "../entities/Response.h"
#include "../entities/Philosopher.h"
#include <string.h>

/**
 * @brief Crée une réponse de type RESPONSE_CREATE.
 *
 * Cette fonction initialise une structure `Response` à zéro avec `memset`, définit le type de réponse
 * à RESPONSE_CREATE et y copie la structure `Philosopher` passée en paramètre.
 *
 * @param philosopher La structure `Philosopher` contenant les informations du philosophe.
 * @return Response La réponse initialisée de type RESPONSE_CREATE.
 */
Response createResponse(Philosopher philosopher) {
    Response response;
    memset(&response, 0, sizeof(response));
    
    response.type = RESPONSE_CREATE;
    response.philosopher = philosopher;
    return response;
}

/**
 * @brief Crée une réponse de type RESPONSE_UPDATE.
 *
 * Cette fonction initialise une structure `Response` à zéro avec `memset`, définit le type de réponse
 * à RESPONSE_UPDATE et y copie la structure `Philosopher` passée en paramètre.
 *
 * @param philosopher La structure `Philosopher` contenant les informations mises à jour du philosophe.
 * @return Response La réponse initialisée de type RESPONSE_UPDATE.
 */
Response updateResponse(Philosopher philosopher) {
    Response response;
    memset(&response, 0, sizeof(response));

    response.type = RESPONSE_UPDATE;
    response.philosopher = philosopher;
    return response;
}

#endif
