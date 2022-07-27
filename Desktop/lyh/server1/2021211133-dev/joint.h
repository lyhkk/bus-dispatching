#include <math.h>
#include "request.h"
extern int TOTAL_STATION;
extern int STRATEGY; // FCFS:0  SSTF:1  SCAN:2
extern int DISTANCE;
extern int time;
extern int *clockwise;
extern int *counterclockwise;
typedef struct Bus
{
    int position;
    int direction; // clockwise:1  counterclockwise:0
    int aim;
    int isStop; // 0:running  1:to stop  2:stop for 1 sec  3:to start again
    int *target;
} Bus;
extern Bus bus;
extern req *request;
