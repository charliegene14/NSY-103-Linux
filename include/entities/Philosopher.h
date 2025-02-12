/**
 * @file Philosopher.h
 * @brief Définit les types et structures de base pour la gestion des philosophes.
 *
 * Ce fichier d'en-tête définit l'énumération `PhilosopherState` qui représente les différents états
 * possibles d'un philosophe, ainsi que la structure `Philosopher` qui contient les informations de base
 * d'un philosophe. Ces définitions sont utilisées pour modéliser le comportement des philosophes.
 *
 * Les inclusions de `<pthread.h>` et `<stdio.h>` préparent l'utilisation des threads et des fonctions
 * d'entrée/sortie.
 *
 * Les directives de préprocesseur (#ifndef, #define, #endif) garantissent que ce fichier d'en-tête est inclus
 * une seule fois lors de la compilation.
 */

#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#include <pthread.h>
#include <stdio.h>

/**
 * @brief Représente l'état d'un philosophe.
 *
 * Cette énumération définit les différents états possibles pour un philosophe :
 *  - THINKING : le philosophe est en train de penser.
 *  - HUNGRY   : le philosophe a faim et souhaite manger.
 *  - EATING   : le philosophe est en train de manger.
 */
typedef enum {
    THINKING, /**< Le philosophe est en train de penser */
    HUNGRY,   /**< Le philosophe est affamé */
    EATING    /**< Le philosophe est en train de manger */
} PhilosopherState;

/**
 * @brief Structure représentant la base d'un philosophe.
 *
 * Cette structure contient les informations essentielles pour représenter un philosophe :
 *  - **id** : Identifiant unique attribué par le serveur, qui sert également d'index.
 *  - **state** : État actuel du philosophe, défini par l'énumération `PhilosopherState`.
 *  - **stateTimer** : Décompte du temps associé à l'action en cours (penser ou manger).
 */
typedef struct {

    /**
     * @brief Identifiant donné par le serveur.
     *
     * Sert également d'index dans le tableau des philosophes en mémoire partagée (index = id - 1).
     */
    int id;

    /**
     * @brief État actuel du philosophe.
     */
    PhilosopherState state;

    /**
     * @brief Décompte du temps associé à l'état actuel.
     *
     * Ce compteur indique la durée restante pour l'action en cours, pour penser et 
     * manger.
     */
    int stateTimer;

} Philosopher;


#endif