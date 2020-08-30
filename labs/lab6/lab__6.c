/*************************************************************************************************************
 * Maggie Horton
 * CS-451L Summer 2020
 * lab__6.c, task 2:
*************************************************************************************************************/

#include <stdlib.h>     //NULL
#include <stdio.h>      //printf
#include <sys/types.h>  //pid_t
#include <unistd.h>     //get_pid
#include <stdlib.h>     //exit, EXIT_FAILURE
#include <sys/wait.h>   //wait
#include <pthread.h>    //threads
#include "lab__6.h"


long sum;                   // Sum of generated values
int finished_producers;     // number of the producer that finished producing


//C: Mutex declaration and initialization
static pthread_mutex_t gen_mutex = PTHREAD_MUTEX_INITIALIZER;

//F: Condition variable declaration and initialization


int main(void) {

    //initialize random seed
    srand(time(NULL));
    sum = 0;

    //A: Create five generator threads
    pthread_t gen_thread1, gen_thread2, gen_thread3, gen_thread4, gen_thread5;
    pthread_create(&gen_thread1, NULL, generator_function, NULL);
    pthread_create(&gen_thread2, NULL, generator_function, NULL);
    pthread_create(&gen_thread3, NULL, generator_function, NULL);
    pthread_create(&gen_thread4, NULL, generator_function, NULL);
    pthread_create(&gen_thread5, NULL, generator_function, NULL);

    //D: Create print thread
    pthread_t print_thread;
    pthread_create(&print_thread, NULL, print_function, NULL);

    //B: Makes sure that all generator threads has finished before proceeding
    pthread_join(gen_thread1, NULL);
    pthread_join(gen_thread2, NULL);
    pthread_join(gen_thread3, NULL);
    pthread_join(gen_thread4, NULL);
    pthread_join(gen_thread5, NULL);


    //E: Makes sure that print thread has finished before proceeding
    pthread_join(print_thread, NULL);
    return (0);

}

void *generator_function(void *v_param) {

    int counter = 0;
    long this_sum = 0;


    while (counter < 200L) {
        pthread_mutex_lock(&gen_mutex);
        long tmp_num = sum; // start of critical section
        long rand_num = rand() % 10;
        printf("current sum of generated num up to now is %ld, adding %ld to it.\n", tmp_num, rand_num);
        sum = tmp_num + rand_num; // end of critical section
        pthread_mutex_unlock(&gen_mutex);

        counter++;
        this_sum += rand_num;
        usleep(100000);
    }


    printf("\033[1;31m"); // red
    printf("---+---+----+----------+---------+---+--+---+------+------+---+--+---\n");
    printf("The sum of produced items by thread %ld at the end is: %ld \n", pthread_self(), this_sum);
    printf("---+---+----+----------+---------+---+--+---+------+------+---+--+---\n\n");
    printf("\033[0m"); // reset color

    finished_producers++;

    //H: If all generator has finished fire signal for condition variable

    return (0);
}

void *print_function(void) {

    //G: Wait until all generator has finished

    printf("\033[1;36m"); // cyan
    printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("The value of counter at the end is: %ld \n", sum);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("\033[0m"); // reset color
}