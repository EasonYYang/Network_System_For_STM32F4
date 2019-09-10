#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	
#include "timer.h"
#include "sram.h"
#include "malloc.h"
#include "string.h"
#include "key.h"
#include "BEEP.h"
#include "lan8720.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "mpu6050.h"
#include "inv_mpu.h"

#define USE_LCD

#ifdef USE_LCD

	#include "lcd.h"
#endif

#ifdef USE_USMART
	#include "usmart.h"	
#endif

#include "tcp_client_connection.h"


/************************************************

ALIENTEKս��STM32������LWIPʵ��
 ����RAW API��TCP Clientʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
 

Prototype: TCP Client experiment based on RAW API.
The original author: ����ԭ�� @ALIENTEK
	
General embedded network system based on STM32F4.
Author: 	Eason. Yang
Contact:	yyichen@kth.se
Date:		2019-08-09
Version:	1.0

************************************************/

extern u8 tcp_client_flag;	 //TCP Client ����ȫ��״̬��Ǳ���


#ifdef USE_LCD
//����UI
//mode:
//bit0:0,������;1,����ǰ�벿��UI
//bit1:0,������;1,���غ�벿��UI
void lwip_test_ui(u8 mode)
{
	u8 speed;
	u8 *buf; 
	buf=mymalloc(SRAMIN,30);	//Allocate memory for buf
	if(buf==NULL)return;
	POINT_COLOR=RED;
	if(mode&1<<0)
	{
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//�����ʾ
		LCD_ShowString(30,30,200,16,16,"TCP Client Test for STM32F4");
		LCD_ShowString(30,50,200,16,16,"Eason. Yang");
		LCD_ShowString(30,70,200,16,16,"Contact: yyichen@kth.se");
		LCD_ShowString(30,90,200,16,16,"2019/8/9"); 
		LCD_ShowString(30,170,200,16,16,"KEY1: Connect!");
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//�����ʾ
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
		else sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
		LCD_ShowString(30,130,210,16,16,buf); 
		speed=LAN8720_Get_Speed();//�õ�����
		if(speed&1<<1)LCD_ShowString(30,150,200,16,16,"Ethernet Speed:100M");
		else LCD_ShowString(30,150,200,16,16,"Ethernet Speed:10M");
		LCD_ShowString(30,170,200,16,16,"PORT: 8087");
		LCD_ShowString(30,190,200,16,16,"KEY1: Connect!");
	}
	myfree(SRAMIN,buf);
}
#endif

int main(void)
{	 
	u8 key;
	delay_init(168);	    	//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			    //LED�˿ڳ�ʼ��
	KEY_Init();	 			//��ʼ������
	TIM3_Int_Init(1000,719);//��ʱ��3Ƶ��Ϊ100hz
	TIM5_Int_Init(19999,7199);
 	FSMC_SRAM_Init();		//��ʼ���ⲿSRAM
	my_mem_init(SRAMIN);	//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);	//��ʼ���ⲿ�ڴ��
	BEEP_Init();
	MPU_Init();

// �� tcp_client_demo.h �ﶨ��������	
#ifdef USE_USMART
 	usmart_dev.init(72);	//��ʼ��USMART	
#endif
#ifdef USE_LCD	
    LCD_Init();				//��ʼ��LCD
	POINT_COLOR = RED; 		
	lwip_test_ui(1);		//����ǰ�벿��UI		 
	
	while(lwip_comm_init()) //lwip��ʼ��
	{
		LCD_ShowString(30,110,200,20,16,"LWIP Init Falied!");
		delay_ms(1200);
		LCD_Fill(30,110,230,130,WHITE); //�����ʾ
		LCD_ShowString(30,110,200,16,16,"Retrying..."); 
	}
	LCD_ShowString(30,110,200,20,16,"LWIP Init Succeed!");
	
	#if LWIP_DHCP   //ʹ��DHCP
	LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");
	//printf("DHCP IP configing...\r\n");
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
	{
		lwip_periodic_handle();	//LWIP�ں���Ҫ��ʱ����ĺ���
		lwip_pkt_handle();
	}
	#endif
	lwip_test_ui(2);		//���غ�벿��UI 
	delay_ms(500);			//��ʱ0.5s
	//tcp_client_connection();  	//TCP Clientģʽ
  	while(1)
	{	
		key = KEY_Scan(0);
		if(key == KEY1_PRES)		//��KEY1����������
		{
			if((tcp_client_flag & 1<<5)) printf("TCP connection is already established!\r\n");	//������ӳɹ�,�����κδ���
			else tcp_client_connection();		//���Ͽ����Ӻ�,��tcp_client_test()����
		}
		delay_ms(10);
	}
	
#else	

	while(lwip_comm_init()) //lwip��ʼ��
	{
		printf("LWIP Init Falied!\r\n");
		delay_ms(1200);
		printf("Retrying...");		
	}
	printf("LWIP Init Succeed!\r\n");
		#if LWIP_DHCP   //ʹ��DHCP
		printf("DHCP IP configing...\r\n");
		while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
		{
			lwip_periodic_handle();	//LWIP�ں���Ҫ��ʱ����ĺ���
			lwip_pkt_handle();
		}
		#endif

		delay_ms(500);			//��ʱ0.5s
		tcp_client_connection();  	//TCP Clientģʽ
		//��ʱ�Ѿ��Ͽ�����
		while(1)
		{	
			key = KEY_Scan(0);
			if(key == KEY1_PRES)		//��KEY1����������
			{
				if((tcp_client_flag & 1<<5)) printf("TCP connection is already established!\r\n");	//������ӳɹ�,�����κδ���
				else tcp_client_connection();		//���Ͽ����Ӻ�,��tcp_client_test()����
			}
			delay_ms(10);
		}
#endif
}
