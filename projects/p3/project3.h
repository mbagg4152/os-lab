#define ARR_SZE             300                 // array size
#define BOTTOM              0                   // people start and end at floor 0
#define DEF_FLOORS          10                  // default number of floors
#define DEF_PEOPLE          1                   // default number of people
#define DEF_TIME            10                  // default wandering time
#define D_DOWN              0                   // down represented by 0
#define D_UP                1                   // up represented by 1
#define MOVE_DOWN           33                  // arbitrary flag value for when elevator is at top
#define MOVE_UP             22                  // arbitrary flag value for when elevator is at bottom
#define OPTS                "p:w:f:"            // pattern used in getopt to read cmdline args
#define OPT_F               'f'                 // number of floors option
#define OPT_P               'p'                 // number of people option
#define OPT_W               'w'                 // wander time option
#define PRINT_E             "Elevator:"         // prefix for elevator print statements
#define PRINT_P             "\t\t\t\t\tPerson"  // prefix for person print statements
#define PRINT_T             "\t\t\t\t\t"        // used to keep items aligned for person statements
#define WAIT                1                   // elevator wait time

#include <semaphore.h>


struct Elevator {
    int direction;          // direction of elevator travel
    int done;               // program-ending flag
    int floors;             // number of floors
    int next_floor;         // either +1 or -1 of this_floor
    int passed;             // counter for number of floors passed
    int this_floor;         // current floor
    int wait;               // max wait/wander time
};

struct Person {
    int floors[ARR_SZE];    // floors to travel to
    int floors_left;        // remaining floors to visit
    int pid;                // unique personal id
    int this_floor;         // current floor
    int times[ARR_SZE];     // time to be spent on each floor
};

sem_t moving_lock[ARR_SZE]; // used to block on different floors when one of the threads' items is moving
sem_t display_lock;         // used to surround print statements
sem_t waiting_lock;         // used when modifying the number of people waiting on each floor
sem_t bottom_lock;          // used when modifying flag which is set to 1 when elevator is at bottom


// assign default values to each var in case they're not supplied in args
int tiers = DEF_FLOORS;     // number of floors in the building
int max_time = DEF_TIME;    // max wandering time
int body_count = DEF_PEOPLE;// number of people
int keep_running = 1;

struct Elevator *lift;
struct Person **people;
pthread_t *worker_threads;
pthread_t elev_thread;

int currently_waiting[ARR_SZE];
int on_bottom = 0;

void check_stdin();
void get_options(int count, char **args, int *people, int *wander, int *floors);
void init_values();
void run_threads();

int swap_direction(int current, int top);
void *move_elevator(void *args);
void going_down(struct Elevator *e);
void going_up(struct Elevator *e);
void init_elevator();
void open_doors(int floor);
void show_waiting_people();

struct Person *init_person(int new_pid);
void *start_ride(void *void_param);
void enter_lift(struct Person *mortal);
void leave_lift(struct Person *mortal);


