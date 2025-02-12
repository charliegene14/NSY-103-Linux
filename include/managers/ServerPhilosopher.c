/**
 * @file ServerPhilosopher.c
 * @brief Implémente les fonctions de gestion des philosophes côté serveur.
 *
 * Ce fichier d'implémentation contient les fonctions permettant de gérer les philosophes côté serveur. 
 * Il inclut la recherche d'un philosophe
 * à partir de son identifiant, l'attribution des baguettes (à gauche et à droite) lors de la création d'un nouveau philosophe,
 * ainsi que la mise à jour de l'état des philosophes (gestion des transitions entre THINKING, HUNGRY et EATING).
 *
 * Les fonctions implémentées dans ce fichier sont :
 *  - **getPhilosopherFromId** : Recherche un philosophe dans le tableau à partir de son identifiant.
 *  - **definePhilosopherRightChopstick** : Attribue la baguette droite pour un nouveau philosophe, en réattribuant
 *    la baguette de l'avant-dernier philosophe si nécessaire.
 *  - **createPhilosopher** : Crée et initialise un philosophe côté serveur, attribue sa baguette gauche et,
 *    pour les philosophes ultérieurs, la baguette droite via la fonction dédiée. Met également à jour le compteur
 *    limitant le nombre de philosophes pouvant manger simultanément.
 *  - **updatePhilosopher** : Met à jour l'état d'un philosophe existant en gérant la libération ou l'acquisition
 *    des baguettes en fonction de son nouvel état, et synchronise l'accès au compteur global.
 *
 * Ce module utilise les mécanismes IPC (sémaphores et files de messages) pour synchroniser l'accès aux ressources partagées.
 *
 * @note L'inclusion de divers fichiers d'entités et de gestion (Chopstick, SharedResources, Request, Response, Logs)
 * permet d'assurer une cohérence dans la gestion des ressources et des communications côté serveur.
 */

#ifndef SERVERPHILOSOPHER_C
#define SERVERPHILOSOPHER_C

#include "../maxmin_philosophers.h"
#include "../utils/sockets.h"
#include "../entities/ServerPhilosopher.h"
#include "../entities/SharedResources.h"
#include "../managers/Chopstick.c"
#include "../managers/Request.c"
#include "../managers/Response.c"
#include "../managers/Logs.c"
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>


/**
 * @brief Recherche un philosophe dans le tableau à partir de son identifiant.
 *
 * Cette fonction parcourt le tableau des philosophes côté serveur et retourne un pointeur vers
 * le philosophe dont l'identifiant correspond à celui passé en paramètre.
 *
 * @param id L'identifiant du philosophe recherché.
 * @param philosophers Tableau des philosophes côté serveur.
 * @return ServerPhilosopher* Pointeur vers le philosophe correspondant ou NULL s'il n'est pas trouvé.
 */
ServerPhilosopher *getPhilosopherFromId(int id, ServerPhilosopher *philosophers) {
    for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
        if (philosophers[i].base.id == id) {
            return &philosophers[i];
        }
    }
    return NULL;
}

/**
 * @brief Attribue la baguette droite à un nouveau philosophe.
 *
 * Cette fonction, de nature bloquante, gère l'attribution de la baguette droite pour un philosophe nouvellement créé.
 * Pour le premier philosophe, aucune attribution n'est faite. Pour les philosophes suivants, la première baguette est
 * assignée à droite du nouveau philosophe. De plus, pour le deuxième philosophe, la baguette gauche du nouveau philosophe
 * est affectée à droite du premier. Pour les philosophes suivants, la fonction réattribue la baguette droite de l'avant-dernier
 * philosophe à la nouvelle baguette créée, en s'assurant d'une synchronisation via un sémaphore.
 *
 * @param philosopher Pointeur vers le philosophe dont la baguette droite doit être définie.
 * @param sharedResources Pointeur vers les ressources partagées contenant les baguettes et les philosophes.
 */
void definePhilosopherRightChopstick(ServerPhilosopher *philosopher, SharedResources *sharedResources) {
    int lastPhilosopherId =  sharedResources->numberPhilosophers;
    
    Chopstick *firstChopstick = &sharedResources->chopsticks[0];
    logServerState(sharedResources->logsQueueId, "Assignation de la baguette 1 (adresse %p) à droite du philosophe %d...\n", firstChopstick , philosopher->base.id);
    philosopher->rightChopstick = firstChopstick;

    // Attribution de la nouvelle baguette à droite de l'avant dernier philosophe, on vérifiant l'accès de son ancienne baguette pour éviter un déplacement en mémoire pendant l'utilisation
    logServerState(sharedResources->logsQueueId, "Assignation de la baguette %d (adresse %p) à droite de l'avant denier philosophe %d...\n",  philosopher->leftChopstick->id, philosopher->leftChopstick, sharedResources->philosophers[lastPhilosopherId - 1].base.id);

    // Quand c'est le deuxième philosophe créé, le premier n'a pas de baguette à droite donc pas de sémaphore a tester
    if (philosopher->base.id == 2) {
        sharedResources->philosophers[0].rightChopstick = philosopher->leftChopstick;
    }

    else if (philosopher->base.id > 2) {
        Chopstick *previousPhlosopherOldRightChopstick = sharedResources->philosophers[lastPhilosopherId - 1].rightChopstick;
        sem_wait(&previousPhlosopherOldRightChopstick->usage);


        sharedResources->philosophers[lastPhilosopherId - 1].rightChopstick = philosopher->leftChopstick;

        sem_post(&previousPhlosopherOldRightChopstick->usage);
    }
}

/**
 * @brief Crée un philosophe côté serveur.
 *
 * Cette fonction synchronise la création d'un nouveau philosophe grâce à un sémaphore, initialise le philosophe,
 * attribue sa baguette gauche via la fonction createChopstick, et, si ce n'est pas le premier philosophe, définit
 * sa baguette droite en appelant definePhilosopherRightChopstick. Le philosophe est ensuite ajouté à la mémoire partagée,
 * et le compteur de philosophes est incrémenté. Si le nombre total de philosophes devient pair, le compteur de philosophes pouvant manger
 * est incrémenté.
 *
 * @param sharedResources Pointeur vers la structure des ressources partagées.
 * @return ServerPhilosopher Le philosophe créé et ajouté aux ressources partagées.
 */
ServerPhilosopher createPhilosopher(SharedResources *sharedResources) {
    sem_wait(&sharedResources->philosopherCreationProcess);

    // Création d'un philosophe
    int lastPhilosopherId =  sharedResources->numberPhilosophers;

    ServerPhilosopher philosopher;
    memset(&philosopher, 0, sizeof(philosopher));

    philosopher.base.id = lastPhilosopherId + 1;

    logServerState(sharedResources->logsQueueId, "Création du philosophe %d...\n", philosopher.base.id);

    // Création et Attribution de la baguette à sa gauche
    philosopher.leftChopstick = createChopstick(philosopher.base.id, sharedResources);;
    
    if (lastPhilosopherId > 0) {
        definePhilosopherRightChopstick(&philosopher, sharedResources);
    }

    // Ajout dans la mémoire partagée
    sharedResources->philosophers[lastPhilosopherId] = philosopher;
    sharedResources->numberPhilosophers += 1;

    // Incrémentation du nombre de philosophes qui peuvent manger en même 
    // Uniquement si le nouveau nombre de philosophes est un multiple de 2 (un philosophe sur deux peut manger)
    if (sharedResources->numberPhilosophers % 2 == 0) {
        sem_post(&sharedResources->maxAllowedEating);
    }

    sem_post(&sharedResources->philosopherCreationProcess);

    return philosopher;
}

/**
 * @brief Met à jour l'état d'un philosophe côté serveur.
 *
 * Cette fonction met à jour la structure d'un philosophe existant dans la mémoire partagée en fonction du nouvel état
 * (THINKING, EATING ou HUNGRY) reçu. En cas de transition de EATING à THINKING, elle libère les baguettes associées
 * et incrémente le compteur principal. Pour l'état HUNGRY, la fonction tente d'acquérir les ressources nécessaires (baguettes
 * et compteur) en tentant d'accéder aux sémaphores en deux fois: d'abord en non-bloquant, pour logguer l'attente si besoin, puis on bloque.
 *
 * @param philosopher La structure `Philosopher` contenant le nouvel état du philosophe.
 * @param serviceSocket Socket de service associé (non utilisé directement dans cette fonction).
 * @param sharedResources Pointeur vers la structure des ressources partagées.
 * @return ServerPhilosopher* Pointeur vers le philosophe mis à jour si une réponse doit être renvoyée au client,
 *         ou NULL si aucune mise à jour à notifier n'est nécessaire.
 */
ServerPhilosopher *updatePhilosopher(Philosopher philosopher, SharedResources *sharedResources) {
    ServerPhilosopher *serverPhilosopher = getPhilosopherFromId(philosopher.id, sharedResources->philosophers);

    if (!serverPhilosopher) {
        logClientInfo(sharedResources->logsQueueId, "Erreur, le philosophe à mettre à jour est introuvable dans la mémoire partagée.\n");
        return NULL;
    }   

    if (philosopher.state == THINKING) {

        // S'il est passé de EATING a THINKING, libération des baguettes et incrémentation du compteur principal
        if (serverPhilosopher->base.state == EATING) {
            
            sem_post(&serverPhilosopher->leftChopstick->usage);
            logClientInfo(sharedResources->logsQueueId, "Baguette gauche libérée\n");
            logServerState(sharedResources->logsQueueId, "Le philosophe %d libère la baguette %d à sa gauche\n", philosopher.id, serverPhilosopher->leftChopstick->id);

            sem_post(&serverPhilosopher->rightChopstick->usage);
            logClientInfo(sharedResources->logsQueueId, "Baguette droite libérée\n");
            logServerState(sharedResources->logsQueueId, "Le philosophe %d libère la baguette %d à sa droite\n", philosopher.id, serverPhilosopher->rightChopstick->id);


            sem_post(&sharedResources->maxAllowedEating);
            logClientInfo(sharedResources->logsQueueId, "Compteur libéré\n");
            logServerState(sharedResources->logsQueueId, "Le philosophe %d libère le compteur\n\n", philosopher.id);

        }

        serverPhilosopher->base = philosopher;
        logClientAction(sharedResources->logsQueueId, serverPhilosopher->base);
        return NULL;
    }
    
    if (philosopher.state == EATING) {
        serverPhilosopher->base = philosopher;
        logClientAction(sharedResources->logsQueueId, serverPhilosopher->base);
        return NULL;
    }
    

    if (philosopher.state == HUNGRY) {

        serverPhilosopher->base = philosopher;

        // On vérifie le compteur principal
        // Si ça ne passe pas on log que le philosophe patiente et il est mis en attente
     
        if (sem_trywait(&sharedResources->maxAllowedEating) == -1 && (errno == EAGAIN)) {
            logServerState(sharedResources->logsQueueId, "Le philosophe %d attend que le compteur se libère\n", philosopher.id);
            logClientInfo(sharedResources->logsQueueId, "En attente de pouvoir manger... \n");
            sem_wait(&sharedResources->maxAllowedEating);
        }
        
        int allowedEating;
        sem_getvalue(&sharedResources->maxAllowedEating, &allowedEating);
        logServerState(sharedResources->logsQueueId, "Le philosophe %d s'ajoute au compteur (dispo restante : %d)\n", philosopher.id, allowedEating);
        
        // Une fois le premier sémaphore pris, on vérifie les deux baguettes

        // On vérifie une première baguette (non bloquant)
        // Si ça ne passe on log que le philosophe patiente et il est mis en attente
        if (sem_trywait(&serverPhilosopher->leftChopstick->usage) == -1 && (errno == EAGAIN)) {
            logServerState(sharedResources->logsQueueId, "Le philosophe %d attend que la baguette %d à sa gauche se libère\n", philosopher.id, serverPhilosopher->leftChopstick->id);
            logClientInfo(sharedResources->logsQueueId, "En attente de la baguette gauche...\n");
            sem_wait(&serverPhilosopher->leftChopstick->usage);
        }
        
      
        logServerState(sharedResources->logsQueueId, "Le philosophe %d prend la baguette %d à sa gauche\n", philosopher.id, serverPhilosopher->leftChopstick->id);


        // On vérifie la seconde baguette (non bloquant)
        // Si ça ne passe on log que le philosophe patiente et il est mis en attente
        if (sem_trywait(&serverPhilosopher->rightChopstick->usage) == -1 && (errno == EAGAIN)) {
            logServerState(sharedResources->logsQueueId, "Le philosophe %d attend que la baguette %d à sa droite se libère\n", philosopher.id, serverPhilosopher->rightChopstick->id);
            logClientInfo(sharedResources->logsQueueId, "En attente de la baguette droite...\n");
            sem_wait(&serverPhilosopher->rightChopstick->usage);
        }
        
        logServerState(sharedResources->logsQueueId, "Le philosophe %d prend la baguette %d à sa droite\n", philosopher.id, serverPhilosopher->rightChopstick->id);

        // Ensuite on peut passer à l'état EATING et envoyer la réponse au client qui attend une réponse
        serverPhilosopher->base.state = EATING;
        serverPhilosopher->base.stateTimer = 0;

        return serverPhilosopher;
    }

    return NULL;
}

#endif