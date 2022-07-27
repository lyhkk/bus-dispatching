#include <ctype.h>
#include <string.h>
#include "joint.h"
#include "FCFS.h"
#include "SSTF.h"
#include "SCAN.h"

int TOTAL_STATION;
int STRATEGY; // FCFS:0  SSTF:1  SCAN:2
int DISTANCE;
int time;
int *clockwise;
int *counterclockwise;
Bus bus;
req *request;

int initParam(); // initialize parameters, return the number of given ones
void initVar();
void print();
void serve();
int input(); // return -1:end     return 0:clock    return 1:wait for next request
void drive();

int main(int argc, char *argv[])
{
    char buf[30];
    int flag = 0;
    initParam();
    if (TOTAL_STATION <= 1 || TOTAL_STATION > 20 || DISTANCE <= 0 || DISTANCE >= 6)
    {
        printf("Error: incorrect parameters\n");
        return -1;
    }
    initVar();
    print();
    while ((flag = input()) != -1)
    {
        if (flag == 0)
        {
            time++;
            if (bus.isStop == 0)
            {
                drive();
            }
            serve();
            if (bus.isStop == 3)
            {
                drive();
                bus.isStop = 0;
            }
            print();
        }
    }
    printf("end\n");
    reset(request);
    return 0;
}

int initParam()
{
    TOTAL_STATION = 5;
    STRATEGY = 0;
    DISTANCE = 2;
    FILE *fp = fopen("dict.dic", "r");
    if (fp == NULL)
    {
        printf("Error: unable to read dict.dic\n");
        return -1;
    }
    char t[50] = {0};
    int num = 0;
    while (!feof(fp))
    {
        fgets(t, 49, fp);
        if (strncmp(t, "TOTAL_STATION", 13) == 0)
        {
            TOTAL_STATION = atoi(t + 16);
            num++;
        }
        if (strncmp(t, "STRATEGY", 8) == 0)
        {
            if (strncmp(t + 11, "FCFS", 4) == 0)
            {
                STRATEGY = 0;
            }
            if (strncmp(t + 11, "SSTF", 4) == 0)
            {
                STRATEGY = 1;
            }
            if (strncmp(t + 11, "SCAN", 4) == 0)
            {
                STRATEGY = 2;
            }
            num++;
        }
        if (strncmp(t, "DISTANCE", 8) == 0)
        {
            DISTANCE = atoi(t + 11);
            num++;
        }
    }
    fclose(fp);
    return num;
}
void initVar()
{
    time = 0;
    bus.isStop = 0;
    bus.position = 0;
    bus.direction = -1;
    bus.aim = -1;
    bus.target = calloc(TOTAL_STATION, sizeof(int));
    clockwise = calloc(TOTAL_STATION, sizeof(int));
    counterclockwise = calloc(TOTAL_STATION, sizeof(int));
    request = initReq();
}
void print()
{
    printf("TIME:%d\n", time);
    printf("BUS:\n");
    printf("position:%d\n", bus.position);
    printf("target:");
    for (int i = 0; i < TOTAL_STATION; i++)
    {
        printf("%d", bus.target[i]);
    }
    printf("\nSTATION:\n");
    printf("clockwise:");
    for (int i = 0; i < TOTAL_STATION; i++)
    {
        printf("%d", clockwise[i]);
    }
    printf("\ncounterclockwise:");
    for (int i = 0; i < TOTAL_STATION; i++)
    {
        printf("%d", counterclockwise[i]);
    }
    printf("\n");
}
void serve()
{
    if (STRATEGY == 0)
    {
        FCFS();
    }
    if (STRATEGY == 1)
    {
        SSTF();
    }
    if (STRATEGY == 2)
    {
        SCAN();
    }
}
int input()
{
    char s[100] = {0};
    int t = 0;
    while (1)
    {
        gets(s);
        if (s[0] == '#')
        {
            continue;
        }
        else
        {
            if (strncmp(s, "clock", 5) == 0 && strncmp(s, "clockwise", 9) != 0)
            {
                return 0;
            }
            else if (strncmp(s, "end", 3) == 0)
            {
                return -1;
            }
            else
            {
                for (int i = 0; i < strlen(s); i++)
                {
                    if (s[i] >= '0' && s[i] <= '9')
                    {
                        t = t * 10 + s[i] - '0';
                    }
                }
                t--;
                if (strncmp(s, "clockwise", 9) == 0 && clockwise[t] == 0)
                {
                    add(request, 1, t);
                    clockwise[t] = 1;
                }
                if (strncmp(s, "counterclockwise", 16) == 0 && counterclockwise[t] == 0)
                {
                    add(request, 2, t);
                    counterclockwise[t] = 1;
                }
                if (strncmp(s, "target", 6) == 0 && bus.target[t] == 0)
                {
                    add(request, 3, t);
                    bus.target[t] = 1;
                }
                return 1;
            }
        }
    }
}
void drive()
{
    if (bus.aim == -1)
    {
        return;
    }
    if (bus.direction == 1)
    {
        bus.position = (bus.position + 1) % (TOTAL_STATION * DISTANCE);
    }
    if (bus.direction == 0)
    {
        bus.position--;
        if (bus.position < 0)
        {
            bus.position += (TOTAL_STATION * DISTANCE);
        }
    }
}