/**
 * @file sockets.h
 * @brief Définit des fonctions et macros pour la gestion des sockets réseau.
 *
 * Ce fichier d'en-tête permet la création et la configuration des sockets pour établir une communication
 * réseau en utilisant le protocole TCP/IP. Il définit des macros pour le port, l'adresse IP, la famille de socket,
 * le type de socket ainsi que le nombre maximum de tentatives pour les opérations de lecture et d'écriture sur un socket.
 *
 * Les fonctions et structures fournies dans ce fichier sont :
 *  - **getSocket()** : Crée et retourne un socket.
 *  - **getServerAddress()** : Retourne la structure in_addr correspondant à l'adresse IP du serveur.
 *  - **getSocketAddress()** : Retourne une structure sockaddr_in configurée avec l'adresse IP et le port du serveur.
 *  - **Socket** : Structure encapsulant un socket et son adresse associée.
 *  - **trySocketRead()** : Tente de lire des données depuis un socket en effectuant plusieurs tentatives en cas d'erreur.
 *  - **trySocketWrite()** : Tente d'écrire des données sur un socket en effectuant plusieurs tentatives en cas d'erreur.
 *
 * @note Ce fichier utilise les bibliothèques <sys/socket.h>, <netinet/in.h>, <arpa/inet.h> et <unistd.h>.
 */
#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 9002
#define ADDRESS "127.0.0.1"
#define SOCKET_FAMILY AF_INET
#define SOCKET_TYPE SOCK_STREAM
#define MAX_ATTEMPTS 5

/**
 * @brief Structure représentant une socket.
 *
 * Cette structure encapsule un descripteur de socket et les informations
 * d'adresse de socket associées, utilisable par le client.
 */
typedef struct {
    int socket;
    struct sockaddr_in socketAddress;
} Socket;


/**
 * @brief Crée et retourne un socket.
 *
 * Cette fonction crée un socket en utilisant la famille et le type définis par les macros SOCKET_FAMILY et SOCKET_TYPE.
 *
 * @return int Le descripteur du socket créé.
 */
int getSocket() {
    return socket(SOCKET_FAMILY, SOCKET_TYPE, 0);
}

/**
 * @brief Retourne l'adresse du serveur sous forme de structure in_addr.
 *
 * Cette fonction convertit l'adresse IP définie par la macro ADDRESS en une structure in_addr.
 *
 * @return struct in_addr Structure contenant l'adresse IP du serveur.
 */
struct in_addr getServerAddress() {

    struct in_addr serverAdress;
    serverAdress.s_addr = inet_addr(ADDRESS);
    
    return serverAdress;
}

/**
 * @brief Retourne l'adresse de socket du serveur.
 *
 * Cette fonction initialise une structure sockaddr_in avec la famille de socket, le port (converti en format réseau)
 * et l'adresse IP du serveur obtenue via getServerAddress().
 *
 * @return struct sockaddr_in Structure contenant l'adresse de socket configurée.
 */
struct sockaddr_in getSocketAddress() {

    struct sockaddr_in socketAddress;
    socketAddress.sin_family = SOCKET_FAMILY;
    socketAddress.sin_port = htons(PORT);
    socketAddress.sin_addr = getServerAddress();

    return socketAddress;
}


/**
 * @brief Tente de lire des données depuis un socket.
 *
 * Cette fonction essaie de lire dataSize octets depuis le socket spécifié dans data. Elle effectue jusqu'à MAX_ATTEMPTS tentatives
 * en cas d'échec de la lecture.
 *
 * @param socket Le descripteur du socket depuis lequel lire.
 * @param data Pointeur vers le buffer dans lequel stocker les données lues.
 * @param dataSize Taille en octets des données à lire.
 * @return ssize_t Le nombre d'octets effectivement lus, ou -1 en cas d'erreur.
 */
ssize_t trySocketRead(int socket, void *data, size_t dataSize) {
    int attemptsLeft = MAX_ATTEMPTS;
    ssize_t bytesReceived;

    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
        // Gestion des différentes requêtes possible
        bytesReceived = read(socket, data, dataSize);

        if (bytesReceived != -1) {
            break;
        }
    }

    return bytesReceived;
}

/**
 * @brief Tente d'écrire des données sur un socket.
 *
 * Cette fonction essaie d'écrire dataSize octets sur le socket spécifié à partir de data. Elle effectue jusqu'à MAX_ATTEMPTS tentatives
 * en cas d'échec de l'écriture.
 *
 * @param socket Le descripteur du socket sur lequel écrire.
 * @param data Pointeur vers les données à envoyer.
 * @param dataSize Taille en octets des données à écrire.
 * @return ssize_t Le nombre d'octets effectivement écrits, ou -1 en cas d'erreur.
 */
ssize_t trySocketWrite(int socket, void *data, size_t dataSize) {
    int attemptsLeft = MAX_ATTEMPTS;
    ssize_t bytesSent;

    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
        // Gestion des différentes requêtes possible
        bytesSent = write(socket, data, dataSize);

        if (bytesSent != -1) {
            break;
        }
    }

    return bytesSent;
}

#endif