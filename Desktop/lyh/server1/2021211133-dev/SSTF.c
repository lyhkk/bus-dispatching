#include<math.h>
#include<joint.h>
#include"SSTF.h"
#define min(a,b) (((a) < (b)) ? (a) : (b))
int preAim[11];
int i_lyh = 0;//顺便服务数组的下标

int regain_aim( int pos ) {//重新获得目标
    int min_distance = 10000, Aim = -1;
    if(request->next == NULL) return -1;
    else
    {
        for(req* p = request->next; p != NULL; p = p->next) {
            int aim_pos = p->aim * DISTANCE;
            int min_now = min(abs(aim_pos-pos),DISTANCE*TOTAL_STATION-abs(aim_pos - pos));
            if(min_now < min_distance) { 
                if((min_now==0 && bus.isStop==2))
                {
                    if((p->type==1&&bus.direction==1)||(p->type==3)||(p->type==2&&bus.direction==0))
                    {
                        Aim = p->aim;
                        min_distance = min_now;
                        if(min_distance == 0) break;
                    }
                }
                else if(min_now > 0) {
                    Aim = p->aim;
                    min_distance = min_now;
                    if(min_distance == 0) break;                    
                }
            }
            else if(min_now == min_distance) {
                if(direction_decision(p->aim) == 1) Aim = p->aim;
            }
        }
        if(min_distance == 0 && i_lyh == 0) {
            delAimx(request, Aim, 1);
            delAimx(request, Aim, 2);
            delAimx(request, Aim, 3);
            clockwise[Aim] = 0;
            counterclockwise[Aim] = 0;
            bus.target[Aim] = 0;
            Aim = regain_aim(pos);
        }
        else if(min_distance == 0 && i_lyh != 0) {
            delAimx(request, Aim, 3);
            bus.target[Aim] = 0;
            if(bus.direction == 1) {
                delAimx(request, Aim, 1);
                clockwise[Aim] = 0;
            }
            if(bus.direction == 0) {
                delAimx(request, Aim, 2);
                counterclockwise[Aim] = 0;
            }
            Aim = regain_aim(pos);
        }
        if(i_lyh != 0) {
            Aim = preAim[i_lyh-1];
            preAim[i_lyh-1] = -1;
            i_lyh --;
        }
    }
    return Aim;
}

int direction_decision(int aim) {//传入车目标站点，决定此时执行请求的方向
    int aim_pos = aim * DISTANCE;
    if(bus.position > aim_pos) {
        if((DISTANCE * TOTAL_STATION) + aim_pos - bus.position > bus.position - aim_pos) 
            return 0;//逆时针更近
        else return 1;
    }
    else {
        if((DISTANCE * TOTAL_STATION) + bus.position - aim_pos < aim_pos - bus.position) 
            return 0;//逆时针更近
        else return 1;
    }
}

void getBy(int totDistance, int distance){//是否顺便以及获得目的地
    if(request->next == NULL) return;
    if(bus.aim == -1)
    {
        bus.aim = regain_aim(bus.position);
        if(bus.aim != -1)
        {
            bus.direction = direction_decision(bus.aim);
            if(bus.direction == 1)
                bus.position = (bus.position + 1) % (TOTAL_STATION * DISTANCE);
            else bus.position --;
            if(bus.position < 0) bus.position += TOTAL_STATION * DISTANCE;
        }
    }
    else 
    {
        for(req* end = request->next; end != NULL; end = end->next) {
            int aim_pos = bus.aim*distance;
            int end_pos = end->aim * distance;
            int pos = bus.position;
            int end_direction = direction_decision(end->aim);
            if(bus.direction == end_direction || bus.position == end_pos)
            {
                int min_aim = min(abs(aim_pos - pos), totDistance - abs(aim_pos - pos));
                int min_end = min(abs(end_pos - pos), totDistance - abs(end_pos - pos));
                if(min_end < min_aim) {
                    int signal = 0;
                    if(end->type == 1 && bus.direction == 1) signal = 1;
                    if(end->type == 2 && bus.direction == 0) signal = 1;
                    if(end->type == 3) signal = 1;
                    if(signal == 1) {
                        preAim[i_lyh] = bus.aim;
                        i_lyh++;
                        bus.aim = end->aim;
                    }
                }
            }
        }
    }
    return;
}

void SSTF(){
    if(request->next == NULL) return;//停止在某一站点，无请求
    else{
        if(bus.isStop != 2) getBy(DISTANCE*TOTAL_STATION, DISTANCE);
        else {
            bus.aim = regain_aim(bus.position);
            if(bus.aim != -1) bus.direction = direction_decision(bus.aim);
            getBy(DISTANCE*TOTAL_STATION, DISTANCE);
            bus.isStop = 3;
        }
        if(bus.position%DISTANCE == 0&&bus.aim==bus.position/DISTANCE&&bus.isStop!=2) {
            int station = bus.position / DISTANCE;
            if(bus.isStop == 1){
                if(i_lyh == 0){
                    delAimx(request, station, 1);
                    delAimx(request, station, 2);
                    delAimx(request, station, 3);
                    if(counterclockwise[station] == 1) counterclockwise[station] = 0;
                    if(clockwise[station] == 1) clockwise[station] = 0;
                    if(bus.target[station] == 1) bus.target[station] = 0;
                }
                else {
                    delAimx(request, station, 3);
                    bus.target[station] = 0;
                    if(bus.direction == 1) {
                        delAimx(request, station, 1);
                        clockwise[station] = 0;
                    }
                    if(bus.direction == 0) {
                        delAimx(request, station, 2);
                        counterclockwise[station] = 0;
                    }
                }
                bus.aim = -1;
                if(i_lyh == 0) bus.direction = -1;
                bus.isStop = 2;
                return;
            }
            else {
                bus.isStop = 1;
                return;
            }
        } 
    } 
}