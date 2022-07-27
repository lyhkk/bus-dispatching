#include "request.h"
req *initReq()
{
    req *head = malloc(sizeof(req));
    head->next = NULL;
    head->type = 114514;
    head->aim = 114514;
    return head;
}
void add(req *head, int type, int aim)
{
    req *p = malloc(sizeof(req));
    p->type = type;
    p->aim = aim;
    p->next = NULL;
    req *end = head;
    while (end->next)
        end = end->next;
    end->next = p;
}
void del(req *head, req *to_del)
{
    int flag = 0;
    if (!head->next)
        return;
    req *pp = head, *p = head->next;
    while (p)
    {
        if (p == to_del)
        {
            flag = 1;
            break;
        }
        p = p->next;
        pp = pp->next;
    }
    if (!flag)
        return;
    pp->next = p->next;
    free(p);
    p=NULL;
}
extern void delAimx(req *head, int aim, int type)
{
    int flag = 0;
    if (!head->next)
        return;
    req *pp = head, *p = head->next;
    while (p)
    {
        if (p->aim == aim && p->type == type)
        {
            flag = 1;
            break;
        }
        p = p->next;
        pp = pp->next;
    }
    if (!flag)
        return;
    pp->next = p->next;
    free(p);
    p=NULL;
}
void reset(req *head)
{
    req *p;
    while (p)
    {
        p = head;
        head = p->next;
        free(p);
        p = NULL;
    }
}