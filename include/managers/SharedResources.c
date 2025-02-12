/**
 * @file SharedResources.c
 * @brief Implémente la fonction d'attachement et d'initialisation des ressources partagées.
 *
 * Ce fichier d'implémentation fournit la fonction `attachSharedResources` qui permet d'attacher un segment
 * de mémoire partagée contenant la structure `SharedResources` et d'initialiser ses champs aux valeurs par défaut.
 *
 * La fonction effectue les opérations suivantes :
 *  - Attache le segment de mémoire partagée identifié par `sharedResourcesMemoryId` en utilisant `shmat`.
 *  - Initialise le sémaphore `maxAllowedEating` à 0, limitant ainsi initialement le nombre de philosophes pouvant manger simultanément.
 *  - Initialise le sémaphore `philosopherCreationProcess` à 1, afin de sécuriser la création concurrente des philosophes.
 *  - Réinitialise les compteurs de philosophes (`numberPhilosophers`) et de baguettes (`numberChopsticks`) à 0.
 *  - Initialise l'identifiant de la file de logs (`logsQueueId`) à 0.
 *
 * Les inclusions nécessaires dans ce fichier sont :
 *  - "../entities/SharedResources.h" pour la définition de la structure `SharedResources`.
 *  - <stdlib.h> pour les fonctions de la bibliothèque standard.
 *  - <math.h> pour les fonctions mathématiques (bien que non utilisées directement dans cette fonction).
 *  - <sys/shm.h> pour la gestion de la mémoire partagée.
 *  - <string.h> pour les opérations sur la mémoire.
 *
 */

#ifndef SHAREDRESOURCES_C
#define SHAREDRESOURCES_C

#include "../entities/SharedResources.h"
#include <stdlib.h>
#include <math.h>
#include <sys/shm.h>
#include <string.h>

/**
 * @brief Attache et initialise les ressources partagées.
 *
 * Cette fonction attache un segment de mémoire partagée identifié par `sharedResourcesMemoryId` et
 * retourne un pointeur vers une structure `SharedResources`. Elle initialise ensuite les sémaphores et
 * compteurs de la structure afin de préparer l'utilisation des ressources partagées par l'application.
 *
 * @param sharedResourcesMemoryId L'identifiant du segment de mémoire partagée.
 * @return SharedResources* Pointeur vers la structure `SharedResources` attachée.
 *
 * @note Cette fonction doit être appelée après la création du segment de mémoire partagée.
 */
SharedResources *attachSharedResources(int sharedResourcesMemoryId) {

    SharedResources *sharedResources = (SharedResources *) shmat(sharedResourcesMemoryId, NULL, 0);
    
    sem_init(&sharedResources->maxAllowedEating, 1, 0);
    sem_init(&sharedResources->philosopherCreationProcess, 1, 1);
    sharedResources->numberPhilosophers = 0;
    sharedResources->numberChopsticks = 0;
    sharedResources->logsQueueId = 0;
    
    return sharedResources;
}

#endif