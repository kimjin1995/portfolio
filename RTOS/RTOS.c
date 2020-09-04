#include "includes.h"
#include<Windows.h>
#include <time.h>
#include "stdio.h"
#include "stdlib.h"
#pragma warning(disable:4047)
#pragma warning(disable:4133)


#define TASK_STK_SIZE 512
#define N_MSG      100
OS_STK TaskStk[TASK_STK_SIZE]; // Task Stack Memory 지정

OS_EVENT *sem;
OS_EVENT *msg_q;
OS_EVENT *dust_q;
OS_EVENT *temp_q;
INT8U state, weather;
void *msg_array[N_MSG];
void *dust_array[N_MSG];
void *temp_array[N_MSG];
FP32 Temperature = 22; // 18 ~ 27도사이로 조정 // 25에 에어컨 on, 18에 히터 on
FP32 Dust = 20; // 50이 마지노선, 안좋으면 10으로 만듦, 40에 on

typedef struct {
	INT8U posX;
	INT8U posY;
	INT8U index;
}Car;

void TaskStartDispInit();
void TaskChangeWeather(void *data);
void TaskChangeTime(void *data);
void TaskDrawWiper(void*data);
void TaskMakeRain(void*data);
void TaskChangeTemperature(void *data);
void TaskChangeDust(void*data);
void TaskAirCondition(void*data);
void TaskAirCleaner(void *data);
void TaskDrawCar(void *data);
void TaskGage(void *data);
void TaskAddCar(void *data);
void TaskCheckTimeandWeather(void *data);

Car car[10] = { 0 };

int  main(void)
{

	INT8U i;
	OSInit();
	TaskStartDispInit();
	msg_q = OSQCreate(msg_array, (INT16U)N_MSG);
	dust_q = OSQCreate(dust_array, (INT16U)N_MSG);
	temp_q = OSQCreate(temp_array, (INT16U)N_MSG);
	sem = OSSemCreate(10);

	OSTaskCreate(TaskChangeWeather, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], 3); // 날씨 변경 Task
	OSTaskCreate(TaskChangeTime, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], 4); // 시간 변경 Task
	OSTaskCreate(TaskCheckTimeandWeather, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], 5); // Message passing을 통한 시간,날씨를 알려주는 Task
	OSTaskCreate(TaskAddCar, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], 6);// 차를 생성하는 Task, 총 10개의 세마포어
	OSTaskCreate(TaskDrawCar, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], 7); // 차를 그려주는 Task, 차의 이동 및 전조등 On/OFF 제어


	OSTaskCreate(TaskChangeTemperature, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], 8); // 현재 온도를 변경해주는 Task
	OSTaskCreate(TaskChangeDust, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], 9); // 미세먼지의 농도를 변경해주는 Task
	OSTaskCreate(TaskAirCondition, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], 10); // 에어컨을 ON/OFF해주는 Task
	OSTaskCreate(TaskAirCleaner, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], 11);// 공기청정기를 ON/OFF해주는 Task

	OSTaskCreate(TaskGage, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], 12); // 현재 미세먼지 농도와 온도를 그려주는 Task
	for (int i = 0; i < 3; i++)
		OSTaskCreate(TaskMakeRain, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], (INT8U)(14 + i)); // 비를 그려주는 Task


	OSTaskCreate(TaskDrawWiper, (void *)NULL, &TaskStk[TASK_STK_SIZE - 1], 17); // 와이퍼의 움직임을 그려주는 Task



	OSStart();
	return 0;
}

// 낮밤, 날씨 순서 메시지
void TaskChangeWeather(void *data)
{
	INT8U err;
	INT8U A;
	INT8S s[10];

	for (;;)
	{
		sprintf(s, "%s", "Rain");
		err = OSQPost(msg_q, (void*)&s); 
		OSTimeDly(5);
		// 8초를 주기로 날씨가 바뀜.비가 오는 날씨이기 때문에 Rain Message passing
		
		sprintf(s, "%s", "Not");
		err = OSQPost(msg_q, (void*)&s);
		OSTimeDly(11);
		// 비가 오지 않을 때는 Not이라는 메시지를 보냄


	}
}
void TaskChangeTime(void *data)
{
	INT8U err;
	INT8S s[10];
	INT8U A;
	for (;;)
	{
		for (INT8U i = 0; i < 18; i++)
		{
			PC_DispStr(0, i, "                                                                    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
		}
		for (INT8U i = 0; i < 25; i++)
			PC_DispStr(70, i, "    l    ", DISP_FGND_BLACK + DISP_BGND_WHITE);

		sprintf(s, "%s", "Day");
		err = OSQPost(msg_q, (void*)&s);
		OSTimeDly(16);

		//16초를 주기로 낮밤이 변경됨. 낮과 밤의 배경을 그려주는 Task, 낮이면 낮이라는 Message를 전달함으로써 현재 날씨를 보여줌.

		for (INT8U i = 0; i < 18; i++)
			PC_DispStr(0, i, "                                                                    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
		for (INT8U i = 0; i < 25; i++)
			PC_DispStr(70, i, "    l    ", DISP_FGND_WHITE + DISP_BGND_BLACK);

		sprintf(s, "%s", "Night");
		err = OSQPost(msg_q, (void*)&s);
		OSTimeDly(16);
		// 16초의 주기로 밤이 됨. 배경은 검정색이 되며, 차들의 전조등이 켜질 수 있도록 밤이라는 message를 전달함.

		OSQFlush(msg_q);// 메시지 큐 삭제
	}
}

// 받아온 메시지를 읽어옴. 받아온 메시지에 현재의 상태가 쓰여있고, 메시지에 따라 현재의 상태를 변경할 수 있는 FSM을 구성함.
void TaskCheckTimeandWeather(void *data)
{
	INT8U err;
	INT8S *s = NULL;
	for (;;)
	{
		s = OSQPend(msg_q, 0, &err);
		if (strcmp(s, "Day") == 0)
			state = 0;
		else if (strcmp(s, "Night") == 0)
			state = 1;
		else if (strcmp(s, "Not") == 0)
			weather = 0;
		else if (strcmp(s, "Rain") == 0)
			weather = 1;
	}
}
// 차를 그리는 작업. 차의 이동 및 전조등 역시 포함하며, 차가 사라지면 세마포어를 반납할 수 있게 한다.
void TaskDrawCar(void *data)
{
	for (;;)
	{
		if (state == 0) // 현재가 낮이라면
		{
			for (INT8U x = 0; x < 10; x++) 
			{
				if (car[x].posX != 0) // 현재 차가 존재한다면
				{
					if (car[x].posX == 76) // 차가 오른쪽 차선인가??
					{
						if (car[x].posY != 24) // 차가 사라지는 곳인가?
							PC_DispStr(car[x].posX, car[x].posY + 1, "  ", DISP_FGND_WHITE + DISP_BGND_WHITE);

						if (car[x].posY == 0) // 차가 만약 끝부분이라면?? 마지막으로 차를 이동시키고 세마포어 반납, 차량 삭제
						{
							PC_DispStr(car[x].posX, car[x].posY--, "△", DISP_FGND_BLACK + DISP_BGND_WHITE);
							car[x].posX = 0;
							car[x].posY = 0;
							OSSemPost(sem);
						}
						else // 끝부분 이 아니라면 차량을 이동시킨다.
							PC_DispStr(car[x].posX, car[x].posY--, "△", DISP_FGND_BLACK + DISP_BGND_WHITE);

					}
					else // 차가 왼쪽 차선인가?
					{
						if (car[x].posY != 0) // 차가 사라지는 곳인가?
							PC_DispStr(car[x].posX, car[x].posY - 1, "  ", DISP_FGND_WHITE + DISP_BGND_WHITE);

						if (car[x].posY == 25) // 끝부분이라면 차량을 이동시키고 세마포어 반납, 차량 삭제
						{
							PC_DispStr(car[x].posX, car[x].posY - 1, "  ", DISP_FGND_WHITE + DISP_BGND_WHITE);
							car[x].posX = 0;
							car[x].posY = 0;
							OSSemPost(sem);
						}
						else // 아니라면 차량 이동,
							PC_DispStr(car[x].posX, car[x].posY++, "▽", DISP_FGND_BLACK + DISP_BGND_WHITE);
					}

				}
			}
		}
		else if (state == 1)//밤인가?
		{
			for (INT8U x = 0; x < 10; x++)
			{
				if (car[x].posX != 0)
				{
					if (car[x].posX == 76) //  왼쪽 차선인가?
					{
						if (car[x].posY != 24) //차가 이동하면서 생기는 잔상 처리
							PC_DispStr(car[x].posX, car[x].posY + 1, "  ", DISP_FGND_BLACK + DISP_BGND_BLACK);
						if (car[x].posY != 0) // 차의 전조등이 켜지는 것을 알리는 작업
							PC_DispStr(car[x].posX, car[x].posY - 1, "▼", DISP_FGND_YELLOW + DISP_BGND_BLACK);




						if (car[x].posY == 0) // 현재 차의 위치가 끝이라면?? 세마포어 반납, 차량 삭제
						{
							PC_DispStr(car[x].posX, car[x].posY--, "△", DISP_FGND_WHITE + DISP_BGND_BLACK);
							car[x].posX = 0;
							car[x].posY = 0;
							OSSemPost(sem);
						}
						else // 아니라면 차량 이동
							PC_DispStr(car[x].posX, car[x].posY--, "△", DISP_FGND_WHITE + DISP_BGND_BLACK);


					}
					else // 오른쪽 차선인가?
					{
						if (car[x].posY != 0) // 차의 잔상 처리
							PC_DispStr(car[x].posX, car[x].posY - 1, "  ", DISP_FGND_BLACK + DISP_BGND_BLACK);

						if (car[x].posY == 25) // 차가 끝부분인가? 세마포어 반납, 차량 삭제
						{
							PC_DispStr(car[x].posX, car[x].posY - 1, "  ", DISP_FGND_BLACK + DISP_BGND_BLACK);
							car[x].posX = 0;
							car[x].posY = 0;
							OSSemPost(sem);
						}
						else // 끝부분이 아닌가?
						{
							if (car[x].posY + 1 != 25) // 다음 차량의 위치가 끝부분인가? 아니라면 전조등 on
							{
								PC_DispStr(car[x].posX, car[x].posY + 1, "▲", DISP_FGND_YELLOW + DISP_BGND_BLACK);
							}
							PC_DispStr(car[x].posX, car[x].posY++, "▽", DISP_FGND_WHITE + DISP_BGND_BLACK);
						}

					}
				}
			}
		}


		OSTimeDly(1); // 1초의 delay
		if (state == 0)
		{
			PC_DispStr(76, 0, "  ", DISP_FGND_WHITE + DISP_BGND_WHITE);
		}
		else if (state == 1)
		{
			PC_DispStr(76, 0, "  ", DISP_FGND_BLACK + DISP_BGND_BLACK);
		}
	}
}
// 차량을 추가하는 작업, 세마포어를 활용함.
void TaskAddCar(void *data) 
{
	srand(time((unsigned int *)0) + (OSTCBCur->OSTCBPrio * 237 >> 4)); // random seed 생성
	INT32U i = 0;
	INT8U err, B;

	for (;;)
	{
		INT8U A = (INT8U)(rand() % 2); // 랜덤값 생성, 0 or 1
		B = (INT8U)(rand() % 5 + 2); // 랜덤값 생성, 0 or 1
		if (A == 0)
		{
			OSSemPend(sem, 0, &err); // 왼쪽 차선에 차량 생성, 세마포어 점유
			car[i % 10].posX = 71;
			car[i % 10].posY = 0;
			i++; 
		}
		else 
		{
			OSSemPend(sem, 0, &err); // 오른쪽차선에 차량 생성, 세마포어 점유
			car[i % 10].posX = 76;
			car[i % 10].posY = 24;
			i++;
		}
		OSTimeDly(B);

	}
}

//와이퍼를 그리는 작업
void TaskDrawWiper(void*data)
{
	INT8U W = 0;
	for (;;)
	{
		if (weather == 0) // 비가 오지 않는다면
		{
			if (state == 1) // 현재 낮이라면
			{
					PC_DispStr(30, 7, "    ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(27, 8, "       ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(24, 9, "          ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(21, 10, "             ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(18, 11, "                ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(15, 12, "                   ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(12, 13, "                      ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(9, 14, "                         ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(6, 15, "                            ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(3, 16, "                               ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(0, 17, "                                  ", DISP_FGND_BLACK + DISP_BGND_BLACK);

					PC_DispStr(64, 7, "    ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(61, 8, "       ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(58, 9, "          ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(55, 10, "             ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(52, 11, "                ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(49, 12, "                   ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(46, 13, "                      ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(43, 14, "                         ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(40, 15, "                            ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(37, 16, "                               ", DISP_FGND_BLACK + DISP_BGND_BLACK);
					PC_DispStr(34, 17, "                                  ", DISP_FGND_BLACK + DISP_BGND_BLACK);

			}
			else if (state == 0) // 현재가 밤이라면
			{
				PC_DispStr(30, 7, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(27, 8, "       ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(24, 9, "          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(21, 10, "             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(18, 11, "                ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(15, 12, "                   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(12, 13, "                      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(9, 14, "                         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(6, 15, "                            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(3, 16, "                               ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(0, 17, "                                  ", DISP_FGND_BLACK + DISP_BGND_WHITE);

				PC_DispStr(64, 7, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(61, 8, "       ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(58, 9, "          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(55, 10, "             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(52, 11, "                ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(49, 12, "                   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(46, 13, "                      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(43, 14, "                         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(40, 15, "                            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(37, 16, "                               ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(34, 17, "                                  ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			}
			OSTimeDly(1);
		}
		else if (weather == 1) // 비가 오는 날씨라면?
		{
		if (state == 1) // 현재가 밤이라면?
			{
			
			PC_DispStr(30, 7, "    ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(27, 8, "       ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(24, 9, "          ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(21, 10, "             ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(18, 11, "                ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(15, 12, "                   ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(12, 13, "                      ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(9, 14, "                         ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(6, 15, "                            ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(3, 16, "                               ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(0, 17, "                                  ", DISP_FGND_BLACK + DISP_BGND_BLACK);

			PC_DispStr(64, 7, "    ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(61, 8, "       ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(58, 9, "          ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(55, 10, "             ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(52, 11, "                ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(49, 12, "                   ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(46, 13, "                      ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(43, 14, "                         ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(40, 15, "                            ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(37, 16, "                               ", DISP_FGND_BLACK + DISP_BGND_BLACK);
			PC_DispStr(34, 17, "                                  ", DISP_FGND_BLACK + DISP_BGND_BLACK);

			}
		else if (state == 0) // 현재가 낮이라면?
		{
			PC_DispStr(30, 7, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(27, 8, "       ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(24, 9, "          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(21, 10, "             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(18, 11, "                ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(15, 12, "                   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(12, 13, "                      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(9, 14, "                         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(6, 15, "                            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(3, 16, "                               ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(0, 17, "                                  ", DISP_FGND_BLACK + DISP_BGND_WHITE);

			PC_DispStr(64, 7, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(61, 8, "       ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(58, 9, "          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(55, 10, "             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(52, 11, "                ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(49, 12, "                   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(46, 13, "                      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(43, 14, "                         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(40, 15, "                            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(37, 16, "                               ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			PC_DispStr(34, 17, "                                  ", DISP_FGND_BLACK + DISP_BGND_WHITE);

		}
			OSTimeDly(1);
			// 1초의 delay
			if (W % 7 == 0) // 와이퍼를 그리는 state
			{
				if (state == 0) // 만약 낮이라면? 비를 그린 뒤 와이퍼를 그림, 삭제되는 비를 그리기 위함
				{
					PC_DispStr(51, 0, "■■             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(48, 1, "■■                ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(45, 2, "■■                   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(42, 3, "■■                      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(39, 4, "■■                         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(36, 5, "■■                            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(33, 6, "■■                               ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(30, 7, "■■                                  ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(27, 8, "■■                                     ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(24, 9, "■■                                        ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(21, 10, "■■                                           ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(18, 11, "■■                                              ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(15, 12, "■■                                                 ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(12, 13, "■■                                                    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(9, 14, "■■                                                       ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(6, 15, "■■                                                          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(3, 16, "■■                                                             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 17, "■■                                                                ", DISP_FGND_BLACK + DISP_BGND_WHITE);

					PC_DispStr(64, 7, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(61, 8, "■■   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(58, 9, "■■      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(55, 10, "■■         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(52, 11, "■■            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(49, 12, "■■               ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(46, 13, "■■                  ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(43, 14, "■■                     ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(40, 15, "■■                        ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(37, 16, "■■                           ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 17, "■■                              ", DISP_FGND_BLACK + DISP_BGND_WHITE);



				}
				else if (state == 1) // 밤일 때. 비를 그리고 와이퍼를 그림. 사라지는 비를 나타내기 위함.
				{
					PC_DispStr(51, 0, "■■             ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(48, 1, "■■                ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(45, 2, "■■                   ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(42, 3, "■■                      ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(39, 4, "■■                         ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(36, 5, "■■                            ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(33, 6, "■■                               ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(30, 7, "■■                                  ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(27, 8, "■■                                     ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(24, 9, "■■                                        ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(21, 10, "■■                                           ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(18, 11, "■■                                              ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(15, 12, "■■                                                 ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(12, 13, "■■                                                    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(9, 14, "■■                                                       ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(6, 15, "■■                                                          ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(3, 16, "■■                                                             ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 17, "■■                                                                ", DISP_FGND_WHITE + DISP_BGND_BLACK);

					PC_DispStr(64, 7, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(61, 8, "■■   ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(58, 9, "■■      ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(55, 10, "■■         ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(52, 11, "■■            ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(49, 12, "■■               ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(46, 13, "■■                  ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(43, 14, "■■                     ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(40, 15, "■■                        ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(37, 16, "■■                           ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 17, "■■                              ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				}
				W++;
				OSTimeDly(1); // 1초의 delay
			}

			if (W % 7 == 1)
			{
				if (state == 0) // 낮일때, 와이퍼가 지나가는 영역을 그림.와이퍼가 지나가면서 비가 사라지도록 함. 와이퍼는 총 2개이기 때문에 2개의 와이퍼를 그리게 됨, 낮 밤 2가지의 상태에 따른 모양이 존재.
				{

					PC_DispStr(34, 0, "■■                 ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(32, 1, "■■                ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(30, 2, "■■               ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(28, 3, "■■              ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(26, 4, "■■             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(24, 5, "■■            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(22, 6, "■■           ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(20, 7, "■■          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(18, 8, "■■         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(16, 9, "■■        ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(14, 10, "■■       ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(12, 11, "■■      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(10, 12, "■■     ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(8, 13, "■■    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(6, 14, "■■   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(4, 15, "■■  ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(2, 16, "■■ ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);

					PC_DispStr(64, 2, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(62, 3, "■■  ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(60, 4, "■■    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(58, 5, "■■      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(56, 6, "■■        ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(54, 7, "■■          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(52, 8, "■■         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(50, 9, "■■        ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(48, 10, "■■       ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(46, 11, "■■      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(44, 12, "■■     ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(42, 13, "■■    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(40, 14, "■■   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(38, 15, "■■  ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(36, 16, "■■ ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);



				}
				else if (state == 1) // 밤일때.  와이퍼가 지나가는 영역을 그림.와이퍼가 지나가면서 비가 사라지도록 함. 와이퍼는 총 2개이기 때문에 2개의 와이퍼를 그리게 됨, 낮 밤 2가지의 상태에 따른 모양이 존재.
				{
					PC_DispStr(34, 0, "■■                 ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(32, 1, "■■                ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(30, 2, "■■               ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(28, 3, "■■              ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(26, 4, "■■             ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(24, 5, "■■            ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(22, 6, "■■           ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(20, 7, "■■          ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(18, 8, "■■         ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(16, 9, "■■        ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(14, 10, "■■       ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(12, 11, "■■      ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(10, 12, "■■     ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(8, 13, "■■    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(6, 14, "■■   ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(4, 15, "■■  ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(2, 16, "■■ ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);

					PC_DispStr(64, 2, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(62, 3, "■■  ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(60, 4, "■■    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(58, 5, "■■      ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(56, 6, "■■        ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(54, 7, "■■          ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(52, 8, "■■         ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(50, 9, "■■        ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(48, 10, "■■       ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(46, 11, "■■      ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(44, 12, "■■     ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(42, 13, "■■    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(40, 14, "■■   ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(38, 15, "■■  ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(36, 16, "■■ ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);

				}

				W++;
				OSTimeDly(1);
			}

			if (W % 7 == 2) // 와이퍼의 2번째 그림
			{

				if (state == 0) // 낮일때
				{
					PC_DispStr(17, 0, "■■                 ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(16, 1, "■■                ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(15, 2, "■■               ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(14, 3, "■■              ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(13, 4, "■■             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(12, 5, "■■            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(11, 6, "■■           ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(10, 7, "■■          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(9, 8, "■■         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(8, 9, "■■        ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(7, 10, "■■       ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(6, 11, "■■      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(5, 12, "■■     ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(4, 13, "■■    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(3, 14, "■■   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(2, 15, "■■  ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(1, 16, "■■ ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);

					PC_DispStr(51, 0, "■■             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(50, 1, "■■              ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(49, 2, "■■               ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(48, 3, "■■              ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(47, 4, "■■             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(46, 5, "■■            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(45, 6, "■■           ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(44, 7, "■■          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(43, 8, "■■         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(42, 9, "■■        ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(41, 10, "■■       ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(40, 11, "■■      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(39, 12, "■■     ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(38, 13, "■■    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(37, 14, "■■   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(36, 15, "■■  ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(35, 16, "■■ ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);

				}
				else if (state == 1) // 밤일때
				{
					PC_DispStr(17, 0, "■■                 ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(16, 1, "■■                ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(15, 2, "■■               ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(14, 3, "■■              ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(13, 4, "■■             ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(12, 5, "■■            ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(11, 6, "■■           ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(10, 7, "■■          ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(9, 8, "■■         ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(8, 9, "■■        ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(7, 10, "■■       ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(6, 11, "■■      ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(5, 12, "■■     ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(4, 13, "■■    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(3, 14, "■■   ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(2, 15, "■■  ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(1, 16, "■■ ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);

					PC_DispStr(51, 0, "■■             ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(50, 1, "■■              ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(49, 2, "■■               ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(48, 3, "■■              ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(47, 4, "■■             ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(46, 5, "■■            ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(45, 6, "■■           ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(44, 7, "■■          ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(43, 8, "■■         ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(42, 9, "■■        ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(41, 10, "■■       ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(40, 11, "■■      ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(39, 12, "■■     ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(38, 13, "■■    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(37, 14, "■■   ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(36, 15, "■■  ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(35, 16, "■■ ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
				}
				W++;
				OSTimeDly(1);
			}

			if (W % 7 == 3) // 3번째 움직임
			{
				if (state == 0)
				{

					PC_DispStr(0, 0, "■■                 ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 1, "■■                ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 2, "■■               ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 3, "■■              ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 4, "■■             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 5, "■■            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 6, "■■           ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 7, "■■          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 8, "■■         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 9, "■■        ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 10, "■■       ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 11, "■■      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 12, "■■     ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 13, "■■    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 14, "■■   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 15, "■■  ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 16, "■■ ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);

					PC_DispStr(34, 0, "■■                 ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 1, "■■                ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 2, "■■               ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 3, "■■              ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 4, "■■             ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 5, "■■            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 6, "■■           ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 7, "■■          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 8, "■■         ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 9, "■■        ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 10, "■■       ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 11, "■■      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 12, "■■     ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 13, "■■    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 14, "■■   ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 15, "■■  ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 16, "■■ ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
				}
				else if (state == 1) // 밤일 때
				{

					PC_DispStr(0, 0, "■■                 ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 1, "■■                ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 2, "■■               ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 3, "■■              ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 4, "■■             ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 5, "■■            ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 6, "■■           ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 7, "■■          ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 8, "■■         ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 9, "■■        ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 10, "■■       ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 11, "■■      ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 12, "■■     ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 13, "■■    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 14, "■■   ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 15, "■■  ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 16, "■■ ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);

					PC_DispStr(34, 0, "■■                 ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 1, "■■                ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 2, "■■               ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 3, "■■              ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 4, "■■             ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 5, "■■            ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 6, "■■           ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 7, "■■          ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 8, "■■         ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 9, "■■        ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 10, "■■       ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 11, "■■      ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 12, "■■     ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 13, "■■    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 14, "■■   ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 15, "■■  ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 16, "■■ ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
				}
				W++;
				OSTimeDly(1);
			}

			if (W % 7 == 4) // 4번째 움직임
			{
				if (state == 0) // 낮일때
				{
					PC_DispStr(0, 0, "                 ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 1, "                ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 2, "               ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 3, "              ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 4, "             ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 5, "            ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 6, "           ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 7, "          ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 8, "         ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 9, "        ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 10, "       ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 11, "      ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 12, "     ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 13, "    ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 14, "   ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 15, "  ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 16, " ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);

					PC_DispStr(34, 0, "                 ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 1, "                ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 2, "               ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 3, "              ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 4, "             ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 5, "            ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 6, "           ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 7, "          ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 8, "         ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 9, "        ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 10, "       ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 11, "      ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 12, "     ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 13, "    ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 14, "   ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 15, "  ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 16, " ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
				}
				else if (state == 1) // 밤일때
				{
					PC_DispStr(0, 0, "                 ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 1, "                ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 2, "               ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 3, "              ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 4, "             ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 5, "            ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 6, "           ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 7, "          ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 8, "         ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 9, "        ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 10, "       ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 11, "      ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 12, "     ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 13, "    ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 14, "   ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 15, "  ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 16, " ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);

					PC_DispStr(34, 0, "                 ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 1, "                ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 2, "               ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 3, "              ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 4, "             ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 5, "            ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 6, "           ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 7, "          ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 8, "         ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 9, "        ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 10, "       ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 11, "      ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 12, "     ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 13, "    ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 14, "   ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 15, "  ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 16, " ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
				}
				W++;
				OSTimeDly(1);
			}

			if (W % 7 == 5) // 5번째 움직임
			{
				if (state == 0) // 낮일때
				{
					PC_DispStr(17, 0, "                 ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(16, 1, "                ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(15, 2, "               ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(14, 3, "              ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(13, 4, "             ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(12, 5, "            ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(11, 6, "           ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(10, 7, "          ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(9, 8, "         ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(8, 9, "        ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(7, 10, "       ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(6, 11, "      ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(5, 12, "     ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(4, 13, "    ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(3, 14, "   ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(2, 15, "  ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(1, 16, " ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);

					PC_DispStr(51, 0 , "                 ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(50, 1, "                  ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(49, 2 , "               ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(48, 3 , "              ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(47, 4 , "             ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(46, 5 , "            ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(45, 6 , "           ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(44, 7 , "          ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(43, 8 , "         ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(42, 9 , "        ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(41, 10, "       ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(40, 11, "      ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(39, 12, "     ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(38, 13, "    ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(37, 14, "   ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(36, 15, "  ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(35, 16, " ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);

				}
				else if (state == 1) // 밤일때
				{
					PC_DispStr(17, 0, "                 ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(16, 1, "                ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(15, 2, "               ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(14, 3, "              ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(13, 4, "             ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(12, 5, "            ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(11, 6, "           ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(10, 7, "          ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(9, 8, "         ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(8, 9, "        ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(7, 10, "       ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(6, 11, "      ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(5, 12, "     ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(4, 13, "    ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(3, 14, "   ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(2, 15, "  ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(1, 16, " ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);

					PC_DispStr(51, 0, "                 ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(50, 1, "                  ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(49, 2, "               ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(48, 3, "              ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(47, 4, "             ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(46, 5, "            ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(45, 6, "           ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(44, 7, "          ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(43, 8, "         ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(42, 9, "        ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(41, 10, "       ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(40, 11, "      ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(39, 12, "     ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(38, 13, "    ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(37, 14, "   ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(36, 15, "  ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(35, 16, " ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
				}

				W++;
				OSTimeDly(1);
			}

			if (W % 7 == 6) // 6번째 움직임
			{
				if (state == 0)
				{
					PC_DispStr(34, 0, "                 ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(32, 1, "                ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(30, 2, "               ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(28, 3, "              ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(26, 4, "             ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(24, 5, "            ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(22, 6, "           ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(20, 7, "          ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(18, 8, "         ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(16, 9, "        ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(14, 10, "       ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(12, 11, "      ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(10, 12, "     ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(8, 13, "    ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(6, 14, "   ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(4, 15, "  ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(2, 16, " ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(0, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);

					PC_DispStr(64, 2, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(62, 3, "      ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(60, 4, "        ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(58, 5, "          ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(56, 6, "            ", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(54, 7, "          ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(52, 8, "         ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(50, 9, "        ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(48, 10, "       ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(46, 11, "      ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(44, 12, "     ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(42, 13, "    ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(40, 14, "   ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(38, 15, "  ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(36, 16, " ■■", DISP_FGND_BLACK + DISP_BGND_WHITE);
					PC_DispStr(34, 17, "■■", DISP_FGND_BLACK + DISP_BGND_WHITE);

				}
				else if (state == 1)
				{
					PC_DispStr(34, 0, "                 ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(32, 1, "                ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(30, 2, "               ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(28, 3, "              ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(26, 4, "             ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(24, 5, "            ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(22, 6, "           ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(20, 7, "          ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(18, 8, "         ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(16, 9, "        ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(14, 10, "       ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(12, 11, "      ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(10, 12, "     ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(8, 13, "    ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(6, 14, "   ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(4, 15, "  ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(2, 16, " ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(0, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);

					PC_DispStr(64, 2, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(62, 3, "      ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(60, 4, "        ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(58, 5, "          ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(56, 6, "            ", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(54, 7, "          ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(52, 8, "         ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(50, 9, "        ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(48, 10, "       ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(46, 11, "      ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(44, 12, "     ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(42, 13, "    ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(40, 14, "   ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(38, 15, "  ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(36, 16, " ■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
					PC_DispStr(34, 17, "■■", DISP_FGND_WHITE + DISP_BGND_BLACK);
				}
				W++;
				OSTimeDly(1);
			}


			if (state == 0)
			{
				PC_DispStr(51, 0, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(48, 1, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(45, 2, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(42, 3, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(39, 4, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(36, 5, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(33, 6, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(30, 7, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(27, 8, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(24, 9, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(21, 10, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(18, 11, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(15, 12, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(12, 13, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(9, 14, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(6, 15, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(3, 16, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(0, 17, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);

				PC_DispStr(64, 7, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(61, 8, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(58, 9, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(55, 10, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(52, 11, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(49, 12, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(46, 13, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(43, 14, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(40, 15, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(37, 16, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
				PC_DispStr(34, 17, "    ", DISP_FGND_BLACK + DISP_BGND_WHITE);
			}
			else
			{
				PC_DispStr(51, 0, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(48, 1, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(45, 2, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(42, 3, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(39, 4, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(36, 5, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(33, 6, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(30, 7, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(27, 8, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(24, 9, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(21, 10, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(18, 11, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(15, 12, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(12, 13, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(9, 14, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(6, 15, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(3, 16, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(0, 17, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);

				PC_DispStr(64, 7, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(61, 8, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(58, 9, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(55, 10, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(52, 11, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(49, 12, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(46, 13, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(43, 14, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(40, 15, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(37, 16, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(34, 17, "    ", DISP_FGND_WHITE + DISP_BGND_BLACK);
			}
		}
	}
}

void TaskMakeRain(void*data)// 비를 그리는 task
{

	INT8U delay;
	INT32U i = 0;
	INT8U POSX, POSY, POSX1, POSY1, POSX2,POSY2,POSX3,POSY3;
	srand(time((unsigned int)NULL));
	INT8U err;
	for (;;)
	{
		// 비가 오는 날씨라면 정해진 영역 내에 비를 그린다. 총 2개의 비를 그리는 Task이다.
		if (weather == 1)
		{
			POSX = rand() % 67;
			POSY = rand() % 17;

			POSX1 = rand() % 67;
			POSY1 = rand() % 17;

			POSX2 = rand() % 67;
			POSY2 = rand() % 17;

			POSX3 = rand() % 67;
			POSY3 = rand() % 17;


			if(state == 0)
				PC_DispStr(POSX, POSY, "●", DISP_FGND_CYAN + DISP_BGND_WHITE);
			else
				PC_DispStr(POSX, POSY, "●", DISP_FGND_CYAN + DISP_BGND_BLACK);

			if (state == 0)
				PC_DispStr(POSX1, POSY1, "●", DISP_FGND_CYAN + DISP_BGND_WHITE);
			else
				PC_DispStr(POSX1, POSY1, "●", DISP_FGND_CYAN + DISP_BGND_BLACK);

			if (state == 0)
				PC_DispStr(POSX2, POSY2, "●", DISP_FGND_CYAN + DISP_BGND_WHITE);
			else
				PC_DispStr(POSX2, POSY2, "●", DISP_FGND_CYAN + DISP_BGND_BLACK);

			if (state == 0)
				PC_DispStr(POSX3, POSY3, "●", DISP_FGND_CYAN + DISP_BGND_WHITE);
			else
				PC_DispStr(POSX3, POSY3, "●", DISP_FGND_CYAN + DISP_BGND_BLACK);

		}
		OSTimeDly(1);
	}


}
//온도를 변경해주는 Task가 된다.
void TaskChangeTemperature(void *data)
{
	INT8S s[10];
	INT8U err;
	FP32 A;
	srand(time((unsigned int *)0) + (OSTCBCur->OSTCBPrio * 237 >> 4)); // 랜덤 seed를 통해 랜덤값을 뽑아낸다.
	for (;;)
	{
		A = (INT8U)(rand() % 10); 
		Temperature -= A / 10 * state;
		// 정해진 랜덤값을 뽑아내어, 현재가 밤이라면 온도가 낮아지도록 한다.
		if (state == 0)// 현재가 낮이면 햇빛때문에 온도가 올라감, 현재의 온도를 +한다.
			Temperature += A / 10;
		A = (INT8U)(rand() % 10); // 랜덤값 생성
		Temperature -= A / 10 * weather; // 비가 오면 온도가 내려가야한다.
		if (weather == 0) // 비가 오지 않으면 온도가 올라간다.
			Temperature += A / 10;
		//현재의 온도를 소수점 아래 한자리까지 추출
		sprintf(s, "%.1lf", Temperature);
		// 메시지 패싱방법을 통해 현재의 온도를 보냄.
		err = OSQPost(temp_q, (void*)&s);
		// 현재의 온도 display로 보여줌
		PC_DispStr(19, 21, s, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

	   
		OSTimeDly(1);
	}
}
//현재 미세먼지의 농도를 조절하는 task
void TaskChangeDust(void*data)
{
	INT8U err;
	INT8S s[10];
	FP32 A;
	srand(time((unsigned int *)0) + (OSTCBCur->OSTCBPrio * 237 >> 4)); // 랜덤 시드값 형성
	for (;;)
	{
		A = (INT8U)(rand() % 50); // 랜덤값 생성
		Dust -= A / 20 * state; // 먼지가 감소하는 것을 나타냄
		if (state == 0) // 현재가 낮이다? 미세먼지가 올라감.
			Dust += A / 10;
		A = (INT8U)(rand() % 50); // 랜덤값 생성
		Dust -= A / 20 * weather; // 현재의 날씨에 따라 미세먼지가 달라짐. 비가 오면 미세먼지가 내려가게 된다.
		if (weather == 0)// 비가 오지 않으면 미세먼지가 올라감.
			Dust += A / 10;

		if (Dust < 0) // 먼지는 0 아래로 떨어지지 않는다.
			Dust = 0;
		sprintf(s, "%.2lf", Dust); // 메시지 패싱 방식으로 현재 미세먼지의 농도를 보냄.
		err = OSQPost(dust_q, (void*)&s);
		PC_DispStr(19, 23, s, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
  
		OSTimeDly(1);
	}
}

//에어컨을 ON/OFF하는 Task
void TaskAirCondition(void*data)
{
	INT8S TT = 0;
	INT8S *s = NULL;
	INT8U err;
	INT8S w[10];
	FP32 A;
	for (;;)
	{
		// 메시지를 통해 현재의 온도를 받음
		s = OSQPend(temp_q, 0, &err);
		// 현재의 온도를 소수점으로 바꿈
		A = atof(s);


		// 현재 온도가 적정하기 때문에 에어컨 또는 히터가 켜지지 않은 상태다? 회색으로 보여줌
		if ((TT == -1 && Temperature > 22) || (TT == 1 && Temperature < 22))
		{
			PC_DispStr(34, 21, "●", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			TT = 0;
		}

		if (A >= 25) // 현재 너무 더워서 에어컨이 필요하다면 회색으로 보여줌
		{
			PC_DispStr(34, 21, "●", DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY);
			TT = 1;
		}
		if (A <= 20) // 온도가 너무 낮다면 히터를 켜며, 빨간색으로 보여줌.
		{
			PC_DispStr(34, 21, "●", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
			TT = -1;
		}
		if (TT == 1) // 히터가 켜지면 -1, 에어컨이 켜지면 +1
			Temperature -= 1;
		if (TT == -1)
			Temperature += 1;


		OSTimeDly(1);
	}
}
// 공기청정기를 관리하는 task
void TaskAirCleaner(void *data)
{
	INT8S *s = NULL;
	INT8U err;
	INT8S w[10];
	FP32 A;
	INT8U C = 0;
	for (;;)
	{

		//메시지를 받아와 현재 미세먼지의 농도를 측정, 이를 소수점으로 받아옴.
		s = OSQPend(dust_q, 0, &err);
		A = atof(s);
		sprintf(w, "%.2lf", A);


		if (A > 40) // 40을 넘어가면 공기청정기 가동, 초록색으로 보여준다.
		{
			PC_DispStr(34, 23, "●", DISP_FGND_GREEN + DISP_BGND_LIGHT_GRAY);
			C = 1;
		}

		if (C == 1 && Dust > 10) // 공기청정기가 켜졌을때, 적정량인 10까지 공기청정기를 가동한다.
		{
			Dust -= 5;
		}
		else if (C == 1 && Dust < 10) // 적절하게 미세먼지가 조절됐을 경우 공기청정기를 끈다.
		{
			C = 0;
			PC_DispStr(34, 23, "●", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
		}

		OSTimeDly(1);

	}
}
// 현재 미세먼지 및 온도의 정도를 보여준다.
void TaskGage(void *data)
{
	for (;;)
	{
		FP32 A = Temperature;
		FP32 B = Dust;

		if (A <= 17)// 현재 온도가 17도 이하라면??
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 0) // 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(40, 21, "■", DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 21, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}

		else if (A > 17 && A <= 18) // 온도가 17~18사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 1)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(43, 21, "■", DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 21, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (A > 18 && A <= 19) // 온도가 18~19 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 2)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(46, 21, "■", DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 21, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (A > 19 && A <= 20) // 온도가 19~20 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 3)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(49, 21, "■", DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 21, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (A > 20 && A <= 21) //온도가 20~21 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 4)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(52, 21, "■", DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 21, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (A > 21 && A <= 22) // 온도가 21~22 사이라면??
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 5)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(55, 21, "■", DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 21, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (A > 22 && A <= 23) // 온도가 22~23 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 6)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(58, 21, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 21, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (A > 23 && A <= 24) // 온도가 23~24 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 7)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(61, 21, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 21, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (A > 24) // 온도가 24도 이상이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 8)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(64, 21, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 21, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}


		if (Dust <= 5) // 미세먼지가 5보다 낮다면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 0)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(40, 23, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 23, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (Dust > 5 && Dust <= 10) // 미세먼지가 5~10 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 1)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(43, 23, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 23, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (Dust > 10 && Dust <= 15) // 미세먼지가 10~15 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 2)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(46, 23, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 23, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (Dust > 15 && Dust <= 20) // 미세먼지가 15~20 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 3)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(49, 23, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 23, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (Dust > 20 && Dust <= 25) // 미세먼지가 20~25 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 4)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(52, 23, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 23, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (Dust > 25 && Dust <= 30) // 미세먼지가 25~30 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 5)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(55, 23, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 23, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (Dust > 30 && Dust <= 35) // 미세먼지가 30~35 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 6)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(58, 23, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 23, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (Dust > 35 && Dust <= 40) // 미세먼지가 35~40 사이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 7)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(61, 23, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 23, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
		else if (Dust >= 40) // 미세먼지가 40 이상이라면?
			for (INT8U i = 0; i < 9; i++)
			{
				if (i == 8)// 현재 온도에 맞는 위치에 표시를 해둔다.
					PC_DispStr(64, 23, "■", DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
				else
					PC_DispStr(40 + 3 * i, 23, "  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}



		OSTimeDly(1);

	}
}
// 시작화면 구성
void  TaskStartDispInit()
{
	INT8S q[5];
	PC_DispStr(0, 0, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 1, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 2, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 3, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 4, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 5, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 6, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 7, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 8, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 9, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 10, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 11, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 12, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 13, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 14, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 15, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 16, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 17, "                                                                    ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 18, "####################################################################■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 19, "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 20, "--------------- --------------- ------ ---------------------------- ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 21, "| Temperature | |         ℃  | | ● | |  |  |  |  |  |  |  |  |  | ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 22, "--------------- --------------- ------ ---------------------------- ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 23, "|  Fine Dust  | |       ㎍/m³| | ● | |  |  |  |  |  |  |  |  |  | ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 24, "--------------- --------------- ------ ---------------------------- ■    l    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

	//PC_DispStr(37, 21, "■", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	//PC_DispStr(40, 21, "■", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	//PC_DispStr(70, 0, "    l    ", DISP_FGND_GRAY + DISP_BGND_BLACK);
	//PC_DispStr(0, 0, "    l    ", DISP_FGND_BLACK + DISP_BGND_WHITE);

}
