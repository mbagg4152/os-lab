#include <semaphore.h>
#define BASE                0       // people start and end at floor 0
#define DEFAULT_FLOORS      10      // default number of floors
#define DEFAULT_PEOPLE      1       // default number of people
#define DEFAULT_TIME        10      // default wandering time
#define MIN_WANDER          1       // people must wander for at least 1 sec
#define OPEN_HOURS          9       // from 8am - 5pm
#define OPTS "p:w:f:"
#define SZE 256
#define TACK_F              'f'
#define TACK_P              'p'
#define TACK_W              'w'
#define UP 1
#define DOWN 0
typedef struct Elevator {
    int max_wait;
    int floors;
    int direction;
    int this_floor;
    int next_floor;
} Elevator;

typedef struct Person {
    int pid;
    int floors_left;
    int floor_order[SZE];
    int wait_order[SZE];
} Person;

char *file_data;
int data_size = SZE;
int floor_count = DEFAULT_FLOORS;
int max_wander = DEFAULT_TIME;
int people_count = DEFAULT_PEOPLE;
sem_t calling_lock[SZE];
sem_t printing_lock;
sem_t waiting_lock;
struct Elevator *elevator;
struct Person **people;
pthread_t *peep_thread;
pthread_t elev_thread;
int people_queue[SZE];

void get_args(int count, char **args, int *people, int *wander, int *floors);
void make_sems();
void init_structs();
void thread_ops();

struct Person *make_person(int pid);
void *goto_work(void *v_params);
void enter_floor(int floor, int pid);
void wait_to_enter(int pid, int floor);
void mod_queue(int floor, int add);

void moving_message(int dir);
void *move(void *args);
void halt(int floor);
void end_check(int done, int sleep_time);
int shift(int dir, int floor);