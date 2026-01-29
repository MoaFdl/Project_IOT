#include "Reader.h"
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
#include "thread_data.h"

// Reader Function
void* reader(void* param)
{
    pthread_detach(pthread_self()); 
    int sockfd = (int)(intptr_t)param;
    // Lock the semaphore
    sem_wait(&x);
    readercount++;

    if (readercount == 1)
        sem_wait(&y);

    // Unlock the semaphore
    sem_post(&x);

    
     char buffer_query[5120]={0};
   

   
    printf("\n%d reader is inside",readercount);
     if (mysql_query(conn, "SELECT * FROM arduino JOIN misurazioni ON arduino.id=misurazioni.id_arduino ;")) {
     
        fprintf(stderr, "%s\n", mysql_error(conn));
    }
    else
    {
       
    MYSQL_RES *result = mysql_store_result(conn);
    
    if (result == NULL) {
        fprintf(stderr, "Nessun risultato o errore: %s\n", mysql_error(conn));
        exit(1);
    }
    /**/

    //_________________________________________________________________
    // Otteniamo il numero di colonne (campi)
    int num_fields = mysql_num_fields(result);

    // 3. Ciclo per leggere le righe (FETCH)
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        // row è un array di stringhe. row[0] è la 1° colonna, row[1] la 2°...
        
        for(int i = 0; i < num_fields; i++) {
            // Controllo se il campo è NULL prima di stamparlo
            //printf("%s ", row[i] ? row[i] : "NULL");

            strcat(buffer_query,row[i] ? row[i] : "NULL");
            
            
        }
        strcat(buffer_query,"\n");
        //printf("\n"); // A capo alla fine di ogni riga del DB
    }
    printf("%s \n %d", buffer_query,strlen(buffer_query));
    send(sockfd,buffer_query, strlen(buffer_query), 0);
//_________________________________________________________________
  
    mysql_free_result(result);
    }

   
    sleep(5);

    // Lock the semaphore
    sem_wait(&x);
    readercount--;

    if (readercount == 0) {
        sem_post(&y);
    }

    // Lock the semaphore
    sem_post(&x);

    printf("\n%d Reader is leaving",readercount + 1);
    
    pthread_exit(NULL);

}