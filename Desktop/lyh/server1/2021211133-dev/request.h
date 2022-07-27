#include <stdio.h>
#include <stdlib.h>
typedef struct re
{
    int type; // type 1:clockwise;type 2:counterclockwise;type 3:target
    int aim;
    struct re *next;
} req;
extern req *initReq();
extern void add(req *head, int type, int aim);
extern void del(req *head, req *to_del);
extern void delAimx(req *head, int aim, int type);
extern void reset(req *head);