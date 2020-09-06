#include "project3.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <getopt.h>

int people_count = DEFAULT_PEOPLE;
int max_wander = DEFAULT_TIME;
int floor_count = DEFAULT_FLOORS;

void parse(int count, char *args[], int *people, int *wander, int *floors);

int main(int argc, char *argv[]) {
    parse(argc, argv, &people_count, &max_wander, &floor_count);
    printf("%d %d %d\n", people_count, max_wander, floor_count);
    return 0;
}

void parse(int count, char *args[], int *people, int *wander, int *floors) {
    int opt;
    // Read the arguments the user passed in
    while ((opt = getopt(count, args, "p:w:f:")) != -1) {
        switch (opt) {
            case 'p':
                *people = atoi(optarg);
                break;
            case 'w':
                *wander = atoi(optarg);
                break;
            case 'f':
                *floors = atoi(optarg);
                break;
            default:
                // Check to see if the current option is any of the
                // default options
                if (optopt == 'p') {
                    printf("nothing provided for -p, defaulting\n");
                }
                if (optopt == 'w') {
                    printf("nothing provided for -w, defaulting\n");
                }
                if (optopt == 'f') {
                    printf("nothing provided for -f, defaulting\n");
                }
                break;
        }
    }
}