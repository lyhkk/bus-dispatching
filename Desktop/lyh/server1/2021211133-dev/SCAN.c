#include "joint.h"
#include "SCAN.h"
#define L (TOTAL_STATION * DISTANCE)

req *getShortest()
{
    req *res = request;
    int s = 0, min = res->aim;
    for (req *t = request->next; t != NULL; t = t->next)
    {
        s = abs(t->aim * DISTANCE - bus.position);
        if (bus.direction != -1)
        {
            s = ((t->aim * DISTANCE < bus.position) ^ bus.direction) ? s : (L - s);
        }
        else
        {
            s = (2 * s < L) ? s : (L - s);
        }
        if (s < min || (s == min && getDirection(t) == 1))
        {
            res = t;
            min = s;
        }
    }
    return res;
}
int getDirection(req *aimNow)
{
    int s = abs(aimNow->aim * DISTANCE - bus.position);
    if (2 * s == L)
    {
        return 1;
    }
    int d = (aimNow->aim * DISTANCE > bus.position);
    if (2 * s > L)
    {
        d = !d;
    }
    return d;
}
void refreshAim()
{
    delAimx(request, bus.position / DISTANCE, 1);
    delAimx(request, bus.position / DISTANCE, 2);
    delAimx(request, bus.position / DISTANCE, 3);
    clockwise[bus.position / DISTANCE] = 0;
    counterclockwise[bus.position / DISTANCE] = 0;
    bus.target[bus.position / DISTANCE] = 0;
}
void SCAN()
{
    req *t = NULL, *s;
    static req *master;
    if (bus.position % DISTANCE != 0)
        return;
    if (bus.aim == -1) // no requests there
    {
        t = getShortest();
        if (t->aim * DISTANCE == bus.position)
        {
            refreshAim();
            t = getShortest();
        }
        if (request->next == NULL)
        {
            return;
        }
        master = t;
        bus.aim = t->aim * DISTANCE;
        bus.direction = getDirection(t);
        bus.position = (bus.direction == 1) ? ((bus.position + 1) % L) : (bus.position - 1);
        bus.position = (bus.position < 0) ? (bus.position + L) : bus.position;
        return;
    }
    // does not reach the aim
    if (bus.isStop == 0)
    {
        for (s = request; s != NULL; s = s->next)
        {
            if (s->aim * DISTANCE == bus.position)
            {
                bus.isStop = 1;
                return;
            }
        }
        if (bus.aim * DISTANCE == bus.position || master->aim * DISTANCE == bus.position)
        {
            bus.isStop = 1;
            return;
        }
    }
    if (bus.isStop == 1) // to stop
    {
        bus.isStop = 2;
        if (master->aim * DISTANCE == bus.position)
        {
            master = NULL;
        }
        refreshAim();
        return;
    }
    if (bus.isStop == 2) // bus stop
    {
        bus.isStop = 3;
        if (request->next == NULL)
        {
            bus.aim = -1;
            bus.direction = -1;
            bus.isStop = 0;
            return;
        }
        if (master == NULL)
        {
            t = getShortest();
            master = t;
            bus.aim = t->aim * DISTANCE;
            bus.direction = getDirection(t);
            return;
        }
    }
}