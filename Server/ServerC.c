// C program for the Server Side

// inet_addr
#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <mysql.h>
#include <cjson/cJSON.h>
#include "Writer.h"
#include "Reader.h"


//variabili globali_______________________________________________________________
// Semaphore variables
sem_t x, y;
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
static char buffer[5120];
//static char buffer_query[5120];

//__________________________________________________________________________________________



// Driver Code
int main()
{
    // Initialize variables
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;

    socklen_t addr_size;
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8989);

    

    char *server = "host.docker.internal";
    char *user = "root";      // Utente default di XAMPP
    char *password = "";      // Password default di XAMPP (vuota)
    char *database = "sensori";  // Database di prova spesso presente in XAMPP

 // 1. Inizializzazione
    conn = mysql_init(NULL);

    // 2. Connessione
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    printf("Connessione al database riuscita!\n");

    // Bind the socket to the
    // address and port number.
    bind(serverSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr));

    // Listen on the socket,
    // with 40 max connection
    // requests queued
    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        printf("Error\n");

    // Array for thread
    pthread_t tid[60];

    int i = 0;

    while (1) {
        addr_size = sizeof(serverStorage);

        // Extract the first
        // connection in the queue
        newSocket = accept(serverSocket,(struct sockaddr*)&serverStorage, &addr_size);
        //
        int choice = 0;
        recv(newSocket,&choice, sizeof(choice), 0);
        printf("%d",choice);
        

        int *pclient = malloc(sizeof(int));
        *pclient = newSocket;
        if (choice == 1) {
           
             
            if (pthread_create(&readerthreads[i++], NULL,reader, (void*)(intptr_t)newSocket)!= 0)

                // Error in creating thread
                printf("Failed to create thread\n");
                free(pclient);
        }
        else if (choice == 2) {
            
            // Create writers thread
            recv(newSocket,&buffer, sizeof(buffer), 0);
             printf("%s",buffer);
            if (pthread_create(&writerthreads[i++], NULL,writer, &newSocket)!= 0)

                // Error in creating thread
                printf("Failed to create thread\n");
                
        }
       
        if (i >= 50) {
            // Update i
            i = 0;

            while (i < 50) {
                // Suspend execution of
                // the calling thread
                // until the target
                // thread terminates
                pthread_join(writerthreads[i++],NULL);
                pthread_join(readerthreads[i++],NULL);
            }

            // Update i
            i = 0;
        }
    }
    mysql_close(conn);
    return 0;
}