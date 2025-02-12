/**
 * @file Chopstick.h
 * @brief Définition de la structure représentant une baguette.
 *
 * Définit la structure de données `Chopstick`.
 *
 * La structure `Chopstick` comporte :
 *  - un entier `id` servant d'identifiant unique pour la baguette,
 *  - un sémaphore `usage` (de type `sem_t`) utilisé pour gérer l'accès concurrent à la baguette.
 *
 * L'inclusion de l'en-tête `<semaphore.h>` est nécessaire pour la gestion des sémaphores.
 *
 * Les directives de préprocesseur (#ifndef, #define, #endif) assurent que ce fichier d'en-tête
 * est inclus une seule fois lors de la compilation.
 *
 */

#ifndef CHOPSTICK_H
#define CHOPSTICK_H

#include <semaphore.h>

/**
 * @brief Structure représentant une baguette.
 *
 * Cette structure permet de définir une baguette avec un identifiant unique et un mécanisme
 * de synchronisation via un sémaphore pour contrôler son utilisation.
 */
typedef struct {

    int id;        /**< Identifiant de la baguette */
    sem_t usage;   /**< Semaphore pour l'utilisation de la baguette */

} Chopstick;

#endif