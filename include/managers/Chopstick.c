/**
 * @file Chopstick.c
 * @brief Contient l'implémentation de la fonction de création d'une baguette.
 *
 * Ce fichier d'implémentation fournit la fonction `createChopstick` qui initialise une baguette avec
 * l'identifiant fourni, met à zéro ses champs, initialise son sémaphore d'utilisation en mode inter-processus,
 * copie la baguette dans la mémoire partagée, et envoie un message de log pour indiquer sa création.
 *
 * Les inclusions nécessaires dans ce fichier sont :
 *  - "../entities/Chopstick.h" pour la définition de la structure `Chopstick`.
 *  - "../entities/SharedResources.h" pour la définition de la structure `SharedResources`.
 *  - "../managers/Logs.c" pour la gestion des logs côté serveur.
 *  - <semaphore.h> pour la gestion des sémaphores.
 *  - <string.h> pour les fonctions `memset` et `memcpy`.
 * 
 */

#ifndef CHOPSTICK_C
#define CHOPSTICK_C

#include "../entities/Chopstick.h"
#include "../entities/SharedResources.h"
#include "../managers/Logs.c"
#include <semaphore.h>
#include <string.h>

/**
 * @brief Crée et initialise une baguette.
 *
 * Cette fonction réalise les opérations suivantes :
 *  - Initialise une baguette avec l'identifiant fourni et remet à zéro ses champs.
 *  - Initialise le sémaphore d'utilisation de la baguette en mode inter-processus avec une valeur initiale de 1.
 *  - Copie la baguette dans la mémoire partagée à l'index correspondant (id - 1) pour assurer une gestion cohérente.
 *  - Envoie un message de log pour notifier la création de la baguette, en indiquant son identifiant et son adresse
 *    dans la mémoire partagée.
 *
 * @param id L'identifiant unique de la baguette.
 * @param sharedResources Pointeur vers la structure `SharedResources` contenant les baguettes et la file de logs.
 * @return Chopstick* Pointeur vers la baguette nouvellement créée dans la mémoire partagée.
 */
Chopstick *createChopstick(int id, SharedResources *sharedResources) {
    Chopstick chopstick;
    memset(&chopstick, 0, sizeof(chopstick));

    chopstick.id = id;
    sem_init(&chopstick.usage, 1, 1);

    // Ajout dans la mémoire partagée
    // Copie avec memcpy pour être certain copier les données à la bonne adresse
    memcpy(&sharedResources->chopsticks[id - 1], &chopstick, sizeof(Chopstick));
    logServerState(sharedResources->logsQueueId, "Baguette %d créée (adresse %p)...\n", id, &sharedResources->chopsticks[id - 1]);

    return &sharedResources->chopsticks[id - 1];
}

#endif