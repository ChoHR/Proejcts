#include <avr/io.h>
#include <avr/interrupt.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define LCD_CTRL PORTG
#define LCD_DATA PORTA
#define LCD_RS 0x01  // PG0
#define LCD_RW 0x02  //PG1
#define LCD_E 0x04  //PG2

typedef struct linked_list L;
typedef struct Database Data;

struct Database
{
	char* phone_number;
	char* name;
}

struct linked_list
{
	Data data;
	L*	 next;
}

int Register();
int Eraser();

int Searcher();
int Home();

// 전역변수 설정

/*	Reg_status
	0 : Home
	1 : Register 화면
	2 : 등록하고 Home으로
*/
volatile unsigned int Reg_status = 0;

/*	Ers_status
	0 : Home
	1 : Eraser 화면
*/
volatile unsigned int Ers_status = 0;

/*	Sch_status
	0 : Home
	1 : Searcher 화면
*/
volatile unsigned int Sch_status = 0;
volatile unsigned int Home_status = 0;


volatile unsigned int Key_UpClick = 0;
volatile unsigned int Key_DownClick = 0;
volatile unsigned int Key_RightClick = 0;
volatile unsigned int Key_LeftClick = 0;

volatile unsigned int Touch = 0;

volatile unsigned int OnClick = 0;

void delay(unsigned long x)
{
 while(x--);
}

// 입력키
SIGNAL(SIG_INTERRUPT0)
{
 OnClic = 1;
 Touch=0;
 Reg_status++;
}

// 삭제키
SIGNAL(SIG_INTERRUPT1)
{
 OnClic = 1;
 Touch=1;
 Ers_status++;
}

// 검색키
SIGNAL(SIG_INTERRUPT2)
{
 OnClic = 1;
 Touch=2;
 Sch_status++;
}

// 홈키
SIGNAL(SIG_INTERRUPT3)
{
 OnClic = 1;
 Touch=3;
 Home_status++;
}

// 상
SIGNAL(SIG_INTERRUPT4)
{
 OnClic = 1;
 Touch=4;
 Key_UpClick;
}

// 하
SIGNAL(SIG_INTERRUPT5)
{
 OnClic = 1;
 Touch=5;
 Key_DownClick++;
}

// 좌
SIGNAL(SIG_INTERRUPT6)
{
 OnClic = 1;
 Touch=6;
 Key_LeftClick++;
}

// 우
SIGNAL(SIG_INTERRUPT7)
{
 OnClic = 1;
 Touch=7;
Key_RightClick++;
}


void EnablePulse(void)
{
  //----------------
  //펄스 만들기
  //----------------
    LCD_CTRL |=LCD_E;  // E=1 로 만든다. 초기값(이전 값)은 0이었어야 한다.
    //다름 bit는 변화 없도록 건들이지 않는다.
    asm(“NOP”);  // 아무것도 안하는 어셈블리 명령어
    //명령어 1 싸이클의 아주 짧은 시간 지연을 준다.
    LCD_CTRL &= ~LCD_E;  //다시 E=0으로 만든다.
    //다른 bit는 변화 없도록 건들이지 않는다.
}
void sendLCDcommand(unsigned char command)
{ LCD_CTRL &=~LCD_RS;  // 명령을 전달할 때 RS=0
  LCD_DATA=command;  // 전송할 명령 code값을 data bus에 실어 놓는다.
  EnablePulse();  //enable pulse를 내보낸다. 이 시점에서 명령이 전송된다.
  delay(20);  //표 7.2에 보면 전송된 명령을 LCD가 받아들여 처리하는데 걸리는 시간이 
  // 표시되어 있는데 대부분이 39[us]로 되어 있다. 46[us]정도라고 예상되는 시간 지연을 둔다.
}
void lnitLcd(void)
{ // LCD 모듈을 사용하려면 먼저 초기화 명령을 전달해 주어야 한다. 
  // LCD에 명령을 전달할 때에는 RS=0, RW=0으로 놓고 (command 함수)
  // data에 필요한 명령을 출력한 후에, (command 함수)
  // E 에 펄스를 가해준다. (command 함수)
  // 시간 지연을 갖는다. (command 함수)
  //----------
  // 모듈 초기화 명령
  // 최초에 전원을 넣은 후 LCD가 준지 될 때 까지 충분히 기다린다.
delay(20000); // 30 [ms] 이상 안정화 시킨다.
  LCD_CTRL &=~LCD_RW;  // LCD 에 명령이나 자료를 전송할 때 항상 RW=0
  sendLCDcommand(0x38);  // function 설정 8bit data 를 한번에 처리,
  // 2Line, 5*7 dot
  sendLCDcommand(0x0C);  // Display를 ON 시킨다. 이걸 안 하면 글자표시 안 됨. 커서는 없앰.
  sendLCDcommand(0x01);  // Clear Screen
  delay(1000);  // 1.53[ms] 이상 되어야 함. -> 계산상 2[ms]이상으로 예측
  // Clear Screen 명령은 실행 시간이 길기 때문에 충분히 기다림.
}
void sendLCDdata(unsigned char data)
{ 
  LCD_CTRL |=LCD_RS;  // data를 보내기 위해 RS=1 만든다.
  LCD_DATA=data;  //  전송할 문자 code값을 data bus에 실어 놓는다.
  EnablePulse();  // enable pulse를 내보낸다. 이 시점에서 명령이 전송된다.
  delay(20);  // data 전송 후 LCD가 처리하는데 시간이 걸린다.
}
void DispString(char *str)
{
   while(*str)
   {
      sendLCDdata(*str);
      str++;
    }
}
void Locata(int x, int y)
{ 
  unsigned char RamAddr;  
  //------ 
  // 임의 위치에 표시하기 위해 위치를 지정해 준다.
  //------
  if(x==0) RamAddr=0x80+y;  //line 1 D.D.RAM address
  else RamAddr=0xC0+y;  //line 2 D.D.RAM address
  sendLCDcommand(RamAddr);
}

int main(void)
{
/////////////////
int position = 0;
int char_position = 0;
int reg_item = 0;
char Reg_name[7]  = "name  ";
char Reg_phone[7] = "phone ";
char character[63] = {' ','0','1','2','3','4','5','6','7','8','9'
					 ,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'
					 ,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
L* list = (L*)malloc(sizeof(L));
list->next = NULL;
list->data.name = NULL;
list->data.phone_number = NULL;

L* database = (L*)malloc(sizeof(L));
list->next = NULL;
list->data.name = NULL;
list->data.phone_number = NULL;

Home();
DDRA=0xFF; 
DDRG=0xFF; // A 와 G 포트를 출력으로 지정
PORTA=0x00;  
PORTG=0x00;  // A  와 G 포트 값의 초기 값 지정

lnitLcd();
DispString(“Happy Day”);
Locate(1, 4);  // 2번째 줄 5째 칸에서부터 문자 표시
DispString(“Welcome !”);

while(1)
{
  if(OnClick == 1){
	// 홈화면에서 입력, 삭제, 검색, 홈키 누를 시
	if(Touch == 0)
	{
		if(Reg_status == 2)
		{
			L* data = (L*)malloc(sizeof(L));
			data->data.name = list->data.name = NULL;
			data->data.phone_number = list->data.phone_number;
			data->next = NULL;

			// database 는 Header로 NULL값이다. 실제 데이터는 그 다음부터 들어간다.
			if(database->next==NULL){
				database->next=data;
			}
			else{
				L* temp=database->next;
				while(temp->next != NULL) temp=temp->next;
				temp->next=data;
			}

			database->next = list;
			Home();
			Reg_status == 0;
		}
		else if(Reg_status == 1){
			//전화번호 등록 화면으로 이동
			list->next = NULL;
			list->data.name = NULL;
			list->data.phone_number = NULL;
	   	    lnitLcd();
		    DispString("name ");
		    Locate(1, 0);  // 2번째 줄 0째 칸에서부터 문자 표시
		    DispString("phone");
			
		}
	}
	else if(Touch == 1)
	{
		if(Ers_status == 2)
		{
			//자료삭제 후 홈화면으로 이동
			Home();
			Ers_status == 0;
		}
		else if(Ers_status == 1){

			//전화번호 삭제 화면으로 이동
			Eraser();

		}
	}
	else if(Touch == 2) 
	{
		if(Sch_status == 2)
		{
			//자료검색. 홈으로 이동하려면 홈키 입력.
			Home();
			Sch_status == 0;
		}
		else if(Sch_status == 1){

			//전화번호 검색 화면으로 이동
			Searcher();
		}
	}
	else if(Touch == 3)
	{
		if(Home_status == 1){

			//홈 화면으로 이동
			Home();
		}
	}
	
	else if(Touch == 4 || Touch == 5 || Touch == 6 || Touch == 7)
	{
		//Register에서 각 항목 입력시의 키보드
		if(Reg_status == 1){
			if(Key_UpClick == 1){
				if(position == -1){
					if(reg_item != 0)
						reg_item = 0;
				}
				else{
					char_position = (char_position+1)%63;
					if(reg_item == 0){
						list->data.name[position] = character[char_position];
						Locate(0, 6);  // 2번째 줄 7째 칸에서부터 문자 표시
					}
					else{
						list->data.phone_number[position] = character[char_position];
						Locate(1, 6);  // 2번째 줄 7째 칸에서부터 문자 표시
					}	
					DispString(character);
					Key_UpClick = 0;
				}
			}
			if(Key_DownClick == 1){
				if(position == -1){
					if(reg_item != 1)
						reg_item = 1;
				}
				else{
					char_position = (char_position-11)%63;
					if(reg_item ==0){
						list->data.name[position] = character[char_position];
						Locate(0, 6);  // 2번째 줄 7째 칸에서부터 문자 표시
					}
					else{
						list->data.phone_number[position] = character[char_position];
						Locate(1, 6);  // 2번째 줄 7째 칸에서부터 문자 표시
					}
					DispString(character);
					Key_DownClick = 0;
				}
			}
			if(Key_RightClick == 1){
				position++;
				char_position  = 0;
				Key_RightClick = 0;
			}
			if(Key_LeftClick == 1){
				if(position < 0)
					position = -1;
				else
					position--;
				char_position = 0;
				Key_LeftClick = 0;
			}
		}
	}
	else{
		// 동작안함
	}
	OnClick = 0;
  }

  return 0;
}
