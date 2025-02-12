/**
 * @file ClientPhilosopher.h
 * @brief Définit la structure représentant un philosophe côté client.
 *
 * Ce fichier d'en-tête définit la structure de données `ClientPhilosopher` qui permet de représenter
 * un philosophe dans une application client. Cette structure intègre la structure de base `Philosopher`,
 * ainsi que le thread associé à l'exécution du philosophe et le socket client pour la communication réseau.
 *
 * La structure `ClientPhilosopher` comporte :
 *  - `base` : de type `Philosopher`, représentant les attributs et comportements de base d'un philosophe.
 *  - `thread` : de type `pthread_t`, représentant le thread d'exécution associé au philosophe.
 *  - `clientSocket` : de type `Socket`, utilisé pour gérer la communication entre le philosophe et le serveur.
 *
 * Les inclusions nécessaires pour ce fichier d'en-tête sont :
 *  - "Philosopher.h" pour la définition de la structure `Philosopher`.
 *  - "../utils/sockets.h" pour la gestion des sockets.
 *  - <pthread.h> pour la gestion des threads POSIX.
 *  - <stdio.h> pour les fonctions d'entrée/sortie.
 *
 * Les directives de préprocesseur (#ifndef, #define, #endif) garantissent que ce fichier d'en-tête
 * est inclus une seule fois lors de la compilation.
 *
 */

#ifndef CLIENTPHILOSOPHER_H
#define CLIENTPHILOSOPHER_H

#include "Philosopher.h"
#include "../utils/sockets.h"
#include <pthread.h>
#include <stdio.h>

/**
 * @brief Structure représentant un philosophe côté client.
 *
 * Cette structure intègre la structure de base `Philosopher` et ajoute :
 *  - le thread d'exécution du philosophe
 *  - un socket client pour la communication réseau avec le serveur.
 */
typedef struct {

    Philosopher base; 

    /**
     * Thread associé au philosophe
     */
    pthread_t thread;

    /**
     * Socket client associé au philosophe
     */
    Socket clientSocket;

} ClientPhilosopher;

#endif