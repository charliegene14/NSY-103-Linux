/**
 * @file SharedResources.h
 * @brief Définit la structure regroupant les ressources partagées utilisées par le serveur.
 *
 * Ce fichier d'en-tête définit la structure `SharedResources` qui centralise l'ensemble des ressources
 * partagées par le serveur. Cette structure inclut les
 * ressources relatives aux philosophes, aux baguettes, ainsi que les sémaphores de contrôle d'accès
 * aux ressources critiques et l'identifiant de la file de logs pour la communication inter-processus.
 *
 * Les éléments définis dans cette structure sont :
 *  - **maxAllowedEating** : Sémaphore limitant le nombre maximum de philosophes pouvant manger simultanément.
 *    Ce nombre est défini comme le plancher du total des philosophes divisé par 2.
 *  - **philosopherCreationProcess** : Sémaphore permettant de sécuriser la création concurrente de philosophes,
 *    en assurant une synchronisation lors de l'accès à la mémoire partagée.
 *  - **philosophers** : Tableau de structures `ServerPhilosopher` représentant les philosophes côté serveur.
 *    La taille maximale de ce tableau est définie par la constante `MAX_PHILOSOPHERS`.
 *  - **numberPhilosophers** : Nombre actuel de philosophes présents dans la mémoire partagée.
 *  - **chopsticks** : Tableau de structures `Chopstick` représentant les baguettes utilisées par les philosophes.
 *    La taille maximale de ce tableau est également définie par `MAX_PHILOSOPHERS`.
 *  - **numberChopsticks** : Nombre actuel de baguettes dans la mémoire partagée.
 *  - **logsQueueId** : Identifiant de la file de logs utilisée pour la communication inter-processus dans la gestion des logs.
 *
 * Les inclusions nécessaires sont :
 *  - "../maxmin_philosophers.h" pour la définition de la constante `MAX_PHILOSOPHERS`.
 *  - "../entities/ServerPhilosopher.h" pour la définition de la structure `ServerPhilosopher`.
 *  - "../entities/Chopstick.h" pour la définition de la structure `Chopstick`.
 *
 * Les directives de préprocesseur (#ifndef, #define, #endif) garantissent que ce fichier est inclus une seule fois
 * lors de la compilation.
 */

#ifndef SHARED_RESOURCES_H
#define SHARED_RESOURCES_H

#include "../maxmin_philosophers.h"
#include "../entities/ServerPhilosopher.h"
#include "../entities/Chopstick.h"

/**
 * @brief Structure regroupant les ressources partagées du serveur.
 *
 * Cette structure centralise les différents éléments partagés entre les processus du serveur, permettant
 * de gérer la synchronisation et la communication inter-processus.
 */
typedef struct {

    /**
     * @brief Sémaphore limitant le nombre maximum de philosophes pouvant manger simultanément.
     *
     * Ce sémaphore est initialisé à la valeur du plancher du nombre total de philosophes divisé par 2,
     * garantissant qu'au maximum la moitié des philosophes puissent manger en même temps.
     */
    sem_t maxAllowedEating;

    /**
     * @brief Sémaphore permettant de sécuriser la création concurrente de philosophes.
     *
     * Ce sémaphore assure que l'accès à la mémoire partagée lors de la création de nouveaux philosophes
     * est correctement synchronisé lorsque plusieurs processus tentent de créer un philosophe simultanément.
     */
    sem_t philosopherCreationProcess;
    
    /**
     * @brief Tableau des philosophes côté serveur.
     *
     * Ce tableau contient les structures `ServerPhilosopher` représentant chaque philosophe.
     * La taille maximale du tableau est définie par la constante `MAX_PHILOSOPHERS`.
     */
    ServerPhilosopher philosophers[MAX_PHILOSOPHERS];

    /**
     * @brief Nombre actuel de philosophes dans la mémoire partagée.
     */
    int numberPhilosophers;
    
    /**
     * @brief Tableau des baguettes.
     *
     * Ce tableau contient les structures `Chopstick` représentant les baguettes utilisées par les philosophes.
     * La taille maximale du tableau est définie par la constante `MAX_PHILOSOPHERS`.
     */
    Chopstick chopsticks[MAX_PHILOSOPHERS];

    /**
     * @brief Nombre actuel de baguettes dans la mémoire partagée.
     */
    int numberChopsticks;

    /**
     * @brief Identifiant de la file de logs.
     *
     * Cet identifiant est utilisé pour la communication inter-processus dans la gestion des logs.
     */
    int logsQueueId;

} SharedResources;


#endif