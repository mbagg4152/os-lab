#define BASE                0       // people start and end at floor 0
#define DEFAULT_FLOORS      10      // default number of floors
#define DEFAULT_PEOPLE      1       // default number of people
#define DEFAULT_TIME        10      // default wandering time
#define MIN_WANDER          1       // people must wander for at least 1 sec
#define OPEN_HOURS          9       // from 8am - 5pm
#define TACK_F              "-f "
#define TACK_P              "-p "
#define TACK_W              "-w "
#define SZE 50

typedef struct Elevator {
    int max_wait;
    int top_floor;
    int direction;
    int this_floor;
    int next_floor;
} elevator;

typedef struct People {
    int pid;
    int floors_left;
    int floor_order[SZE];
    int wait_order[SZE];
} person;