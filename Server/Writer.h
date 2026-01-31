#ifndef WRITER_H
#define WRITER_H

//librerie_______________________________________________________________
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <mysql.h>
#include <cjson/cJSON.h>
//_______________________________________________________________________

extern int readercount;
void* writer(void* param);

#endif