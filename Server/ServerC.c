// C program for the Server Side
//librerie_______________________________________________________________
//socket
#include <arpa/inet.h>
#include <sys/socket.h>
// For threading
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//_______________________________________________________________________

//external files
#include "Writer.h"
#include "Reader.h"
#include "thread_data.h"


//variabili globali_______________________________________________________________
sem_t x, y; // Semaphore variables
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;

int main()
{
    
    int serverSocket; 
    int newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;

    socklen_t addr_size;
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8989);

    // Bind the socket to the address and port number.
    bind(serverSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr));

    // Listen on the socket,with 40 max connection requests queued
    if (listen(serverSocket, 50) == 0)
    {
        printf("Listening\n");
    }   
    else
    {
        printf("Error\n");
    }   

    // Array for thread
    pthread_t tid[60];

    int i = 0;

    while (1) {
        addr_size = sizeof(serverStorage);

        // Extract the first connection in the queue
        newSocket = accept(serverSocket,(struct sockaddr*)&serverStorage, &addr_size);
        int choice = 0;
        recv(newSocket,&choice, sizeof(choice), 0);
        printf("%d",choice);

        ThreadParams *T_params = malloc(sizeof(ThreadParams));
        T_params->x = &x;
        T_params->y = &y;
        T_params->newSocket = newSocket;
        
        if (choice == 1) {
           // Crea reader
            if (pthread_create(&readerthreads[i++], NULL,reader, (void*)T_params)!= 0)
            {
                printf("Failed to create reader\n");
            }
                
        }
        else if (choice == 2) {
            // Crea writer
            if (pthread_create(&writerthreads[i++], NULL,writer, (void*)T_params)!= 0)
            {
                printf("Failed to create writer\n");
            }
        }
       
        if (i >= 50) {

            i = 0;

            while (i < 50) 
            {
                // Suspend execution of the calling thread until the target thread terminates
                pthread_join(writerthreads[i++],NULL);
                pthread_join(readerthreads[i++],NULL);
            }

            i = 0;
        }
    }
    return 0;
}