/**
 * @file random.h
 * @brief Fournit des fonctions pour l'initialisation du générateur de nombres aléatoires et la génération de nombres dans une plage donnée.
 *
 * Ce fichier d'en-tête propose deux fonctions principales :
 *  - **initRandom()** : Initialise le générateur de nombres aléatoires en utilisant le temps actuel et le PID du processus.
 *  - **randomRange(int min, int max)** : Génère un nombre aléatoire compris entre min et max inclus.
 *
 * Les inclusions nécessaires dans ce fichier sont :
 *  - `<stdlib.h>` pour les fonctions rand() et srand().
 *  - `<time.h>` pour la gestion du temps.
 *  - `<unistd.h>` pour la fonction getpid().
 *  - `<sys/time.h>` pour la structure timeval et la fonction gettimeofday().
 *
 * @note Il est recommandé d'appeler **initRandom()** une seule fois au démarrage du programme afin d'initialiser correctement le générateur de nombres aléatoires.
 */

#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

/**
 * @brief Initialise le générateur de nombres aléatoires.
 *
 * Cette fonction récupère le temps actuel à l'aide de gettimeofday() et utilise ce temps ainsi que le PID du processus
 * pour initialiser le générateur de nombres aléatoires via srand(). Cela permet d'obtenir une séquence de nombres aléatoires
 * différente à chaque exécution du programme.
 *
 * @note Appeler cette fonction une seule fois au démarrage du programme.
 */
void initRandom() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * 1000000 + tv.tv_usec + getpid());
}

/**
 * @brief Génère un nombre aléatoire dans une plage donnée.
 *
 * Cette fonction utilise rand() pour retourner un nombre aléatoire compris entre min et max inclus.
 *
 * @param min La valeur minimale de la plage.
 * @param max La valeur maximale de la plage.
 * @return int Un nombre aléatoire entre min et max inclus.
 */
int randomRange(int min, int max) {
    return (rand() % (max + 1 - min) + min);
}

#endif