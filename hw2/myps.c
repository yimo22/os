#include <stdio.h>
#include "header.h"

#define COLUMN_CNT 12 // 출력할 Col Max value

// 출력시 사용할 Value
enum IDXS{
  USER_IDX=0,  // 0
  PID_IDX,     // 1
  CPU_IDX,     // 2
  MEM_IDX,     // 3
  VSZ_IDX,     // 4
  RSS_IDX,     // 5
  TTY_IDX,     // 6
  STAT_IDX,    // 7
  START_IDX,   // 8
  TIME_IDX,    // 9 
  CMD_IDX,     // 10
  COMMAND_IDX, // 11
};

//  완성한 myProc의 포인터 저장 배열
//  PROCESS_MAX : 4096 
myProc procList[PROCESS_MAX]; 

// 현재까지 완성한 myProc 의 갯수 저장
int procCnt = 0; 

unsigned long uptime;         // os 부팅 후 지난 시간
unsigned long beforeUptime;   // 이전 실행 시의 os 부팅 후 지난 시각
unsigned long memTotal;       // 전체 물리 메모리 크기 저장
unsigned int hertz;           // os의 hertz값을 저장 (초당 Context Switching 횟수 저장)

time_t now;
time_t before;

pid_t myPid;            // 자기 자신의 pid 저장
uid_t myUid;            // 자기 자신의 uid 저장
char myPath[PATH_LEN];  // 자기 자신의 path 저장
char myTTY[TTY_LEN];    // 자기 자신의 tty 저장

// Option 여부에 따른 flag
// BSD SYSTEM 을 기준으로, Option은 a | u | x 만 구현함.
bool Ops_a = false;
bool Ops_u = false;
bool Ops_x = false;
void UpdateColumnWidth(int * colWidth){
  // BUFFER_SIZE : 1024
  char buff[BUFFER_SIZE];

  // USER 의 최대 길이 저장
  // USER 를 탐색해서 최대길이를 갖는 USER를 찾아 그 길이를 저장한다.
  // 출력 비율을 맞추기 위함
  if(Ops_u){
    for(int i=0;i<procCnt;i++){
      if(colWidth[USER_IDX] < strlen(procList[i].user)){
        // 기존 길이(4) 를 초과하는 경우 이를 갱신
        colWidth[USER_IDX] = strlen(procList[i].user);
      }
    }
  } else{
    colWidth[USER_IDX] = -1;
  }

  // PID 의 최대길이 수정
  // PID 를 탐색해서 최대길이를 갖는 PID를 찾아 그 길이로 colWidth[PID_IDX]를 UPDATE 한다.
  // 출력 비율을 맞추기 위함
  for(int i=0;i<procCnt;i++){
    // unsigned long 값을 str으로 가져옴
    sprintf(buff,"%lu",procList[i].pid);

    if(colWidth[PID_IDX] < strlen(buff)){
      colWidth[PID_IDX] = strlen(buff);
    }
  }

  // CPU 최대 길이 저장
  // CPU 길이를 UPDATE
  // 출력 비율을 맞추기 위함
  if(Ops_a){
    for(int i = 0; i < procCnt; i++){			
      sprintf(buff, "%3.1Lf", procList[i].cpu);
      if(colWidth[CPU_IDX] < strlen(buff))
        colWidth[CPU_IDX] = strlen(buff);
    }
  } else{
    colWidth[CPU_IDX] = -1;
  }

  // MEM 최대 길이 저장
  // MEM 길이를 UPDATE
  // 출력 비율을 맞추기 위함
  if(Ops_a){
    for(int i = 0; i < procCnt; i++){			
      sprintf(buff, "%3.1Lf", procList[i].mem);
      if(colWidth[MEM_IDX] < strlen(buff))
        colWidth[MEM_IDX] = strlen(buff);
    }
  } else{
    colWidth[MEM_IDX] = -1;
  }


  // VSZ 최대 길이 저장
  // VSZ 길이를 UPDATE
  // 출력 비율을 맞추기 위함
  if(Ops_a){
    for(int i = 0; i < procCnt; i++){			
      sprintf(buff, "%lu", procList[i].vsz);
      if(colWidth[VSZ_IDX] < strlen(buff))
        colWidth[VSZ_IDX] = strlen(buff);
    }
  } else{
    colWidth[VSZ_IDX] = -1;
  }


  // RSS 최대 길이 저장
  // RSS 길이를 UPDATE
  // 출력 비율을 맞추기 위함
  if(Ops_a){
    for(int i = 0; i < procCnt; i++){			
      sprintf(buff, "%lu", procList[i].rss);
      if(colWidth[RSS_IDX] < strlen(buff))
        colWidth[RSS_IDX] = strlen(buff);
    }
  } else{
    colWidth[RSS_IDX] = -1;
  }


  // TTY 최대 길이 저장
  // TTY 길이를 UPDATE
  // 출력 비율을 맞추기 위함
	for(int i = 0; i < procCnt; i++){			
		if(colWidth[TTY_IDX] < strlen(procList[i].tty))
			colWidth[TTY_IDX] = strlen(procList[i].tty);
	}
  

  // STAT 최대 길이 저장
  // STAT 길이를 UPDATE
  // 출력 비율을 맞추기 위함
  if(Ops_a || Ops_u || Ops_x){
    for(int i = 0; i < procCnt; i++){			
      if(colWidth[STAT_IDX] < strlen(procList[i].stat))
        colWidth[STAT_IDX] = strlen(procList[i].stat);
    }
  } else{
    colWidth[STAT_IDX] = -1;
  }

  // START 최대 길이 저장
  // START 길이를 UPDATE
  // 출력 비율을 맞추기 위함
  if(Ops_u){
    for(int i = 0; i < procCnt; i++){			
      if(colWidth[START_IDX] < strlen(procList[i].start))
        colWidth[START_IDX] = strlen(procList[i].start);
    }
  } else{
    colWidth[START_IDX] = -1;
  }

  // TIME 최대 길이 저장
  // TIME 길이를 UPDATE
  // 출력 비율을 맞추기 위함
	for(int i = 0; i < procCnt; i++){			
		if(colWidth[TIME_IDX] < strlen(procList[i].time))
			colWidth[TIME_IDX] = strlen(procList[i].time);
	}


  // CMD 최대 길이 저장
  // CMD 길이를 UPDATE
  // 출력 비율을 맞추기 위함
	for(int i = 0; i < procCnt; i++){			
		if(colWidth[CMD_IDX] < strlen(procList[i].cmd))
			colWidth[CMD_IDX] = strlen(procList[i].cmd);
	}


  // COMMAND 최대 길이 저장
  // COMMAND 길이를 UPDATE
  // 출력 비율을 맞추기 위함
	for(int i = 0; i < procCnt; i++){			
		if(colWidth[COMMAND_IDX] < strlen(procList[i].command))
			colWidth[COMMAND_IDX] = strlen(procList[i].command);
	}
}
void ViewMyPs(void){
  //  ps 명령어의 Column 에 해당하는 값들의 길이를 colWidth 배열로 저장
  // ex) USER_STR : "USER" , PID_STR : "PID"
  int colWidth[COLUMN_CNT] = {
    strlen(USER_STR), strlen(PID_STR), strlen(CPU_STR), strlen(MEM_STR),
		strlen(VSZ_STR), strlen(RSS_STR), strlen(TTY_STR), strlen(STAT_STR),
		strlen(START_STR), strlen(TIME_STR), strlen(CMD_STR), strlen(COMMAND_STR) 
  };

  UpdateColumnWidth(colWidth);
  // BUFFER_SIZE : 1024
  char buff[BUFFER_SIZE];
  
  int gap = 0;
  
  // USER 출력
  if(Ops_u){
    // MAX(USER) - strlen(USER_STR) 해서 차이를 구함
    gap = colWidth[USER_IDX] - strlen(USER_STR); 

    strcat(buff,USER_STR);

    // gap 차이 만큼 blank 채움
    // 좌측정렬
    for(int i=0;i < gap; i++)
      strcat(buff," ");

    strcat(buff, " ");
  }

  // PID 출력
  //PID 출력
	gap = colWidth[PID_IDX] - strlen(PID_STR);	//PID의 길이 차 구함
	for(int i = 0; i < gap; i++)				//PID 우측 정렬
		strcat(buff, " ");
	strcat(buff, PID_STR);

	strcat(buff, " ");

	//%CPU 출력
	if(Ops_u){
		gap = colWidth[CPU_IDX] - strlen(CPU_STR);	//CPU의 길이 차 구함
		for(int i = 0; i < gap; i++)				//CPU 우측 정렬
			strcat(buff, " ");
		strcat(buff, CPU_STR);

		strcat(buff, " ");
	}

	//%MEM 출력
	if(Ops_u){
		gap = colWidth[MEM_IDX] - strlen(MEM_STR);	//MEM의 길이 차 구함
		for(int i = 0; i < gap; i++)				//MEM 우측 정렬
			strcat(buff, " ");
		strcat(buff, MEM_STR);

		strcat(buff, " ");
	}

	//VSZ 출력
	if(Ops_u){
		gap = colWidth[VSZ_IDX] - strlen(VSZ_STR);	//VSZ의 길이 차 구함
		for(int i = 0; i < gap; i++)				//VSZ 우측 정렬
			strcat(buff, " ");
		strcat(buff, VSZ_STR);

		strcat(buff, " ");
	}

	//RSS 출력
	if(Ops_u){
		gap = colWidth[RSS_IDX] - strlen(RSS_STR);	//RSS의 길이 차 구함
		for(int i = 0; i < gap; i++)				//RSS 우측 정렬
			strcat(buff, " ");
		strcat(buff, RSS_STR);

		strcat(buff, " ");
	}

	//TTY 출력
	gap = colWidth[TTY_IDX] - strlen(TTY_STR);	//TTY의 길이 차 구함
	strcat(buff, TTY_STR);
	for(int i = 0; i < gap; i++)				//TTY 좌측 정렬
		strcat(buff, " ");

	strcat(buff, " ");

	//STAT 출력
	if(Ops_a || Ops_u || Ops_x){
		gap = colWidth[STAT_IDX] - strlen(STAT_STR);	//STAT의 길이 차 구함
		strcat(buff, STAT_STR);
		for(int i = 0; i < gap; i++)				//STAT 좌측 정렬
			strcat(buff, " ");

		strcat(buff, " ");
	}

	//START 출력
	if(Ops_u){
		gap = colWidth[START_IDX] - strlen(START_STR);//START의 길이 차 구함
		strcat(buff, START_STR);
		for(int i = 0; i < gap; i++)				//START 좌측 정렬
			strcat(buff, " ");

		strcat(buff, " ");
	}

	//TIME 출력
	gap = colWidth[TIME_IDX] - strlen(TIME_STR);	//TIME의 길이 차 구함
	for(int i = 0; i < gap; i++)				//TIME 우측 정렬
		strcat(buff, " ");
	strcat(buff, TIME_STR);

	strcat(buff, " ");

	//COMMAND 또는 CMD 출력
	if(Ops_a || Ops_u || Ops_x)
		strcat(buff, COMMAND_STR);
	else
		strcat(buff, CMD_STR);					//CMD 바로 출력

	buff[COLS] = '\0';							//터미널 너비만큼 잘라 출력
	printf("%s\n", buff);

	/*****		column 출력 종료	*****/


	/*****		process 출력 시작	*****/

	char token[TOKEN_LEN];
	memset(token, '\0', TOKEN_LEN);

	for(int i = 0; i < procCnt; i++){
		memset(buff, '\0', BUFFER_SIZE);

		//USER 출력
		if(Ops_u){
			gap = colWidth[USER_IDX] - strlen(procList[i].user);	//USER의 길이 차 구함
			strcat(buff, procList[i].user);
			for(int i = 0; i < gap; i++)				//USER 좌측 정렬
				strcat(buff, " ");

			strcat(buff, " ");
		}

		//PID 출력
		memset(token, '\0', TOKEN_LEN);
		sprintf(token, "%lu", procList[i].pid);
		gap = colWidth[PID_IDX] - strlen(token);		//PID의 길이 차 구함
		for(int i = 0; i < gap; i++)				//PID 우측 정렬
			strcat(buff, " ");
		strcat(buff, token);

		strcat(buff, " ");

		//%CPU 출력
		if(Ops_u){
			memset(token, '\0', TOKEN_LEN);
			sprintf(token, "%3.1Lf", procList[i].cpu);
			gap = colWidth[CPU_IDX] - strlen(token);	//CPU의 길이 차 구함
			for(int i = 0; i < gap; i++)			//CPU 우측 정렬
				strcat(buff, " ");
			strcat(buff, token);

			strcat(buff, " ");
		}

		//%MEM 출력
		if(Ops_u){
			memset(token, '\0', TOKEN_LEN);
			sprintf(token, "%3.1Lf", procList[i].mem);
			gap = colWidth[MEM_IDX] - strlen(token);	//MEM의 길이 차 구함
			for(int i = 0; i < gap; i++)			//MEM 우측 정렬
				strcat(buff, " ");
			strcat(buff, token);

			strcat(buff, " ");
		}

		//VSZ 출력
		if(Ops_u){
			memset(token, '\0', TOKEN_LEN);
			sprintf(token, "%lu", procList[i].vsz);
			gap = colWidth[VSZ_IDX] - strlen(token);	//VSZ의 길이 차 구함
			for(int i = 0; i < gap; i++)			//VSZ 우측 정렬
				strcat(buff, " ");
			strcat(buff, token);

			strcat(buff, " ");
		}

		//RSS 출력
		if(Ops_u){
			memset(token, '\0', TOKEN_LEN);
			sprintf(token, "%lu", procList[i].rss);
			gap = colWidth[RSS_IDX] - strlen(token);	//RSS의 길이 차 구함
			for(int i = 0; i < gap; i++)			//RSS 우측 정렬
				strcat(buff, " ");
			strcat(buff, token);

			strcat(buff, " ");
		}

		//TTY 출력
		gap = colWidth[TTY_IDX] - strlen(procList[i].tty);	//TTY의 길이 차 구함
		strcat(buff, procList[i].tty);
		for(int i = 0; i < gap; i++)						//TTY 좌측 정렬
			strcat(buff, " ");

		strcat(buff, " ");

		//STAT 출력
		if(Ops_a || Ops_u || Ops_x){
			gap = colWidth[STAT_IDX] - strlen(procList[i].stat);	//STAT의 길이 차 구함
			strcat(buff, procList[i].stat);
			for(int i = 0; i < gap; i++)				//STAT 좌측 정렬
				strcat(buff, " ");

			strcat(buff, " ");
		}

		//START 출력
		if(Ops_u){
			gap = colWidth[START_IDX] - strlen(procList[i].start);	//START의 길이 차 구함
			strcat(buff, procList[i].start);
			for(int i = 0; i < gap; i++)				//START 좌측 정렬
				strcat(buff, " ");

			strcat(buff, " ");
		}

		//TIME 출력
		gap = colWidth[TIME_IDX] - strlen(procList[i].time);	//TIME의 길이 차 구함
		for(int i = 0; i < gap; i++)				//TIME 우측 정렬
			strcat(buff, " ");
		strcat(buff, procList[i].time);

		strcat(buff, " ");

		//COMMAND 또는 CMD 출력
		if(Ops_a || Ops_u || Ops_x){
			strcat(buff, procList[i].command);		//COMMAND 바로 출력
		}
		else{
			strcat(buff, procList[i].cmd);			//CMD 바로 출력
		}

		buff[COLS] = '\0';							//터미널 너비만큼만 출력
		printf("%s\n", buff);

	}

	/*****		process 출력 종료	*****/

	return;
}

int main(int argc, char *argv[])
{
	memTotal = get_mem_total();					//전체 물리 메모리 크기
	hertz = (unsigned int)sysconf(_SC_CLK_TCK);	//os의 hertz값 얻기(초당 context switching 횟수)

	myPid = getpid();			//자기 자신의 pid

	char pidPath[FNAME_LEN];
	memset(pidPath, '\0', FNAME_LEN);
	sprintf(pidPath, "/%d", myPid);

	strcpy(myPath, PROC);			//자기 자신의 /proc 경로 획득
	strcat(myPath, pidPath);

	getTTY(myPath, myTTY);			//자기 자신의 tty 획득


	for(int i = strlen(PTS); i < strlen(myTTY); i++)
		if(!isdigit(myTTY[i])){
			myTTY[i] = '\0';
			break;
		}

	myUid = getuid();			//자기 자신의 uid

	initscr();				//출력 윈도우 초기화
	endwin();				//출력 윈도우 종료

	for(int i = 1; i < argc; i++){					//Option 획득
		for(int j = 0; j < strlen(argv[i]); j++){
			switch(argv[i][j]){
				case 'a':
					Ops_a = true;
					break;
				case 'u':
					Ops_u = true;
					break;
				case 'x':
					Ops_x = true;
					break;
			}
		}
	}

  // Option에 맞게 실행
	search_proc(true, Ops_a, Ops_u, Ops_x, NULL);
	ViewMyPs();

	return 0;
}