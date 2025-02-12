/**
 * @file maxmin_philosophers.h
 * @brief Définit les macros de configuration pour le nombre de philosophes et les temps d'état.
 *
 * Ce fichier d'en-tête définit plusieurs constantes utilisées pour paramétrer l'application
 * relative au problème des philosophes.
 *
 * Les macros définies sont :
 *  - **MIN_PHILOSOPHERS** : Le nombre minimum de philosophes requis pour démarrer le système (2).
 *  - **MAX_PHILOSOPHERS** : Le nombre maximum de philosophes autorisés dans le système. 
 *    Cette valeur est arbitraire mais doit être inférieure ou égale à la macro FOPEN_MAX.
 *  - **MIN_STATE_TIME** : Le temps minimum (en secondes) qu'un philosophe doit passer dans un état donné.
 *  - **MAX_STATE_TIME** : Le temps maximum (en secondes) qu'un philosophe peut passer dans un état donné.
 *
 * @note Ces valeurs permettent de contrôler le comportement de l'application et d'assurer
 *       que le système dispose d'un nombre adéquat de philosophes pour fonctionner correctement.
 */

#ifndef MAXMIN_PHILOSOPHERS_H
#define MAXMIN_PHILOSOPHERS_H

/**
 * @brief Nombre minimum de philosophes requis pour démarrer le système.
 *
 * 2 est le minimum requis pour que le système puisse démarrer.
 */
#define MIN_PHILOSOPHERS 2

/**
 * @brief Nombre maximum de philosophes autorisés.
 *
 * Valeur arbitraire mais doit être inférieure ou égale à la macro FOPEN_MAX.
 */
#define MAX_PHILOSOPHERS 7

/**
 * @brief Temps minimum dans un état en secondes.
 */
#define MIN_STATE_TIME 5

/**
 * @brief Temps maximum dans un état en secondes.
 */
#define MAX_STATE_TIME 10

#endif