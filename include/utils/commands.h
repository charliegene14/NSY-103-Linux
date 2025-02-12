/**
 * @file commands.h
 * @brief Définit les macros et fonctions pour le traitement des commandes utilisateur.
 *
 * Ce fichier d'en-tête fournit les constantes et les fonctions nécessaires pour analyser et valider les commandes
 * saisies par l'utilisateur. Il permet notamment de distinguer les commandes d'ajout de philosophes et de fermeture
 * du programme.
 *
 * Les macros définies sont :
 *  - **ADD_COMMAND** : La chaîne de caractères utilisée pour ajouter des philosophes ("/add").
 *  - **QUIT_COMMAND** : La chaîne de caractères utilisée pour quitter le programme ("/quit").
 *  - **COMMAND_SIZE** : La taille maximale d'une commande, calculée comme la taille de ADD_COMMAND, plus un espace,
 *    plus la taille d'un entier (pour représenter le nombre de philosophes).
 *
 * Les fonctions fournies par ce fichier sont :
 *  - **isAddCommand** : Vérifie si une commande correspond à la commande d'ajout.
 *  - **isQuitCommand** : Vérifie si une commande correspond à la commande de fermeture.
 *  - **getAddCommandNumber** : Extrait et retourne le nombre de philosophes à ajouter à partir d'une commande d'ajout.
 *
 * L'inclusion des bibliothèques <string.h>, <stdlib.h>, <ctype.h> et <stdbool.h> permet d'utiliser des fonctions
 * de manipulation de chaînes, de conversion et la gestion des booléens.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define ADD_COMMAND "/add"
#define QUIT_COMMAND "/quit"

// Taille de la plus grande commande possible + taille d'un espace + taille d'un entier pour le nombre de philosophes
#define COMMAND_SIZE sizeof(ADD_COMMAND) + sizeof(" ") + sizeof(int) 

/**
 * @brief Vérifie si une commande correspond à la commande d'ajout.
 *
 * Cette fonction extrait le début de la chaîne passée en paramètre et le compare à ADD_COMMAND ("/add").
 * Elle retourne true si la commande commence par "/add", sinon false.
 *
 * @param command Chaîne de caractères représentant la commande saisie par l'utilisateur.
 * @return bool true si la commande est une commande d'ajout, false sinon.
 */
bool isAddCommand(char *command) {
    char addCommand[sizeof(ADD_COMMAND)];
    strncpy(addCommand, command, strlen(ADD_COMMAND));
    addCommand[strlen(ADD_COMMAND)] = '\0';  // Assurez-vous de la terminaison

    if (strcmp(addCommand, ADD_COMMAND) == 0) {
        return true;
    }

    return false;
}

/**
 * @brief Vérifie si une commande correspond à la commande de fermeture.
 *
 * Cette fonction compare la chaîne passée en paramètre avec QUIT_COMMAND ("/quit").
 * Elle retourne true si la commande correspond exactement à "/quit", sinon false.
 *
 * @param command Chaîne de caractères représentant la commande saisie par l'utilisateur.
 * @return bool true si la commande est la commande de fermeture, false sinon.
 */
bool isQuitCommand(char *command) {
     if (strcmp(command, QUIT_COMMAND) == 0) {
        return true;
    }

    return false;
}

/**
 * @brief Extrait le nombre de philosophes à ajouter à partir d'une commande d'ajout.
 *
 * Cette fonction récupère la partie numérique de la commande en se positionnant après la commande ADD_COMMAND et l'espace.
 * Elle vérifie ensuite que la chaîne extraite ne contient que des chiffres. Si c'est le cas, la fonction convertit
 * la chaîne en entier et la retourne. Sinon, elle retourne -1 pour indiquer une erreur de format.
 *
 * @param command Chaîne de caractères représentant la commande d'ajout.
 * @return int Le nombre de philosophes à ajouter, ou -1 en cas d'erreur de format.
 */
int getAddCommandNumber(char *command) {
    // On récupère le nombre de philosophes en se plaçant après la commande add et l'espace dans l'adresse mémoire de command
    char *numberOfPhilosophers = command + strlen(ADD_COMMAND) + 1; // On ajoute 1 pour l'espace

    // On vérifie si le nombre de philosophes est bien un entier
    for (int i = 0; i < strlen(numberOfPhilosophers); i++) {
        if (!isdigit(numberOfPhilosophers[i])) {
            return -1;
        }
    }

    return atoi(numberOfPhilosophers);
}

#endif