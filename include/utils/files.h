/**
 * @file files.h
 * @brief Définit des fonctions utilitaires pour la gestion des fichiers et du tampon d'entrée standard.
 *
 * Ce fichier d'en-tête fournit deux fonctions principales :
 *  - **emptyStdBuffer()** : Vide le tampon de l'entrée standard (clavier) en consommant tous les caractères restants.
 *  - **getFilePath(char *filename)** : Construit et retourne le chemin absolu d'un fichier en se basant sur le répertoire courant.
 *
 * Les fonctions utilisent les bibliothèques standards <stdio.h>, <string.h>, <unistd.h> et <stdlib.h>.
 *
 * @note La fonction getFilePath() alloue dynamiquement la mémoire pour le chemin complet. Il est impératif de libérer
 *       cette mémoire après utilisation afin d'éviter les fuites.
 */

#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * @brief Vide le tampon de l'entrée standard.
 *
 * Cette fonction lit et ignore tous les caractères restants dans l'entrée standard (clavier) jusqu'à rencontrer
 * un caractère de nouvelle ligne ('\n') ou la fin du fichier (EOF). Cela permet d'éviter les problèmes de débordement
 * lors de la lecture des entrées utilisateur.
 */
void emptyStdBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Construit et retourne le chemin absolu d'un fichier.
 *
 * Cette fonction récupère le répertoire courant à l'aide de getcwd(), puis concatène ce répertoire avec le nom du fichier
 * passé en paramètre pour construire le chemin absolu du fichier. La mémoire allouée pour le chemin complet est dynamique
 * et doit être libérée par l'appelant.
 *
 * @param filename Chaîne de caractères représentant le nom du fichier.
 * @return char* Pointeur vers une chaîne de caractères contenant le chemin absolu du fichier, ou NULL en cas d'erreur.
 *
 * @note En cas d'échec de getcwd() ou de malloc(), la fonction affiche un message d'erreur via perror() et retourne NULL.
 */
char *getFilePath(char *filename) {

    char *cwd = getcwd(NULL, 0);

    if (cwd == NULL) {
        perror("getcwd");
        return NULL;
    }

    // Calcule la taille nécessaire :
    //   longueur de cwd + 1 caractère pour '/' + longueur de filename + 1 caractère pour '\0'
    size_t len = strlen(cwd) + 1 + strlen(filename) + 1;
    char *fullPath = malloc(len);
    if (fullPath == NULL) {
        perror("malloc");
        free(cwd);
        return NULL;
    }

    // Construit la chaîne finale
    snprintf(fullPath, len, "%s/%s", cwd, filename);

    // Libère le buffer alloué par getcwd
    free(cwd);

    return fullPath;
}

#endif