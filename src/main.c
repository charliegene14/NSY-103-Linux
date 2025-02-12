/**
 * @file main.c
 * @brief Point d'entrée de l'application client pour le problème des philosophes.
 *
 * Ce fichier implémente la logique principale de l'application client. Il gère la connexion au serveur,
 * la création des philosophes côté client, la communication avec le serveur via des sockets, ainsi que
 * la gestion des commandes de l'utilisateur permettant d'ajouter des philosophes ou de quitter l'application.
 *
 * Les inclusions dans ce fichier fournissent les définitions nécessaires pour :
 *  - Les macros du nombre min et max de philosophes (maxmin_philosophers.h).
 *  - L'affichage de messages sur la console (print_message.h).
 *  - La gestion des fichiers, sockets et commandes (files.h, sockets.h, commands.h).
 *  - La génération de nombres aléatoires (random.h).
 *  - La définition de la structure `ClientPhilosopher` qui représente un philosophe côté client.
 *  - L'implémentation des fonctions de requêtes et de réponses (Request.c et Response.c).
 *  - Les bibliothèques standard pour les opérations d'entrée/sortie, la gestion des threads et des erreurs.
 *
 * @note Ce fichier utilise une boucle infinie pour permettre à l'utilisateur d'ajouter dynamiquement
 * des philosophes et de gérer leur cycle de vie via des threads.
 */

#include "../include/maxmin_philosophers.h"
#include "../include/utils/print_message.h"
#include "../include/utils/files.h"
#include "../include/utils/sockets.h"
#include "../include/utils/commands.h"
#include "../include/utils/random.h"
#include "../include/entities/ClientPhilosopher.h"
#include "../include/managers/Request.c"
#include "../include/managers/Response.c"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

/**
 * @brief Quitte le programme en affichant un message d'information.
 *
 * Cette fonction affiche un message indiquant la fermeture du programme et des clients, puis termine l'exécution
 * de l'application en appelant exit().
 * 
 * En cas de coupure, les sockets se fermeront et le serveur sera immédiatement notifié.
 */
void quit() {
    printMessage(INFO, "Fermeture du programme et des clients...");
    exit(EXIT_SUCCESS);
}

/**
 * @brief Routine du thread associé à chaque philosophe.
 *
 * Cette fonction représente le comportement d'un philosophe côté client. Elle exécute en boucle les étapes suivantes :
 *  - Réduit le timer associé à l'état courant du philosophe.
 *  - Lorsque le timer atteint zéro, le philosophe change d'état :
 *      - S'il était en train de penser, il passe à l'état HUNGRY.
 *      - S'il était en train de manger, il passe à l'état THINKING et un nouveau timer est assigné.
 *      - En cas d'état inconnu, un message d'erreur est affiché et le programme se termine.
 *  - Envoie une requête de mise à jour de l'état du philosophe au serveur via un socket.
 *  - Si le philosophe est en état HUNGRY, il attend la réponse du serveur qui lui accorde l'autorisation de manger.
 *  - Une fois la réponse reçue, il met à jour son état et réinitialise le timer.
 *
 * @param arg Pointeur vers la structure `ClientPhilosopher` représentant le philosophe concerné.
 * @return void* Toujours retourne NULL à la fin de l'exécution du thread.
 */
void *philosopherThread(void *arg) {
    ClientPhilosopher *philosopher = (ClientPhilosopher *) arg;

    while (1) {

        sleep(1);
        philosopher->base.stateTimer -= 1;
        
        // Timer à 0, changement d'état
        if (philosopher->base.stateTimer <= 0) {
            switch(philosopher->base.state) {

                case THINKING:
                    philosopher->base.state = HUNGRY;
                    break;

                case EATING:
                    philosopher->base.state = THINKING;
                    philosopher->base.stateTimer = randomRange(MIN_STATE_TIME, MAX_STATE_TIME);

                    break;

                default:
                    printMessage(ERROR, "L'état du philosophe n'est pas pris en charge.\n");
                    exit(EXIT_FAILURE);
            }
            
        }

        Request request = updateRequest(philosopher->base);

        if (trySocketWrite(philosopher->clientSocket.socket, &request, sizeof(request)) == -1) {
            printMessage(ERROR, "Erreur lors d'une tentative d'envoi d'un requête de mise à jour.\n");
            perror("write");
            exit(EXIT_FAILURE);
        }

        // Quand le philosophe souhaite manger, attente du serveur pour avoir l'autorisation et récupérer le philosophe avec l'état EATING
        if (philosopher->base.state == HUNGRY) {

            Response response;

            if (trySocketRead(philosopher->clientSocket.socket, &response, sizeof(response)) == -1) {
                printMessage(ERROR, "Erreur lors d'une tentative d'envoi d'un requête de mise à jour.\n");
                perror("write");
                close(philosopher->clientSocket.socket);
                exit(EXIT_FAILURE);
            }

            if (response.type != RESPONSE_UPDATE) {
                printMessage(ERROR, "Le type de réponse attendu n'est pas correct.\n");
                close(philosopher->clientSocket.socket);
                break;
            }

            philosopher->base.state = response.philosopher.state;
            philosopher->base.stateTimer = randomRange(MIN_STATE_TIME, MAX_STATE_TIME);
        }

    }

    return NULL;
}

/**
 * @brief Ajoute des philosophes côté client et crée leur thread associé.
 *
 * Cette fonction permet d'ajouter un nombre spécifié de philosophes. Pour chaque philosophe :
 *  - Un nouveau socket client est créé et une connexion au serveur est établie.
 *  - Une requête de création de philosophe est envoyée au serveur.
 *  - Le client reçoit en réponse les informations initiales du philosophe (identifiant, état, timer).
 *  - Le philosophe est ajouté à la liste locale des philosophes et son état est initialisé.
 *  - Un thread est créé pour gérer le cycle de vie du philosophe via la fonction `philosopherThread`.
 *
 * @param number Nombre de philosophes à ajouter.
 * @param currentNumber Pointeur vers le compteur actuel de philosophes.
 * @param philosophers Tableau de structures `ClientPhilosopher` où les nouveaux philosophes seront ajoutés.
 */
void addPhilosophers(int number, int *currentNumber, ClientPhilosopher *philosophers) {
    printMessage(INFO, "\nAjout de %d philosophe%s.. \n", number, (number > 1 ? "s" : ""));

    for (int i = 0; i < number; i++) {

        // On créer un philosophe
        ClientPhilosopher newPhilosopher;

        // Création d'un socket client par philosophe
        newPhilosopher.clientSocket.socket = getSocket();
        newPhilosopher.clientSocket.socketAddress = getSocketAddress();

        // Tentative de connexion
        if (connect(newPhilosopher.clientSocket.socket, (struct sockaddr *) &newPhilosopher.clientSocket.socketAddress, sizeof(newPhilosopher.clientSocket.socketAddress)) == -1) {
            printMessage(ERROR, "Une erreur est survenue lors d'une tentative de connexion au serveur.\n");
            perror("connect");
            break;
        }

        // On commence la liaison en attribuant un id par le serveur
        // Avec l'envoi d'une requête pour créer un philosophe, utilisation de write() plutôt que send() car pas besoin de flag

        Request request = createRequest();
       
        if (trySocketWrite(newPhilosopher.clientSocket.socket, &request, sizeof(request)) == -1) {
            printMessage(ERROR, "Une erreur est survenue lors d'une requête d'ajout de philosophe.\n");
            perror("write");
            close(newPhilosopher.clientSocket.socket);
            break;
        }

        // Récéption du philosophe (id, état, timer)
        Response response;

        if (trySocketRead(newPhilosopher.clientSocket.socket, &response, sizeof(response)) == -1) {
            printMessage(ERROR, "Une erreur est survenue lors de la réception d'une réponse d'ajout de philosophe.\n");
            perror("write");
            close(newPhilosopher.clientSocket.socket);
            break;
        }
     
        if (response.type != RESPONSE_CREATE) {
            printMessage(ERROR, "Le type de réponse attendu n'est pas correct.\n");
            close(newPhilosopher.clientSocket.socket);
            break;
        }

        printMessage(INFO, "Philosophe reçu par le serveur : %d \n", response.philosopher.id);

        newPhilosopher.base = response.philosopher;
        newPhilosopher.base.state = THINKING;
        newPhilosopher.base.stateTimer = randomRange(MIN_STATE_TIME, MAX_STATE_TIME);
        
        // On l'ajoute à la liste des philosophes en mémoire.
        philosophers[*currentNumber] = newPhilosopher;

        // On créé son thread associé
        if (pthread_create(&philosophers[*currentNumber].thread, NULL, philosopherThread, &philosophers[*currentNumber]) == 0) {
            printMessage(
                SUCCESS, 
                "Le philosphe %d a été créé et connecté avec succès. \n",
                *currentNumber + 1
            );

            *currentNumber += 1;
        }        
    }
}

/**
 * @brief Fonction principale du client.
 *
 * La fonction main initialise les variables nécessaires et entre dans une boucle infinie permettant
 * à l'utilisateur d'ajouter dynamiquement des philosophes via des commandes saisies au clavier.
 * Pour chaque ajout, la fonction vérifie la validité de la commande, détermine le nombre de philosophes à ajouter,
 * et appelle la fonction `addPhilosophers` pour créer et connecter les nouveaux philosophes.
 *
 * @param argc Nombre d'arguments passés en ligne de commande.
 * @param argv Tableau des arguments passés en ligne de commande.
 * @return int Retourne 0 lors de la terminaison normale du programme.
 */
int main(int argc, char *argv[]) {

    ClientPhilosopher philosophers[MAX_PHILOSOPHERS];
    int numberOfPhilosophers = 0;

    // Boucle pour ajouter autant de philosophes que souhaité
    while (1) {

        // Tant qu'il n'y a pas de philosophe, on doit ajouter le nombre minimum requis, sinon on peut les ajouter un par un
        int minPhilosophers = numberOfPhilosophers >= 2 ? 1 : MIN_PHILOSOPHERS;

        printMessage(
            INFO, 
            "\nNombre de philosophes autour de la table : %d / %d (places restantes : %d)\n", 
            numberOfPhilosophers,
            MAX_PHILOSOPHERS,
            MAX_PHILOSOPHERS - numberOfPhilosophers
        );

        printf(
            "Saisir '%s nombre' pour ajouter un nombre de philosophes (%d à %d) ou '%s' pour arrêter le programme. \n", 
            ADD_COMMAND, 
            minPhilosophers,
            MAX_PHILOSOPHERS - numberOfPhilosophers,
            QUIT_COMMAND
        );

        char command[COMMAND_SIZE];
        bool isCommandValid = false;

        do {

            // On vide la variable command pour éviter des erreurs de lecture
            strcpy(command, "");
            int addedPhilosophers = 0;

            // On utilise fgets pour protéger d'un buffer overflow
            if (!fgets(command, COMMAND_SIZE, stdin)) {
                printMessage(ERROR, "Erreur à la lecture de la commande.");
                perror("fgets");
                exit(EXIT_FAILURE);
            }

            // Supression du caractère de retour à la ligne
            if (command[strlen(command) - 1] == '\n') {
                command[strlen(command) - 1] = '\0';
            } else {
                // Vider le buffer d'entrée si le retour à la ligne n'a pas été lu lors d'une commande trop longue
                emptyStdBuffer();
            }

            if (isQuitCommand(command)) {
                quit();
            }

            if (isAddCommand(command)) {
                int numberToAdd = getAddCommandNumber(command);
                if (numberToAdd >= minPhilosophers && numberToAdd <= MAX_PHILOSOPHERS - numberOfPhilosophers) {
                    isCommandValid = true;
                    addPhilosophers(numberToAdd, &numberOfPhilosophers, philosophers);
                }
            }


            if (!isCommandValid) {
                printMessage(WARNING, "La commande %s n'est pas valide. \n", command);
            }

        } while (!isCommandValid);

    }

    return 0;
}