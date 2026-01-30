#include "Writer.h"
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

// Writer Function
void* writer(void* param)
{
   // extern readercount;
    pthread_detach(pthread_self()); 
    //int sockfd = (int)(intptr_t)param;
    ThreadParams *p = (ThreadParams*)param;
    //______________________________
    cJSON *dato_json;
    //_______________________________
     char query_buffer[5120];
    //__________________________
    char buffer[5120];
    char id_ard[5];
    char nome_client[50];
    char stato[8];
    char tipo_sensore[50];
    char descrizione[255];
    char unita_misura[50];
    //_________________________________________
    char note[1024];
    char *dato;
    char timestamp[50];

    //____________________________________________
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    //____________________________________________
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
    //____________________________________________


    printf("\nWriter is trying to enter");

    // Lock the semaphore
    sem_wait(p->y);
    printf("\nWriter has entered");

     //_______________________________
        recv(p->newSocket,&buffer, sizeof(buffer), 0);
    printf("%s",buffer);

    // parse the JSON data
    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json);
         exit(1);
    }

    
    //_______________________________
    dato_json = cJSON_GetObjectItemCaseSensitive(json, "id");
    strcpy(id_ard,dato_json->valuestring);
    dato_json = cJSON_GetObjectItemCaseSensitive(json, "nome");
    strcpy(nome_client,dato_json->valuestring);
    dato_json = cJSON_GetObjectItemCaseSensitive(json, "stato");
    strcpy(stato,dato_json->valuestring);
    dato_json = cJSON_GetObjectItemCaseSensitive(json, "tipo");
    strcpy(tipo_sensore,dato_json->valuestring);
    dato_json = cJSON_GetObjectItemCaseSensitive(json, "descrizione");
    strcpy(descrizione,dato_json->valuestring);
    dato_json = cJSON_GetObjectItemCaseSensitive(json, "misura");
    strcpy(unita_misura,dato_json->valuestring);
    //_________________________________________
   
    dato_json = cJSON_GetObjectItemCaseSensitive(json, "nota");
    strcpy(note,dato_json->valuestring);
    dato_json = cJSON_GetObjectItemCaseSensitive(json, "timestamp");
    strcpy(timestamp,dato_json->valuestring);
   
    dato_json = cJSON_GetObjectItemCaseSensitive(json, "dato");
    if (cJSON_IsArray(dato_json)) {
        // Converte ["1","2",...] in una stringa grezza "[\"1\",\"2\",...]"
        dato= cJSON_PrintUnformatted(dato_json);
    } else if (cJSON_IsString(dato_json)) {
        // Caso di backup se arrivasse come stringa normale
        // Dobbiamo duplicarla per poterla liberare con free() dopo allo stesso modo
        dato = strdup(dato_json->valuestring);
    } else {
        dato = strdup("[]"); // Default array vuoto
    }


    cJSON_Delete(json);
    //____________________________________________
   strcpy(query_buffer,"");
   
    sprintf(query_buffer, 
    "INSERT INTO arduino (id, nome_client, stato, tipo_sensore, descrizione, unita_misura) "
    "VALUES ('%s', '%s', '%s', '%s', '%s', '%s') " // <--- Assumo 'stato' sia stringa, ho aggiunto apici
    "ON DUPLICATE KEY UPDATE "
    "nome_client = VALUES(nome_client), "
    "stato = VALUES(stato), "
    "tipo_sensore = VALUES(tipo_sensore), "
    "descrizione = VALUES(descrizione), "
    "unita_misura = VALUES(unita_misura)", 
    id_ard, nome_client, stato, tipo_sensore, descrizione, unita_misura);

        if (mysql_query(conn, query_buffer)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
    }

    printf("Query 1 eseguita con successo.\n");
    strcpy(query_buffer,"");
    sprintf(query_buffer, "INSERT INTO misurazioni VALUES(NULL,'%s','%s','%s','%s')", id_ard, note, dato, timestamp);
    
    if (mysql_query(conn, query_buffer)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
    }

    printf("Query 2 eseguite con successo.\n");

    // Unlock the semaphore
    sem_post(p->y);

    printf("\nWriter is leaving");
    mysql_close(conn);
     mysql_thread_end();
    free(p);
    pthread_exit(NULL);
}