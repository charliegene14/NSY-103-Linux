/**
 * @file print_message.h
 * @brief Définit des fonctions pour l'affichage de messages colorés sur la console.
 *
 * Ce fichier d'en-tête fournit une énumération pour représenter différents types de messages (INFO, SUCCESS, WARNING, ERROR)
 * ainsi que des fonctions permettant d'obtenir une couleur associée à un type de message et d'afficher un message formaté avec
 * cette couleur. Les couleurs sont appliquées à l'aide des séquences d'échappement ANSI.
 *
 * Les inclusions nécessaires sont :
 *  - <stdio.h> pour la gestion de l'affichage.
 *  - <stdarg.h> pour la gestion des arguments variables dans la fonction printMessage().
 *
 * @note Les séquences d'échappement ANSI utilisées ici peuvent ne pas être supportées par tous les terminaux.
 */

#ifndef PRINT_MESSAGE_H
#define PRINT_MESSAGE_H

#include <stdio.h>
#include <stdarg.h>

/**
 * @brief Énumération des types de messages.
 *
 * Cette énumération représente les différents niveaux de gravité pour les messages affichés :
 *  - INFO : Messages d'information (affichés en bleu).
 *  - SUCCESS : Messages de succès (affichés en vert).
 *  - WARNING : Messages d'avertissement (affichés en jaune).
 *  - ERROR : Messages d'erreur (affichés en rouge).
 */
typedef enum {
    INFO,
    SUCCESS,
    WARNING,
    ERROR
} MessageType;


/**
 * @brief Retourne la séquence d'échappement ANSI correspondant à la couleur associée au type de message.
 *
 * @param type Le type du message (INFO, SUCCESS, WARNING, ERROR).
 * @return char* La séquence d'échappement ANSI pour la couleur correspondante.
 */
char* getColor(MessageType type) {
    switch (type) {
        case INFO: return "\x1B[34m"; // Blue
        case SUCCESS: return "\x1B[32m"; // Green
        case WARNING: return "\x1B[33m"; // Yellow
        case ERROR: return "\x1B[31m"; // Red
        default: return "\x1B[0m"; // Reset
    }
}

/**
 * @brief Affiche un message formaté avec une couleur spécifique sur la console.
 *
 * Cette fonction affiche un message sur la console en utilisant un format de type printf, précédé
 * de la séquence d'échappement de la couleur correspondant au type de message, et réinitialise la couleur
 * à la fin de l'affichage.
 *
 * @param type Le type de message à afficher.
 * @param format La chaîne de format (comme pour printf) décrivant le message à afficher.
 * @param ... Les arguments variables correspondant à la chaîne de format.
 */
void printMessage(MessageType type, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("%s", getColor(type));
    vprintf(format, args);
    printf("\x1B[0m"); // Reset
    va_end(args);
}

#endif