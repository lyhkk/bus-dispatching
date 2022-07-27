
#include "joint.h"
#include "FCFS.h"

//传入车位置和目标站点，决定此时执行请求的方向，0为逆时针，1为顺时针
int get_direction(int aim)
{
	int aim_pos = aim * DISTANCE;
	if(bus.position > aim_pos)
	{
		if((DISTANCE * TOTAL_STATION) + aim_pos - bus.position > bus.position - aim_pos) //逆时针更近
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		if((DISTANCE * TOTAL_STATION) + bus.position - aim_pos < aim_pos - bus.position) //逆时针更近
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
}

//获取下一个目标站点 
int getNextReq()
{
	req *nextReq = request->next;
	if(nextReq == NULL)
	{
		return -1;
	}
	else
	{
		return nextReq->aim;
	}
}

void FCFS()
{
	if(request->next == NULL || request == NULL) return; //无请求，停止在某一站点
	else
	{
		if(bus.position % DISTANCE == 0 && bus.aim == bus.position / DISTANCE) //到站
		{
			int station = bus.position / DISTANCE;
			if(bus.isStop == 1)
			{
				while(request->next->next!=NULL&&request->next->aim==request->next->next->aim)
				{
					switch(request->next->type) //多个请求一并处理 
					{
						case 1:
							clockwise[station] = 0;
							break;
						case 2:
							counterclockwise[station] = 0;
							break;
						default:
							bus.target[station] = 0;
							break;
					}
					del(request, request->next);
				}
				if(request->next != NULL)
				{
					switch(request->next->type)
					{
						case 1:
							clockwise[station] = 0;
							break;
						case 2:
							counterclockwise[station] = 0;
							break;
						default:
							bus.target[station] = 0;
							break;
					}
					del(request, request->next);
				}
				int nextAim = getNextReq();
				if(nextAim != -1)
				{
					bus.aim = nextAim;
					bus.direction = get_direction(nextAim);
				}
				else
				{
					bus.aim = -1;
					bus.direction = -1;
				}
				bus.isStop = 0;
			}
			else
			{
				bus.isStop = 1;
				return;
			}
		}
		if(bus.aim == -1)
		{
			bus.aim = getNextReq();
			if(bus.aim != -1)
			{
				bus.direction = get_direction(bus.aim);
				if(bus.direction == 1)
				{
					bus.position = (bus.position + 1) % (TOTAL_STATION * DISTANCE);
				}
				else
				{
					bus.position --;
				}
				if(bus.position < 0)
				{
					bus.position += TOTAL_STATION * DISTANCE;
				}
			}
		}
	}
	return;
}
