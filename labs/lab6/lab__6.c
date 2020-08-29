/*************************************************************************************************************
 * Maggie Horton
 * CS-451L Summer 2020
 * lab__6.c: 
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

//F: Condition variable declaration and initialization


int main(void) {

    //initialize random seed
    srand(time(NULL));
    sum = 0;

    //A: Create five generator threads

    pthread_t gen1, gen2, gen3, gen4, gen5;
    pthread_create(&gen1, NULL, generator_function, NULL);
    pthread_create(&gen2, NULL, generator_function, NULL);
    pthread_create(&gen3, NULL, generator_function, NULL);
    pthread_create(&gen4, NULL, generator_function, NULL);
    pthread_create(&gen5, NULL, generator_function, NULL);
    //D: Create print thread

    //B: Makes sure that all generator threads has finished before proceeding
    pthread_join(gen1, NULL);
    pthread_join(gen2, NULL);
    pthread_join(gen3, NULL);
    pthread_join(gen4, NULL);
    pthread_join(gen5, NULL);

    print_function();// remove this for TASK 2

    //E: Makes sure that print thread has finished before proceeding

    return (0);

}

void *generator_function(void *v_param) {

    int counter = 0;
    long sum_this_generator = 0;

    while (counter < 200L) {
        long tmpNumber = sum;
        long rnd_number = rand() % 10;
        printf("current sum of the generated number up to now is %ld going to add %ld to it.\n", tmpNumber,
               rnd_number);
        sum = tmpNumber + rnd_number;
        counter++;
        sum_this_generator += rnd_number;
        usleep(100000);
    }

    printf("\033[01;33m"); // prints in yellow color
    printf("--+---+----+----------+---------+---+--+---+------+------+---+--+-\n");
    printf("The sum of produced items by thread %ld at the end is: %ld \n", pthread_self(),
           sum_this_generator);
    printf("--+---+----+----------+---------+---+--+---+------+------+---+--+-\n\n");
    printf("\033[0m");

    finished_producers++;

    //H: If all generator has finished fire signal for condition variable

    return (0);
}

void *print_function(void) {

    //G: Wait until all generator has finished

    printf("\033[1;34m");
    printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("The value of counter at the end is: %ld \n", sum);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("\033[0m");
}
