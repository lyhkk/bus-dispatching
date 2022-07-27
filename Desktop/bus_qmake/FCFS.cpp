/* dispatches the bus with FCFS(First Come First Serve) Strategy
@author: Jiuhe Chen
@date: 4 July 2022
*/
#include <mainwindow.h>

//传入车位置和目标站点，决定此时执行请求的方向，0为逆时针，1为顺时针
int bus::get_direction(int aim)
{
	int aim_pos = aim * w->DISTANCE;
	if(position > aim_pos)
	{
		if((w->DISTANCE * w->TOTAL_STATION) + aim_pos - position > position - aim_pos) //逆时针更近
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
		if((w->DISTANCE * w->TOTAL_STATION) + position - aim_pos < aim_pos - position) //逆时针更近
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
int bus::getNextReq()
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

void bus::FCFS()
{
	if(request->next == NULL || request == NULL) return; //无请求，停止在某一站点
	else
	{
		if(position % w->DISTANCE == 0 && aim == position / w->DISTANCE) //到站
		{
			int station = position / w->DISTANCE;
			if(isStop == 1)
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
							target[station] = 0;
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
							target[station] = 0;
							break;
					}
					del(request, request->next);
				}
				int nextAim = getNextReq();
				if(nextAim != -1)
				{
					aim = nextAim;
					direction = get_direction(nextAim);
				}
				else
				{
					aim = -1;
					direction = -1;
				}
				isStop = 0;
			}
			else
			{
				isStop = 1;
				return;
			}
		}
		if(aim == -1)
		{
			aim = getNextReq();
			if(aim != -1)
			{
				direction = get_direction(aim);
				if(direction == 1)
				{
					position = (position + 1) % (w->TOTAL_STATION * w->DISTANCE);
				}
				else
				{
					position --;
				}
				if(position < 0)
				{
					position += w->TOTAL_STATION * w->DISTANCE;
				}
			}
		}
	}
	return;
}
