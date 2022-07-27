#include "mainwindow.h"
#define min(a,b) (((a) < (b)) ? (a) : (b))

int i_lyh = 0;
int preAim[11];
int bus::regain_aim( int pos ) { //重新获得目标,isLifted_aim不为0->目标为isLifted_aim
    int min_distance = 10000, Aim = -1;
    if(request->next == NULL) {
        return -1;
    } else {
        for( req *p = request->next; p != NULL; p = p->next ) {
            int aim_pos = p->aim * w->DISTANCE;
            int min_now = min(abs(aim_pos - pos), w->DISTANCE * w->TOTAL_STATION - abs(aim_pos - pos));
            if( min_now < min_distance ) {
                Aim = p->aim;
                min_distance = min_now;
                if(min_distance == 0) {
                    break;
                }
            } else if(min_now == min_distance) {
                if(direction_decision(p->aim) == 1) {
                    Aim = p->aim;
                }
            }
        }
        if( min_distance == 0 && i_lyh == 0) {
            delAimx(request, Aim, 1);
            delAimx(request, Aim, 2);
            delAimx(request, Aim, 3);
            clockwise[Aim] = 0;
            counterclockwise[Aim] = 0;
            target[Aim] = 0;
            Aim = regain_aim(pos);
        }
        if(i_lyh != 0) {
            Aim = preAim[i_lyh - 1];
            preAim[i_lyh - 1] = -1;
            i_lyh --;
        }
    }
    return Aim;
}

int bus::direction_decision(int aim) { //传入车目标站点，决定此时执行请求的方向
    int aim_pos = aim * w->DISTANCE;
    if( position > aim_pos ) {
        if((w->DISTANCE * w->TOTAL_STATION) + aim_pos - position > position - aim_pos) {
            return 0;    //逆时针更近
        } else {
            return 1;
        }
    } else {
        if((w->DISTANCE * w->TOTAL_STATION) + position - aim_pos < aim_pos - position) {
            return 0;    //逆时针更近
        } else {
            return 1;
        }
    }
}

void bus::getBy( int totDistance, int distance ) { //是否顺便以及获得目的地
    if(request->next == NULL) {
        return;
    }
    if(aim == -1) {
        aim = request->next->aim;
        if(aim != -1) {
            direction = direction_decision(aim);
            if(direction == 1) {
                position = (position + 1) % (w->TOTAL_STATION * w->DISTANCE);
            } else {
                position --;
            }
            if(position < 0) {
                position += w->TOTAL_STATION * w->DISTANCE;
            }
        }
    } else {
        for(req *end = request->next; end != NULL; end = end->next) {
            int aim_pos = aim * distance;
            int end_pos = end->aim * distance;
            int pos = position;
            int end_direction = direction_decision(end->aim);
            if(direction == end_direction) {
                int min_aim = min(abs(aim_pos - pos), totDistance - abs(aim_pos - pos));
                int min_end = min(abs(end_pos - pos), totDistance - abs(end_pos - pos));
                if(min_end < min_aim) {
                    int signal = 0;
                    if(end->type == 1 && direction == 1) {
                        signal = 1;
                    }
                    if(end->type == 2 && direction == 0) {
                        signal = 1;
                    }
                    if(end->type == 3) {
                        signal = 1;
                    }
                    if(signal == 1) {
                        preAim[i_lyh] = aim;
                        i_lyh++;
                        aim = end->aim;
                    }
                }
            }
        }
    }
    return;
}

void bus::SSTF() {
    /*
        1>到站->是否是目标(否->return,是->执行clock下的函数)
        2>有请求->对尾结点判断是否顺便服务
    */
    if(request->next == NULL) {
        return;    //停止在某一站点，无请求
    } else {
        getBy( w->DISTANCE * w->TOTAL_STATION, w->DISTANCE );
        if( position % w->DISTANCE == 0 && aim == position / w->DISTANCE ) { //到站
            int station = position / w->DISTANCE;
            if(isStop == 1) {
                if(i_lyh == 0) {
                    delAimx(request, station, 1);
                    delAimx(request, station, 2);
                    delAimx(request, station, 3);
                    if(counterclockwise[station] == 1) {
                        counterclockwise[station] = 0;
                    }
                    if(clockwise[station] == 1) {
                        clockwise[station] = 0;
                    }
                    if(target[station] == 1) {
                        target[station] = 0;
                    }
                } else {
                    delAimx(request, station, 3);
                    target[station] = 0;
                    if(direction == 1) {
                        delAimx(request, station, 1);
                        clockwise[station] = 0;
                    }
                    if(direction == 0) {
                        delAimx(request, station, 2);
                        counterclockwise[station] = 0;
                    }
                }
                int new_aim = regain_aim(position);
                if(new_aim != -1) {
                    aim = new_aim;
                    getBy(w->DISTANCE * w->TOTAL_STATION, w->DISTANCE);
                    direction = direction_decision(new_aim);
                } else {
                    aim = -1;
                    direction = -1;
                }
                isStop = 0;
            } else {
                isStop = 1;
                return;
            }
        }
    }
    return;
}
