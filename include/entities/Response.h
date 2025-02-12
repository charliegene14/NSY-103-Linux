/**
 * @file Response.h
 * @brief Définit les types et structures pour la gestion des réponses du serveur.
 *
 * Ce fichier d'en-tête définit l'énumération `ResponseType` qui répertorie les différents types de réponses
 * envoyées par le serveur dans le cadre de la gestion des philosophes, ainsi que la structure `Response` qui
 * encapsule ces réponses.
 *
 * L'énumération `ResponseType` inclut :
 *  - **RESPONSE_CREATE** : Réponse à une requête de création d'un nouveau philosophe.
 *  - **RESPONSE_UPDATE** : Réponse à une requête de mise à jour d'un philosophe existant.
 *
 * La structure `Response` contient :
 *  - un champ `type` de type `ResponseType` indiquant le type de réponse,
 *  - un champ `philosopher` de type `Philosopher` contenant les informations du philosophe concerné.
 *
 * L'inclusion de "Philosopher.h" est nécessaire pour accéder à la définition de la structure `Philosopher`.
 *
 * Les directives de préprocesseur (#ifndef, #define, #endif) garantissent que ce fichier d'en-tête est inclus
 * une seule fois lors de la compilation.
 */

#ifndef RESPONSE_H
#define RESPONSE_H

#include "Philosopher.h"

/**
 * @brief Énumération des types de réponses envoyées par le serveur.
 *
 * Cette énumération définit les différents types de réponses possibles lors de l'interaction avec le serveur
 * pour la gestion des philosophes.
 */
typedef enum {
    RESPONSE_CREATE, /**< Réponse à une requête de création d'un nouveau philosophe. */
    RESPONSE_UPDATE  /**< Réponse à une requête de mise à jour d'un philosophe existant. */
} ResponseType;

/**
 * @brief Structure représentant une réponse envoyée par le serveur.
 *
 * Cette structure encapsule une réponse envoyée par le serveur, incluant le type de réponse et les informations
 * du philosophe concerné.
 */
typedef struct {
    ResponseType type;       /**< Type de la réponse (création ou mise à jour) */
    Philosopher philosopher; /**< Informations du philosophe concerné */
} Response;



#endif