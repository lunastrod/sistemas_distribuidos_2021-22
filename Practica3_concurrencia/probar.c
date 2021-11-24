void *lectores_prio_lector(void *arg) {

    struct response response;

    sem_wait(&control);
        num_clientes_lectores++;
        numero_lectores++;
        ratio_counter++;

        if(numero_lectores == 1) {
            sem_wait(&sem_prio_lect);
        }
        if ((ratio_exist && ratio_counter % *(int*) arg == 0) && num_clientes_escritores != 0) {
            block_ratio = 1;
            sem_post(&sem_mutex);
            sem_wait(&sem_ratio);
        }
    sem_post(&control);
    //INICIO SECCIÓN CRÍTICA
    //FIN SECCIÓN CRÍTICA
    sem_wait(&control);
        numero_lectores--;
        if(numero_lectores == 0) {
            sem_post(&sem_mutex);
            sem_post(&sem_prio_lect);
        }
        num_clientes_lectores--;
        if(num_clientes_lectores == 0){
            sem_post(&sem_cliente_lector);
        }
    sem_post(&control);
}









void *escritores_prio_lector(void *arg) {
    struct response response;

    if (numero_lectores == 1){
        sem_wait(&sem_prio_lect);
    }

    sem_wait(&control);
    num_clientes_escritores++;
    sem_post(&control);

    sem_wait(&sem_escritores_max);
    sem_wait(&sem_mutex);

    //INICIO SECCIÓN CRÍTICA
    //FIN SECCIÓN CRÍTICA

    if(block_ratio) {
        block_ratio = 0;
        sem_post(&sem_ratio);
    }
    sem_post(&sem_mutex);
    sem_post(&sem_escritores_max);

    sem_wait(&control);
    num_clientes_escritores--;
    sem_post(&control);

}