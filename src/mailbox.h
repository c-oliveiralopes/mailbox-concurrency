#ifndef MAILBOX_H
#define MAILBOX_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// Definições de constantes
#define BUFFER_SIZE 12
#define NUM_SENDERS 4
#define NUM_RECEIVERS 4
#define MAX_MESSAGES 4

// Identificadores das threads
typedef enum {
    THREAD_A = 0,
    THREAD_B = 1,
    THREAD_C = 2,
    THREAD_D = 3,
    THREAD_E = 4,
    THREAD_F = 5,
    THREAD_G = 6,
    THREAD_H = 7
} thread_id_t;

// Estrutura da mensagem
typedef struct {
    int data;           // Dados combinados (ID_destino + mensagem)
    int sender_id;      // ID do remetente para logging
    int is_valid;       // Flag indicando se a mensagem é válida
} message_t;

// Estrutura do mailbox
typedef struct {
    message_t buffer[BUFFER_SIZE];
    int count;
    int in;
    int out;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} mailbox_t;

// Estrutura para passar dados para as threads
typedef struct {
    int thread_id;
    int message_value;
    int destination_id;
    mailbox_t* mailbox;
} thread_data_t;

// Estrutura para threads destinatárias
typedef struct {
    int thread_id;
    mailbox_t* mailbox;
    message_t received_messages[MAX_MESSAGES];
    int message_count;
} receiver_data_t;

// Declaração das funções
void init_mailbox(mailbox_t* mailbox);
void destroy_mailbox(mailbox_t* mailbox);

// Funções de manipulação de bits (conforme especificado)
int converte_valor(unsigned char bytex2, unsigned char bytex1);
void encontra_bytes(int valor, unsigned char* byte2, unsigned char* byte1);

// Funções das threads
void* sender_thread(void* arg);
void* mailbox_thread(void* arg);
void* receiver_thread(void* arg);

// Funções auxiliares
void send_message(mailbox_t* mailbox, message_t msg);
message_t receive_message(mailbox_t* mailbox);
void print_thread_name(int thread_id);

#endif // MAILBOX_H