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

// �������� ����

/*	Reg_status
	0 : Home
	1 : Register ȭ��
	2 : ����ϰ� Home����
*/
volatile unsigned int Reg_status = 0;

/*	Ers_status
	0 : Home
	1 : Eraser ȭ��
*/
volatile unsigned int Ers_status = 0;

/*	Sch_status
	0 : Home
	1 : Searcher ȭ��
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

// �Է�Ű
SIGNAL(SIG_INTERRUPT0)
{
 OnClic = 1;
 Touch=0;
 Reg_status++;
}

// ����Ű
SIGNAL(SIG_INTERRUPT1)
{
 OnClic = 1;
 Touch=1;
 Ers_status++;
}

// �˻�Ű
SIGNAL(SIG_INTERRUPT2)
{
 OnClic = 1;
 Touch=2;
 Sch_status++;
}

// ȨŰ
SIGNAL(SIG_INTERRUPT3)
{
 OnClic = 1;
 Touch=3;
 Home_status++;
}

// ��
SIGNAL(SIG_INTERRUPT4)
{
 OnClic = 1;
 Touch=4;
 Key_UpClick;
}

// ��
SIGNAL(SIG_INTERRUPT5)
{
 OnClic = 1;
 Touch=5;
 Key_DownClick++;
}

// ��
SIGNAL(SIG_INTERRUPT6)
{
 OnClic = 1;
 Touch=6;
 Key_LeftClick++;
}

// ��
SIGNAL(SIG_INTERRUPT7)
{
 OnClic = 1;
 Touch=7;
Key_RightClick++;
}


void EnablePulse(void)
{
  //----------------
  //�޽� �����
  //----------------
    LCD_CTRL |=LCD_E;  // E=1 �� �����. �ʱⰪ(���� ��)�� 0�̾���� �Ѵ�.
    //�ٸ� bit�� ��ȭ ������ �ǵ����� �ʴ´�.
    asm(��NOP��);  // �ƹ��͵� ���ϴ� ����� ��ɾ�
    //��ɾ� 1 ����Ŭ�� ���� ª�� �ð� ������ �ش�.
    LCD_CTRL &= ~LCD_E;  //�ٽ� E=0���� �����.
    //�ٸ� bit�� ��ȭ ������ �ǵ����� �ʴ´�.
}
void sendLCDcommand(unsigned char command)
{ LCD_CTRL &=~LCD_RS;  // ����� ������ �� RS=0
  LCD_DATA=command;  // ������ ��� code���� data bus�� �Ǿ� ���´�.
  EnablePulse();  //enable pulse�� ��������. �� �������� ����� ���۵ȴ�.
  delay(20);  //ǥ 7.2�� ���� ���۵� ����� LCD�� �޾Ƶ鿩 ó���ϴµ� �ɸ��� �ð��� 
  // ǥ�õǾ� �ִµ� ��κ��� 39[us]�� �Ǿ� �ִ�. 46[us]������� ����Ǵ� �ð� ������ �д�.
}
void lnitLcd(void)
{ // LCD ����� ����Ϸ��� ���� �ʱ�ȭ ����� ������ �־�� �Ѵ�. 
  // LCD�� ����� ������ ������ RS=0, RW=0���� ���� (command �Լ�)
  // data�� �ʿ��� ����� ����� �Ŀ�, (command �Լ�)
  // E �� �޽��� �����ش�. (command �Լ�)
  // �ð� ������ ���´�. (command �Լ�)
  //----------
  // ��� �ʱ�ȭ ���
  // ���ʿ� ������ ���� �� LCD�� ���� �� �� ���� ����� ��ٸ���.
delay(20000); // 30 [ms] �̻� ����ȭ ��Ų��.
  LCD_CTRL &=~LCD_RW;  // LCD �� ����̳� �ڷḦ ������ �� �׻� RW=0
  sendLCDcommand(0x38);  // function ���� 8bit data �� �ѹ��� ó��,
  // 2Line, 5*7 dot
  sendLCDcommand(0x0C);  // Display�� ON ��Ų��. �̰� �� �ϸ� ����ǥ�� �� ��. Ŀ���� ����.
  sendLCDcommand(0x01);  // Clear Screen
  delay(1000);  // 1.53[ms] �̻� �Ǿ�� ��. -> ���� 2[ms]�̻����� ����
  // Clear Screen ����� ���� �ð��� ��� ������ ����� ��ٸ�.
}
void sendLCDdata(unsigned char data)
{ 
  LCD_CTRL |=LCD_RS;  // data�� ������ ���� RS=1 �����.
  LCD_DATA=data;  //  ������ ���� code���� data bus�� �Ǿ� ���´�.
  EnablePulse();  // enable pulse�� ��������. �� �������� ����� ���۵ȴ�.
  delay(20);  // data ���� �� LCD�� ó���ϴµ� �ð��� �ɸ���.
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
  // ���� ��ġ�� ǥ���ϱ� ���� ��ġ�� ������ �ش�.
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
DDRG=0xFF; // A �� G ��Ʈ�� ������� ����
PORTA=0x00;  
PORTG=0x00;  // A  �� G ��Ʈ ���� �ʱ� �� ����

lnitLcd();
DispString(��Happy Day��);
Locate(1, 4);  // 2��° �� 5° ĭ�������� ���� ǥ��
DispString(��Welcome !��);

while(1)
{
  if(OnClick == 1){
	// Ȩȭ�鿡�� �Է�, ����, �˻�, ȨŰ ���� ��
	if(Touch == 0)
	{
		if(Reg_status == 2)
		{
			L* data = (L*)malloc(sizeof(L));
			data->data.name = list->data.name = NULL;
			data->data.phone_number = list->data.phone_number;
			data->next = NULL;

			// database �� Header�� NULL���̴�. ���� �����ʹ� �� �������� ����.
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
			//��ȭ��ȣ ��� ȭ������ �̵�
			list->next = NULL;
			list->data.name = NULL;
			list->data.phone_number = NULL;
	   	    lnitLcd();
		    DispString("name ");
		    Locate(1, 0);  // 2��° �� 0° ĭ�������� ���� ǥ��
		    DispString("phone");
			
		}
	}
	else if(Touch == 1)
	{
		if(Ers_status == 2)
		{
			//�ڷ���� �� Ȩȭ������ �̵�
			Home();
			Ers_status == 0;
		}
		else if(Ers_status == 1){

			//��ȭ��ȣ ���� ȭ������ �̵�
			Eraser();

		}
	}
	else if(Touch == 2) 
	{
		if(Sch_status == 2)
		{
			//�ڷ�˻�. Ȩ���� �̵��Ϸ��� ȨŰ �Է�.
			Home();
			Sch_status == 0;
		}
		else if(Sch_status == 1){

			//��ȭ��ȣ �˻� ȭ������ �̵�
			Searcher();
		}
	}
	else if(Touch == 3)
	{
		if(Home_status == 1){

			//Ȩ ȭ������ �̵�
			Home();
		}
	}
	
	else if(Touch == 4 || Touch == 5 || Touch == 6 || Touch == 7)
	{
		//Register���� �� �׸� �Է½��� Ű����
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
						Locate(0, 6);  // 2��° �� 7° ĭ�������� ���� ǥ��
					}
					else{
						list->data.phone_number[position] = character[char_position];
						Locate(1, 6);  // 2��° �� 7° ĭ�������� ���� ǥ��
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
						Locate(0, 6);  // 2��° �� 7° ĭ�������� ���� ǥ��
					}
					else{
						list->data.phone_number[position] = character[char_position];
						Locate(1, 6);  // 2��° �� 7° ĭ�������� ���� ǥ��
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
		// ���۾���
	}
	OnClick = 0;
  }

  return 0;
}
