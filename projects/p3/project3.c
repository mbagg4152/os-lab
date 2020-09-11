#include "project3.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <getopt.h>
#include <zconf.h>

int main(int argc, char *argv[]) {
    check_stdin();
    get_options(argc, argv, &body_count, &max_time, &tiers);
    init_values();
    run_threads();
    return 0;
}

void check_stdin() {
    if (fseek(stdin, 0, SEEK_END), ftell(stdin) > 0) {
        rewind(stdin);
    } else {
        printf("Either a file was not directed or the file supplied is empty. Exiting.\n");
        exit(1);
    }
}

void get_options(int count, char **args, int *people, int *wander, int *floors) {
    int arg;
    while ((arg = getopt(count, args, OPTS)) != -1) {
        switch (arg) {
            case OPT_P:
                *people = atoi(optarg);
                break;
            case OPT_W:
                *wander = atoi(optarg);
                break;
            case OPT_F:
                *floors = atoi(optarg);
                break;
            default:
                printf("Something is wrong with the supplied options\n");
                break;
        }
    }
}

void init_values() {
    sem_init(&display_lock, 0, 1);
    sem_init(&waiting_lock, 0, 1);
    sem_init(&bottom_lock, 0, 1);
    for (int i = 0; i < tiers; i++) {
        sem_init(&moving_lock[i], 0, 0);
    }

    init_elevator();
    people = (struct Person **) malloc(sizeof(struct Person) * (body_count - 1));
    worker_threads = (pthread_t *) malloc(sizeof(pthread_t) * (body_count - 1));

    for (int i = 0; i < body_count; i++) {
        people[i] = (struct Person *) malloc(sizeof(struct Person));
        people[i] = init_person(i);
    }
}

void run_threads() {
    pthread_create(&elev_thread, NULL, move_elevator, (void *) lift);
    for (int i = 0; i < body_count; i++) {
        pthread_create(&worker_threads[i], NULL, start_ride, (void *) people[i]);
    }
    pthread_join(elev_thread, NULL);
    for (int i = 0; i < body_count - 1; i++) {
        pthread_join(worker_threads[i], NULL);
    }
}

void init_elevator() {
    lift = (struct Elevator *) malloc(sizeof(struct Elevator));
    lift->direction = 1;
    lift->done = 0;
    lift->floors = tiers;
    lift->next_floor = 1;
    lift->this_floor = BOTTOM;
    lift->wait = max_time;
}

void *move_elevator(void *args) {
    struct Elevator *hoist = (struct Elevator *) args;
    sleep(WAIT);
    while (keep_running) {
        sleep(WAIT);
        hoist->this_floor = hoist->next_floor;
        if (hoist->this_floor == BOTTOM) {
            sem_wait(&bottom_lock);
            on_bottom = 1;
            sem_post(&bottom_lock);
        } else {
            if (on_bottom) {
                sem_wait(&bottom_lock);
                on_bottom = 0;
                sem_post(&bottom_lock);
            }
        }

        sem_wait(&waiting_lock);
        if (currently_waiting[hoist->this_floor]) {
            hoist->passed = 0;
            hoist->done = 0;
            open_doors(hoist->this_floor);
        } else {
            hoist->passed++;
            printf("%s Passing floor %d\n", PRINT_E, hoist->this_floor);
        }
        sem_post(&waiting_lock);

        if (hoist->passed >= ((tiers - 1) * 2)) {
            if (hoist->done) {
                sem_wait(&display_lock);
                printf("%s Made full trip & waited but nobody showed up. Leaving...\n", PRINT_E);
                sem_post(&display_lock);
                pthread_exit(0);
            } else {
                sem_wait(&display_lock);
                printf("%s Couldn't find anybody waiting, sleeping for %d sec.\n", PRINT_E, hoist->wait);
                sem_post(&display_lock);
                sleep(hoist->wait);
                hoist->done = 1;
            }
        }

        int change_direction = swap_direction(hoist->this_floor, (hoist->floors - 1));
        if (change_direction == MOVE_UP) {
            going_up(hoist);
        } else if (change_direction == MOVE_DOWN) {
            going_down(hoist);
        } else {
            if (hoist->direction == D_UP) {
                hoist->next_floor++;

            } else if (hoist->direction == D_DOWN) {
                hoist->next_floor--;
            }
        }
    }
}

void going_up(struct Elevator *e) {
    sem_wait(&display_lock);
    printf("\n%s Going Up!\n", PRINT_E);
    sem_post(&display_lock);
    show_waiting_people();
    e->direction = D_UP;
    e->next_floor++;
}

void going_down(struct Elevator *e) {
    sem_wait(&display_lock);
    printf("\n%s Heading Down!\n", PRINT_E);
    sem_post(&display_lock);
    show_waiting_people();
    e->direction = D_DOWN;
    e->next_floor--;
}

void open_doors(int floor) {
    sem_wait(&display_lock);
    printf("%s Opening on floor %i\n", PRINT_E, floor);
    sem_post(&display_lock);
    sem_post(&moving_lock[floor]);
    sleep(WAIT);
    sem_wait(&moving_lock[floor]);
}

void show_waiting_people() {
    sem_wait(&display_lock);
    printf("Floor\t\tNo. of people waiting\n");
    for (int i = 0; i < tiers; i++) {
        printf("%i\t\t%i\n", i, currently_waiting[i]);
    }
    printf("\n");
    sem_post(&display_lock);
}

int swap_direction(int current, int top) {
    if (current == top) {
        return MOVE_DOWN;
    } else if (current == BOTTOM) {
        return MOVE_UP;
    } else {
        return 0;
    }
}

struct Person *init_person(int new_pid) {
    struct Person *person = (struct Person *) malloc(sizeof(struct Person));
    sem_wait(&display_lock);
    printf("%s %d\n%sFloor\t\tTime\n", PRINT_P, new_pid, PRINT_T);

    int wandering_pairs;
    scanf("%i", &wandering_pairs); // read in line of single int from stdin
    person->pid = new_pid;
    person->floors_left = wandering_pairs;

    for (int i = 0; i < wandering_pairs; i++) {
        scanf("%i", &person->floors[i]);
        scanf("%i", &person->times[i]);

        printf("%s%i\t\t", PRINT_T, person->floors[i]);
        printf("%i\n", person->times[i]);
    }
    person->this_floor = person->floors[0];
    printf("\n");
    sem_post(&display_lock);
    return person;

}

void *start_ride(void *void_param) {
    struct Person *human = (struct Person *) void_param;
    int index = 0, next_floor = 0, wander_time = 0, this_floor = 0;
    while (keep_running) {
        wander_time = human->times[index];
        if (wander_time > max_time) {
            wander_time = max_time;
        }
        next_floor = human->floors[index];
        this_floor = human->this_floor;
        leave_lift(human);
        human->this_floor = next_floor;

        sem_wait(&display_lock);
        printf("%s %d: Wandering on floor %i for %i sec.\n", PRINT_P, human->pid, this_floor, wander_time);
        sem_post(&display_lock);

        sleep(wander_time);
        if (human->floors_left < 1) {
            if (on_bottom) {
                sem_wait(&display_lock);
                printf("%s %d: Leaving... \n", PRINT_P, human->pid);
                sem_post(&display_lock);
                pthread_exit(0);
            } else {
                continue;
            }
        }
        enter_lift(human);
        index++;
        human->floors_left--;
    }

}

void leave_lift(struct Person *mortal) {
    sem_wait(&waiting_lock);
    currently_waiting[mortal->this_floor]++;
    sem_post(&waiting_lock);

    sem_wait(&moving_lock[mortal->this_floor]);
    sem_post(&moving_lock[mortal->this_floor]);

    sem_wait(&waiting_lock);
    currently_waiting[mortal->this_floor]--;
    sem_post(&waiting_lock);
}

void enter_lift(struct Person *mortal) {
    sem_wait(&waiting_lock);
    currently_waiting[mortal->this_floor]++;
    sem_post(&waiting_lock);

    sem_wait(&moving_lock[mortal->this_floor]);
    sem_wait(&display_lock);
    printf("%s %d: Getting on elevator on floor %d\n", PRINT_P, mortal->pid, mortal->this_floor);
    sem_post(&display_lock);
    sem_post(&waiting_lock);
    sem_post(&moving_lock[mortal->this_floor]);

    sem_wait(&waiting_lock);
    currently_waiting[mortal->this_floor]--;
    sem_post(&waiting_lock);
}