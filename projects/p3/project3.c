#include "project3.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <getopt.h>
#include <zconf.h>

int main(int argc, char *argv[]) {
    get_args(argc, argv, &people_count, &max_wander, &floor_count);
    printf("%d %d %d\n", people_count, max_wander, floor_count);
    //get_stdin();
    make_sems();
    init_structs();
    thread_ops();
    return 0;
}
/**
 * Starting Functions
**/
void get_args(int count, char **args, int *people, int *wander, int *floors) {
    int tack;
    while ((tack = getopt(count, args, OPTS)) != -1) {
        switch (tack) {
            case TACK_P:
                *people = atoi(optarg);
                break;
            case TACK_W:
                *wander = atoi(optarg);
                break;
            case TACK_F:
                *floors = atoi(optarg);
                break;
            default:
                if (optopt == TACK_P || optopt == TACK_W || optopt == TACK_F) {
                    printf("nothing provided for -p, -w and/or -f\n");
                }
                break;
        }
    }
}

void make_sems() {
    sem_init(&printing_lock, 0, 1);
    for (int i = 0; i < 4; i++) {
        sem_init(&calling_lock[i], 0, 0);
    }
    sem_init(&waiting_lock, 0, 1);
}

void init_structs() {
    elevator = (struct Elevator *) malloc(sizeof(struct Elevator));
    elevator->max_wait = max_wander;
    elevator->floors = floor_count;
    elevator->direction = 1;
    elevator->this_floor = 0;
    elevator->next_floor = 1;
    people = (struct Person **) malloc(sizeof(struct Person) * (people_count - 1));
    peep_thread = (pthread_t *) malloc(sizeof(pthread_t) * (people_count - 1));

    for (int i = 0; i < people_count; i++) {
        people[i] = (struct Person *) malloc(sizeof(struct Person));
        people[i] = make_person(i);
    }
}

void thread_ops() {
    pthread_create(&elev_thread, NULL, move, (void *) elevator);
    for (int i = 0; i < people_count; i++) {
        pthread_create(&peep_thread[i], NULL, goto_work, (void *) people[i]);
    }
    pthread_join(elev_thread, NULL);
    for (int i = 0; i < people_count - 1; i++) {
        pthread_join(peep_thread[i], NULL);
    }
}

/**
 * Elevator Functions
**/
void *move(void *args) {
    struct Elevator *el = (struct Elevator *) args;
    int moved = 0;
    int done = 0;
    moving_message(UP);
    while (1) {
        sleep(1);
        el->this_floor = el->next_floor;
        sem_wait(&waiting_lock);
        if (people_queue[el->this_floor]) {
            halt(el->this_floor);
            moved = 1;
        }
        sem_post(&waiting_lock);
        int dir_change = shift(el->direction, el->this_floor);
        if (!dir_change) {
            if (el->direction == UP) {
                el->next_floor = el->this_floor + 1;
            } else {
                el->next_floor = el->this_floor - 1;
            }
        } else {
            if (el->direction == UP) {
                moving_message(DOWN);
                el->next_floor = el->this_floor - 1;
            } else {
                if (!moved) {
                    end_check(done, el->max_wait);
                    done = 1;
                } else {
                    done = 0;
                }
                moving_message(UP);
                el->next_floor = el->this_floor + 1;
                moved = 0;
            }
            el->direction = !el->direction;
        }
    }

}

void halt(int floor) {
    sem_wait(&printing_lock);
    printf("Elevator opening doors @ %i\n", floor);
    sem_post(&printing_lock);
    sem_post(&calling_lock[floor]);
    sleep(MIN_WANDER);
    sem_wait(&calling_lock[floor]);
}

void moving_message(int dir) {
    sem_wait(&printing_lock);
    if (dir) {
        printf("Elevator - Going Up!\n");
        for (int i = 0; i < floor_count; i++) {
            printf("%i people waiting on floor %i\n", people_queue[i], i);
        }
    } else {
        printf("Elevator - Heading Down!\n");
    }

    sem_post(&printing_lock);
}

void end_check(int done, int sleep_time) {
    if (done) {
        sem_wait(&printing_lock);
        printf("\nElevator is leaving\n");
        sem_post(&printing_lock);
        exit(0);

    }
    sem_wait(&printing_lock);
    printf("\nelevator at bottom floor with no waiting people\nsleeping for %i seconds\n", sleep_time);
    sem_post(&printing_lock);
    sleep(sleep_time);
}

int shift(int dir, int floor) {
    int changed = 0;
    if ((dir && floor == floor_count) || (!(dir && floor == 0))) {
        changed = 1;
    }
    return changed;
}

/**
 * Person Functions
**/
struct Person *make_person(int pid) {
    struct Person *person = (struct Person *) malloc(sizeof(struct Person));
    sem_wait(&printing_lock);
    printf("person %d:\nfloor\t\ttime\n", pid);
    int visited;
    scanf("%i", &visited);
    person->pid = pid;
    person->floors_left = visited;

    for (int i = 0; i < visited; i++) {
        scanf("%i", &person->floor_order[i]);
        scanf("%i", &person->wait_order[i]);

        printf("%i\t\t", person->floor_order[i]);
        printf("%i\n", person->wait_order[i]);
    }
    printf("\n");
    sem_post(&printing_lock);
    return person;

}

void *goto_work(void *v_params) {
    struct Person *worker = (struct Person *) v_params;
    int f_index = 0;
    int next_floor = 0;
    int this_floor = 0;
    int this_wait = 0;
    pid_t this_pid;

    while (1) {
        this_pid = worker->pid;
        this_wait = worker->wait_order[f_index];
        next_floor = worker->floor_order[f_index];
        enter_floor(next_floor, this_pid);
        this_floor = next_floor;
        printf("Person %d is wandering on floor %i for %i seconds", this_pid, this_floor, this_wait);
        sem_post(&printing_lock);
        sleep(this_wait);
        if (!(f_index < (worker->floors_left - 1))) {
            printf("person %d is out of here\n", this_pid);
            pthread_exit(0);
        }
        wait_to_enter(this_pid, this_floor);
        f_index++;
    }
}

void enter_floor(int floor, int pid) {
    mod_queue(floor, 1);
    sem_wait(&printing_lock);
    printf("person %d is waiting for elevator to hit floor %d\n", pid, floor);
    sem_post(&printing_lock);
    sem_wait(&calling_lock[floor]);
    sem_wait(&printing_lock);
    printf("person %d walking onto floor %d\n", pid, floor);
    sem_post(&calling_lock[floor]);
    sem_post(&printing_lock);
    mod_queue(floor, 0);

}

void wait_to_enter(int pid, int floor) {
    mod_queue(floor, 1);
    sem_wait(&printing_lock);
    printf("person %d is waiting at floor %d\n", pid, floor);
    sem_post(&waiting_lock);
    sem_post(&calling_lock[floor]);
    mod_queue(floor, 0);
}

void mod_queue(int floor, int add) {
    sem_wait(&waiting_lock);
    if (add) {
        people_queue[floor]++;
    } else {
        people_queue[floor]--;
    }
    sem_post(&waiting_lock);
}

