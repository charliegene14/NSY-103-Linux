/**
 * @file ServerPhilosopher.h
 * @brief Définit la structure représentant un philosophe côté serveur.
 *
 * Ce fichier d'en-tête définit la structure `ServerPhilosopher` qui permet de représenter un philosophe
 * dans le contexte du serveur. La structure intègre la base d'un philosophe ainsi que les pointeurs vers
 * les baguettes gauche et droite, lesquelles sont stockées en mémoire partagée.
 *
 * La structure `ServerPhilosopher` contient les champs suivants :
 *  - **base** : Structure `Philosopher` contenant les informations de base du philosophe (identifiant, état, et timer).
 *  - **leftChopstick** : Pointeur vers la baguette gauche en mémoire partagée.
 *  - **rightChopstick** : Pointeur vers la baguette droite en mémoire partagée.
 *
 * Les inclusions nécessaires pour ce fichier d'en-tête sont :
 *  - "Philosopher.h" pour la définition de la structure `Philosopher`.
 *  - "Chopstick.h" pour la définition de la structure `Chopstick`.
 *  - <pthread.h> pour la gestion des threads.
 *  - <stdio.h> pour les fonctions d'entrée/sortie.
 *  - <sys/types.h> pour les types de données système.
 *
 * Les directives de préprocesseur (#ifndef, #define, #endif) garantissent que ce fichier est inclus une seule fois
 * lors de la compilation.
 *
 * @note Bien que la structure `ServerPhilosopher` soit similaire à celle utilisée côté client, elle intègre
 * explicitement des références aux baguettes en mémoire partagée, ce qui est essentiel pour la gestion synchronisée
 * des ressources côté serveur.
 */

#ifndef SERVERPHILOSOPHER_H
#define SERVERPHILOSOPHER_H

#include "Philosopher.h"
#include "Chopstick.h"

#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>

/**
 * @brief Structure représentant un philosophe côté serveur.
 *
 * Cette structure permet de représenter un philosophe dans le contexte du serveur en intégrant :
 *  - La base du philosophe, contenant les informations essentielles (identifiant, état et timer).
 *  - Un pointeur vers la baguette gauche en mémoire partagée.
 *  - Un pointeur vers la baguette droite en mémoire partagée.
 */
typedef struct {

    /**
     * @brief Base d'un philosophe.
     *
     * La structure `Philosopher` contient les informations de base du philosophe, identiques à celles utilisées côté client.
     */
    Philosopher base;

    /**
     * @brief Pointeur vers la baguette gauche en mémoire partagée.
     *
     * Ce pointeur permet d'accéder à la baguette gauche du philosophe dans l'espace de mémoire partagée.
     */
    Chopstick *leftChopstick;

    /**
     * @brief Pointeur vers la baguette droite en mémoire partagée.
     *
     * Ce pointeur permet d'accéder à la baguette droite du philosophe dans l'espace de mémoire partagée.
     */
    Chopstick *rightChopstick;

} ServerPhilosopher;


#endif