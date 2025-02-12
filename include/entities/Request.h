/**
 * @file Request.h
 * @brief Définit les types et structures pour la gestion des requêtes adressées au serveur.
 *
 * Ce fichier d'en-tête définit l'énumération `RequestType` qui répertorie les différents types de requêtes
 * pouvant être envoyées au serveur dans le cadre de la gestion des philosophes, ainsi que la structure `Request`
 * qui encapsule ces requêtes.
 *
 * La définition de `RequestType` inclut :
 *  - **REQUEST_CREATE** : Requête pour demander au serveur de créer un nouveau philosophe.
 *  - **REQUEST_UPDATE** : Requête pour demander au serveur de mettre à jour un philosophe existant.
 *
 * La structure `Request` comporte :
 *  - un champ `type` de type `RequestType` indiquant la nature de la requête,
 *  - un champ `philosopher` de type `Philosopher`, contenant les informations du philosophe concerné.
 *
 * L'inclusion de "Philosopher.h" est nécessaire pour accéder à la définition de la structure `Philosopher`.
 *
 * Les directives de préprocesseur (#ifndef, #define, #endif) garantissent que ce fichier d'en-tête est inclus
 * une seule fois lors de la compilation.
 */

#ifndef REQUEST_H
#define REQUEST_H

#include "Philosopher.h"

/**
 * @brief Énumération des types de requêtes adressées au serveur.
 *
 * Cette énumération définit les différents types de requêtes qui peuvent être envoyées par les
 * processus clients pour interagir avec le serveur dans la gestion des philosophes.
 */
typedef enum {
    
    /**
     * @brief Requête pour demander au serveur de créer un nouveau philosophe.
     */
    REQUEST_CREATE,

    /**
     * @brief Requête pour demander au serveur de mettre à jour un philosophe existant.
     */
    REQUEST_UPDATE,

} RequestType;

/**
 * @brief Structure représentant une requête adressée au serveur.
 *
 * Cette structure encapsule une requête envoyée au serveur, incluant le type de requête
 * et les informations du philosophe concerné.
 */
typedef struct {
    RequestType type;       /**< Type de la requête (création ou mise à jour) */
    Philosopher philosopher;/**< Structure contenant les informations du philosophe */
} Request;



#endif