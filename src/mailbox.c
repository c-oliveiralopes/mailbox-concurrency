#include "mailbox.h"

// Variáveis globais
mailbox_t main_mailbox;
receiver_data_t receivers[NUM_RECEIVERS];
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

// Inicializa o mailbox
void init_mailbox(mailbox_t* mailbox) {
    mailbox->count = 0;
    mailbox->in = 0;
    mailbox->out = 0;
    
    pthread_mutex_init(&mailbox->mutex, NULL);
    pthread_cond_init(&mailbox->not_full, NULL);
    pthread_cond_init(&mailbox->not_empty, NULL);
    
    // Inicializa buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        mailbox->buffer[i].data = 0;
        mailbox->buffer[i].sender_id = -1;
        mailbox->buffer[i].is_valid = 0;
    }
}

// Destroi o mailbox
void destroy_mailbox(mailbox_t* mailbox) {
    pthread_mutex_destroy(&mailbox->mutex);
    pthread_cond_destroy(&mailbox->not_full);
    pthread_cond_destroy(&mailbox->not_empty);
}

// Funções de manipulação de bits conforme especificado
int converte_valor(unsigned char bytex2, unsigned char bytex1) {
    return bytex2 * 256 + bytex1;
}

void encontra_bytes(int valor, unsigned char* byte2, unsigned char* byte1) {
    *byte2 = ((valor >> 8) & 255);
    *byte1 = (valor & 255);
}

// Envia mensagem para o mailbox (síncrono)
void send_message(mailbox_t* mailbox, message_t msg) {
    pthread_mutex_lock(&mailbox->mutex);
    
    // Aguarda até haver espaço no buffer
    while (mailbox->count == BUFFER_SIZE) {
        pthread_cond_wait(&mailbox->not_full, &mailbox->mutex);
    }
    
    // Adiciona mensagem ao buffer
    mailbox->buffer[mailbox->in] = msg;
    mailbox->in = (mailbox->in + 1) % BUFFER_SIZE;
    mailbox->count++;
    
    pthread_mutex_lock(&print_mutex);
    printf("Mensagem enviada para mailbox - Remetente: ");
    print_thread_name(msg.sender_id);
    printf(", Dados: 0x%04X\n", msg.data);
    pthread_mutex_unlock(&print_mutex);
    
    // Sinaliza que há mensagem disponível
    pthread_cond_signal(&mailbox->not_empty);
    pthread_mutex_unlock(&mailbox->mutex);
}

// Recebe mensagem do mailbox (síncrono)
message_t receive_message(mailbox_t* mailbox) {
    pthread_mutex_lock(&mailbox->mutex);
    
    // Aguarda até haver mensagem no buffer
    while (mailbox->count == 0) {
        pthread_cond_wait(&mailbox->not_empty, &mailbox->mutex);
    }
    
    // Remove mensagem do buffer
    message_t msg = mailbox->buffer[mailbox->out];
    mailbox->buffer[mailbox->out].is_valid = 0;
    mailbox->out = (mailbox->out + 1) % BUFFER_SIZE;
    mailbox->count--;
    
    // Sinaliza que há espaço disponível
    pthread_cond_signal(&mailbox->not_full);
    pthread_mutex_unlock(&mailbox->mutex);
    
    return msg;
}

// Função para imprimir nome da thread
void print_thread_name(int thread_id) {
    switch(thread_id) {
        case THREAD_A: printf("Thread A"); break;
        case THREAD_B: printf("Thread B"); break;
        case THREAD_C: printf("Thread C"); break;
        case THREAD_D: printf("Thread D"); break;
        case THREAD_E: printf("Thread E"); break;
        case THREAD_F: printf("Thread F"); break;
        case THREAD_G: printf("Thread G"); break;
        case THREAD_H: printf("Thread H"); break;
        default: printf("Thread %d", thread_id); break;
    }
}

// Thread remetente
void* sender_thread(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    
    pthread_mutex_lock(&print_mutex);
    printf("Iniciando ");
    print_thread_name(data->thread_id);
    printf(" - enviará valor %d para ", data->message_value);
    print_thread_name(data->destination_id);
    printf("\n");
    pthread_mutex_unlock(&print_mutex);
    
    // Simula processamento
    sleep(1);
    
    // Cria mensagem usando mascaramento de bits
    message_t msg;
    msg.data = converte_valor((unsigned char)data->destination_id, (unsigned char)data->message_value);
    msg.sender_id = data->thread_id;
    msg.is_valid = 1;
    
    // Envia mensagem para o mailbox
    send_message(data->mailbox, msg);
    
    pthread_mutex_lock(&print_mutex);
    print_thread_name(data->thread_id);
    printf(" finalizou envio\n");
    pthread_mutex_unlock(&print_mutex);
    
    return NULL;
}

// Thread mailbox - gerencia distribuição das mensagens
void* mailbox_thread(void* arg) {
    mailbox_t* mailbox = (mailbox_t*)arg;
    int messages_processed = 0;
    
    pthread_mutex_lock(&print_mutex);
    printf("Thread Mailbox iniciada - aguardando mensagens...\n");
    pthread_mutex_unlock(&print_mutex);
    
    while (messages_processed < NUM_SENDERS) {
        // Recebe mensagem do buffer
        message_t msg = receive_message(mailbox);
        
        // Extrai informações da mensagem usando mascaramento de bits
        unsigned char destination_id, message_value;
        encontra_bytes(msg.data, &destination_id, &message_value);
        
        pthread_mutex_lock(&print_mutex);
        printf("Mailbox processou mensagem - Destino: ");
        print_thread_name((int)destination_id);
        printf(", Valor: %d\n", (int)message_value);
        pthread_mutex_unlock(&print_mutex);
        
        // Entrega mensagem para o destinatário correto
        int receiver_index = destination_id - THREAD_E;
        if (receiver_index >= 0 && receiver_index < NUM_RECEIVERS) {
            receivers[receiver_index].received_messages[receivers[receiver_index].message_count] = msg;
            receivers[receiver_index].message_count++;
        }
        
        messages_processed++;
    }
    
    pthread_mutex_lock(&print_mutex);
    printf("Thread Mailbox finalizou processamento de %d mensagens\n", messages_processed);
    pthread_mutex_unlock(&print_mutex);
    
    return NULL;
}

// Thread destinatária
void* receiver_thread(void* arg) {
    receiver_data_t* data = (receiver_data_t*)arg;
    
    pthread_mutex_lock(&print_mutex);
    printf("Iniciando ");
    print_thread_name(data->thread_id);
    printf(" - aguardando mensagens...\n");
    pthread_mutex_unlock(&print_mutex);
    
    // Aguarda mensagens serem processadas pelo mailbox
    sleep(3);
    
    // Exibe mensagens recebidas
    for (int i = 0; i < data->message_count; i++) {
        unsigned char sender_id, message_value;
        encontra_bytes(data->received_messages[i].data, &sender_id, &message_value);
        
        pthread_mutex_lock(&print_mutex);
        print_thread_name(data->thread_id);
        printf(" recebeu mensagem: %d da ", (int)message_value);
        print_thread_name(data->received_messages[i].sender_id);
        printf("\n");
        pthread_mutex_unlock(&print_mutex);
    }
    
    return NULL;
}

int main() {
    pthread_t sender_threads[NUM_SENDERS];
    pthread_t mailbox_thread_id;
    pthread_t receiver_threads[NUM_RECEIVERS];
    
    thread_data_t sender_data[NUM_SENDERS];
    
    printf("=== Sistema de Mailbox Concorrente ===\n\n");
    
    // Inicializa mailbox
    init_mailbox(&main_mailbox);
    
    // Inicializa dados dos receivers
    for (int i = 0; i < NUM_RECEIVERS; i++) {
        receivers[i].thread_id = THREAD_E + i;
        receivers[i].mailbox = &main_mailbox;
        receivers[i].message_count = 0;
    }
    
    // Configura dados dos senders
    sender_data[0] = (thread_data_t){THREAD_A, 10, THREAD_E, &main_mailbox};
    sender_data[1] = (thread_data_t){THREAD_B, 20, THREAD_F, &main_mailbox};
    sender_data[2] = (thread_data_t){THREAD_C, 30, THREAD_G, &main_mailbox};
    sender_data[3] = (thread_data_t){THREAD_D, 40, THREAD_H, &main_mailbox};
    
    // Cria thread mailbox
    pthread_create(&mailbox_thread_id, NULL, mailbox_thread, &main_mailbox);
    
    // Cria threads remetentes
    for (int i = 0; i < NUM_SENDERS; i++) {
        pthread_create(&sender_threads[i], NULL, sender_thread, &sender_data[i]);
    }
    
    // Cria threads destinatárias
    for (int i = 0; i < NUM_RECEIVERS; i++) {
        pthread_create(&receiver_threads[i], NULL, receiver_thread, &receivers[i]);
    }
    
    // Aguarda finalização das threads remetentes
    for (int i = 0; i < NUM_SENDERS; i++) {
        pthread_join(sender_threads[i], NULL);
    }
    
    // Aguarda finalização da thread mailbox
    pthread_join(mailbox_thread_id, NULL);
    
    // Aguarda finalização das threads destinatárias
    for (int i = 0; i < NUM_RECEIVERS; i++) {
        pthread_join(receiver_threads[i], NULL);
    }
    
    // Cleanup
    destroy_mailbox(&main_mailbox);
    pthread_mutex_destroy(&print_mutex);
    
    printf("\n=== Todas as threads finalizaram com sucesso ===\n");
    
    return 0;
}