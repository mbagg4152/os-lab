#ifndef PROJECT_3
#define PROJECT_3 1


#define ARR_SZE             300                 // array size
#define BASE              0                   // people start and end at floor 0
#define DEF_FLOORS          10                  // default number of floors
#define DEF_PEOPLE          1                   // default number of people
#define DEF_TIME            10                  // default wandering time
#define D_DOWN              0                   // down represented by 0
#define D_UP                1                   // up represented by 1
#define MOVE_DOWN           33                  // arbitrary flag value for when elevator is at top_floor
#define MOVE_UP             22                  // arbitrary flag value for when elevator is at bottom
#define OPTS                "p:w:f:"            // pattern used in getopt to read cmdline args
#define OPT_F               'f'                 // number of floors option
#define OPT_P               'p'                 // number of people option
#define OPT_W               'w'                 // wander time option
#define PRINT_E             "Elevator:"         // prefix for elevator print statements
#define PRINT_P             "\t\t\t\t\t\tPerson"  // prefix for person print statements
#define WAIT                1                   // elevator wait time
#define true 1
#define false 0

#define PRINT_LEN 2048

#define COUNT_NAME "counter_sem"
#define PERMS 0660

#include <semaphore.h>


struct Elevator {
    int direction;          // direction of elevator travel
    int done;               // program-ending flag
    int floors;             // number of floors
    int next_floor;         // either +1 or -1 of this_floor
    int passed;             // counter for number of floors passed
    int this_floor;         // current floor
    int wait;               // max wait/wander time
    pthread_t thread_id;
};

struct Person {
    int floors[ARR_SZE];    // floors to travel to
    int floors_left;        // remaining floors to visit
    int pid;                // unique personal id
    int this_floor;         // current floor
    int times[ARR_SZE];     // time to be spent on each floor
    int done;
    int wandered[ARR_SZE];
    int last_pair_index;
    pthread_t thread_id;

};

sem_t arrival_lock;
sem_t bottom_lock;          // used when modifying flag which is set to 1 when elevator is at bottom
sem_t display_lock;         // used to surround print statements
sem_t leave_lock;
sem_t moving_lock[ARR_SZE]; // used to block on different floors when one of the threads' items is moving
sem_t waiting_lock;         // used when modifying the number of people waiting on each floor
sem_t changing_lock;
sem_t crit_lock;
sem_t allowed_entry[ARR_SZE];
sem_t allowed_exit[ARR_SZE];
sem_t allowed_quit;
sem_t ready_check;

// assign default values to each var in case they're not supplied in args
int body_count = DEF_PEOPLE;    // number of people
int max_time = DEF_TIME;        // max wandering time
int tiers = DEF_FLOORS;         // number of floors in the building


struct Elevator *lift;
struct Person **people;

pthread_t *worker_threads;
pthread_t elev_thread;

int currently_waiting[ARR_SZE];
int keep_running = 1;
int last_person_index = 0;
int top_floor = 0;
int can_exit[ARR_SZE];
int can_enter[ARR_SZE];
int can_quit;
int ready_quit_count;

void check_stdin();
void get_options(int a_count, char **o_args, int *p_count, int *w_time, int *f_count);
void init_values();
void run_threads();

void *move_elevator(void *);

void init_elevator();
void open_doors(int floor, int want_leave);
void show_waiting_people(int);

struct Person *init_person(int);
void *start_ride(void *);
void enter_lift(struct Person *);
void leave_lift(struct Person *, int);

int lock_print(const char *, ...);


#endif