#include "public.h"

/**
* @}��������ѧԺ ����ѵ������ 
    ����161  ���ұ� �������
		2018.10.29
  */
#define START_SHIBIE Send_string(start_shibie,9,UART_2)  //Send_string(start_shibie,9,UART_2)  //ʶ��

uint8 flag_NUM;
uint8 flag_Gray_A;
uint8 flag_now;
uint8 flag_go;

u8 shibie_over=0;//shangpo_over=0;
u8 HMI_Receive_over=0;


/*		Uart5_Send_string("#004P0520T0800!");    ����̨  1


*/
void uart_init(u32 pclk2,u32 bound);

char WuLiao[2]={0};

char start_shibie[9]={0x7E,0x00,0x08,0x01,0x00,0x02,0x01,0xAB,0xCD};  //ʶ��ָ��
char board_1[10]={0};       //���������� ��ץȡ�����
char board_2[6]={0};        //���������� ��ץȡ���������
char board_flag[6]={0X31};     //���ץȡ�����
 
bool couch_flag = 0;        //�Ƿ��Ǻϱ�־
bool Choose_flag = 0;       //HMI�Ƿ�ѡ��

void  Start_clock()  //ʱ�ӳ�ʼ��������ʱ�ӵĿ���
{
		uint8  HSEStartUpStatus;   
		RCC_DeInit();    //RCC ��λ
		RCC_HSEConfig(RCC_HSE_ON); //�����ⲿʱ�� ��ִ�г�ʼ��
		HSEStartUpStatus = RCC_WaitForHSEStartUp();	//�ȴ��ⲿʱ��׼����
		while(HSEStartUpStatus == ERROR);	  //����ʧ�� ������ȴ�
	
		RCC_HCLKConfig(RCC_SYSCLK_Div1);		//�����ڲ�����ʱ��
		RCC_PCLK1Config(RCC_HCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);//�ⲿʱ��Ϊ8M ���ﱶƵ��72M
	
		RCC_PLLCmd(ENABLE); 
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);  //�ȴ�PLL���þ��� 
	
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);       
		while(RCC_GetSYSCLKSource() != 0x08);       		//�л�ϵͳʱ��ΪPLL���ȴ����
	
		RCC_HSICmd(DISABLE);			         //�ر��ڲ�ʱ��HSI
	
	  ///////******************ʱ�ӿ���******************/////
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);   							//TIM5,TIM3��ʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_UART4|RCC_APB1Periph_UART5,ENABLE);  //��������ʱ��	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE); //GPIO��ʱ��
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);   //DMA��ʱ��
}


void test()
{
		while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)==1);
		TIM5->CCR3=150;
		TIM5->CCR4=150;
		TIM5->CCR2=150;
}

void Init()         //��ʼ��
{
	//	GPIO_InitTypeDef GPIO_InitStructure;
	Start_clock();
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);           //��ռΪ0-1����ӦΪ0-7
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);   //ӳ������

	GPIO_PWM_INIT();      //PWM_GPIO��ʼ��
	TIM1_PWM_INIT();      //PWM��ʼ��
	GRAY_GPIO_INIT();     //�Ҷ�����ʼ��
	GPIO_Gray_INIT();	    //���Ҷȣ����⣩��ʼ��
	UART_GPIO_INIT();     //����GPIO��ʼ��
	UART2_INIT();         //ɨ��ģ���ʼ��
	UART5_INIT();         //��е�۳�ʼ��
	UART4_INIT();         //��������ʼ��
	USART_Cmd(USART2, ENABLE);//ɨ��ģ��ʹ��
	USART_Cmd(UART5, ENABLE);	//������ʹ��	
	delay_ms(1000);				//��1s

	Uart5_Send_string("#000P0520T0800!");
	delay_ms(50);
	Uart5_Send_string("#001P1170T0800!");
	delay_ms(50);
	Uart5_Send_string("#002P0750T0800!");   //��С����
	delay_ms(50);
	Uart5_Send_string("#003P1955T0800!");
	delay_ms(50);
	Uart5_Send_string("#004P0520T0500!");
	delay_ms(50);
	Uart5_Send_string("#005P0900T0800!");      //0800�ſ�
	delay_ms(50);                        //��ʼ����*/
	BUZZER_ON;
	delay_ms(700);
	BUZZER_OFF;
}

void GET()          //��׽��
{
	if(!Gray_STOP_A)
	{
			delay_ms(8);
			if(!Gray_STOP_A)
			{
					flag_Gray_A++;
					if(flag_Gray_A==3)
						flag_Gray_A=0;
			}
	}	
}
void Avoiding()     //����
{
				Run_motor(-200,-200);  //��
				delay_ms(400);
				STOP;
				
				Run_motor(-300,300); 	    //��ת
				delay_s(1);
				delay_ms(200);
				STOP;
				
				Run_motor(300,300);      //ֱ��  
				delay_ms(700);
				STOP;
				
				Run_motor(300,-300);     //��ת
				delay_s(1);
				delay_ms(310);
				STOP;
				/*
				Run_motor(300,300);      //ֱ��
				delay_s(1);
				delay_ms(80);
				STOP;
				
				Run_motor(300,-300);     //��ת
				delay_s(1);
				delay_ms(300);
				STOP;*/
				turn_left();
				
				Run_motor(300,300);      //ֱ��  
				delay_ms(150);
			
				STOP;
				
				Run_motor(-300,300); 	    //��ת
				delay_s(1);
				delay_ms(200);
			//	delay_ms(1050);
				STOP;	

}
void Arrange()      //��ľ��������̨ƥ��
{
	  u8 i=0,j=0;
		if(HMI_Receive_over==1)
		{
			for(i=0;i<10;i++)
				if(board_1[i]==1)
				{
					board_flag[j]=i+1;
					j++;
				}
		}
}
void Rotate_UP()     //����̨װ������
{
	if(WuLiao[1]==board_flag[0])
		Uart5_Send_string("#004P0570T0800!");    //����̨  1
	else if(WuLiao[1]==board_flag[1])
		Uart5_Send_string("#004P0940T0800!");    //����̨  2
	else if(WuLiao[1]==board_flag[2])
		Uart5_Send_string("#004P1380T0800!");    //����̨  3
	else if(WuLiao[1]==board_flag[3])
		Uart5_Send_string("#004P1850T0800!");    //����̨  4
	else if(WuLiao[1]==board_flag[4])
		Uart5_Send_string("#004P2310T0800!");    //����̨  5
	//else if(WuLiao[1]==board_flag[5])
	//	Uart5_Send_string("#004P1520T0800!");    //����̨  6
}
void Judge()         //ɨ���ж�  
{
	static  u8 n=0;
	START_SHIBIE;
	delay_ms(1000);
	flag_now++;
	if((WuLiao[1]==board_flag[0]) || (WuLiao[1]==board_flag[1]) || (WuLiao[1]==board_flag[2]))   //�˶�
	{
		while(shibie_over)
		{
			shibie_over=0;
			couch_flag=1;
			
			START_SHIBIE;
		}
		Rotate_UP();
		delay_ms(100);
	}
}
void Recognition_1() //ǰ��ɨ�趯��
{
		Uart5_Send_string("#002P1250T0800!");
		delay_ms(50);
		Uart5_Send_string("#000P1120T0800!");
		delay_ms(50);
		Uart5_Send_string("#001P1220T0800!");
		delay_ms(50);
		Uart5_Send_string("#003P1240T0800!");
    delay_s(2);
}
void Recognition_2() //����ɨ�趯��
{
		Uart5_Send_string("#002P1250T0800!");
		delay_ms(50);
		Uart5_Send_string("#000P1120T0800!");
		delay_ms(50);
		Uart5_Send_string("#001P1280T0800!");
		delay_ms(50);
		Uart5_Send_string("#003P1350T0800!");
    delay_s(2);
}
void Ahold_1()       //ץȡ����1 ��ǰ�ţ�
{   
	
		Uart5_Send_string("#000P1120T0800!");   //��ʼץȡ
		delay_ms(500);
		Uart5_Send_string("#001P1450T0800!");
		delay_ms(100);
		Uart5_Send_string("#002P1280T0800!");
		delay_ms(100);
		Uart5_Send_string("#003P1355T0800!");
		delay_ms(300);
		Uart5_Send_string("#005P1465T0800!");
		delay_ms(100);                           //ǰ��ץȡ��������
			
		delay_s(2);  
		
		Uart5_Send_string("#002P1250T0500!");//̧��
		delay_ms(150);
		Uart5_Send_string("#003P1350T0800!");//
		delay_ms(100);
		Uart5_Send_string("#001P1000T0800!");
		delay_ms(600);
		Uart5_Send_string("#000P0500T0800!");
		delay_ms(100);
					
	//	Uart5_Send_string("#004P0570T0800!");
	//	delay_ms(100);
		Uart5_Send_string("#002P1080T0800!");
		delay_ms(100);
		Uart5_Send_string("#003P1080T0800!");
		delay_ms(300);
		
		delay_s(1);
		Uart5_Send_string("#005P1000T0800!");
		delay_ms(100);
		Uart5_Send_string("#003P1300T0500!");  //װ��
		delay_ms(500);		
		couch_flag=0;	
		flag_NUM++;
}

void Ahold_2()       //ץȡ����2 �����ţ�
{
		Uart5_Send_string("#005P0940T0800!");
		delay_ms(100); 
	
		Uart5_Send_string("#000P1120T0800!");   //��ʼץȡ
		delay_ms(500);
		Uart5_Send_string("#001P1640T1000!");
		delay_ms(100);
		Uart5_Send_string("#003P1575T0500!");
		delay_ms(50);
		Uart5_Send_string("#002P1280T0500!");
		delay_ms(800);

		Uart5_Send_string("#005P1465T0800!");
		delay_ms(100);                           //ǰ��ץȡ��������
			
		delay_s(2);  
		
		Uart5_Send_string("#003P1600T0800!");//̧��
		delay_ms(150);
		Uart5_Send_string("#002P1200T1000!");//
		delay_ms(100);
		Uart5_Send_string("#001P1000T1000!");
		delay_ms(600);
		Uart5_Send_string("#000P0500T0800!");
		delay_ms(100);
					
		Uart5_Send_string("#002P1080T1000!");
		delay_ms(100);
		Uart5_Send_string("#003P1080T1000!");
		delay_ms(300);
		
		delay_s(1);
		Uart5_Send_string("#005P1000T0800!");
		delay_ms(100);
		Uart5_Send_string("#003P1300T0500!");  //װ��		
		delay_ms(500);		
		couch_flag=0;	
		flag_NUM++;
}

void Dropt_up()      //���϶���1 ���ϲ㣩
{
	flag_Gray_A=1;
		if(flag_Gray_A==1)
		{
				STOP;
				BUZZER_ON;
				delay_ms(700);
				BUZZER_OFF;
		  	Uart5_Send_string("#004P0570T0500!");

	    	delay_ms(100);
		  	Uart5_Send_string("#000P0500T0800!");
				delay_ms(100);
				Uart5_Send_string("#005P0850T0800!");
				delay_ms(50);
				//Uart5_Send_string("#003P1300T0800!"); 
				delay_s(1);

				Uart5_Send_string("#002P1070T0800!");
				delay_ms(50);
				
				Uart5_Send_string("#001P0935T0800!");
				delay_ms(50);
				Uart5_Send_string("#003P1145T0800!");
				delay_ms(500);
				
				Uart5_Send_string("#005P1460T0800!");   //��ȡ
				delay_ms(50);
				delay_s(2);
			
				Uart5_Send_string("#002P1100T0800!");   //̧��
				delay_ms(800);
				Uart5_Send_string("#003P1570T0800!");
				delay_ms(800);
				
				Uart5_Send_string("#000P1110T0800!");
				delay_ms(800);
				
				Uart5_Send_string("#002P0890T0800!");   //̧��
				delay_ms(1000);
				
				Uart5_Send_string("#001P1030T0800!");
				delay_ms(800);
				Uart5_Send_string("#005P1058T0800!");  //����
				delay_ms(800);
				Uart5_Send_string("#001P0935T0800!");
				
				flag_Gray_A++;
				while(1);
		}
}
void Dropt_down()    //���϶���2 ���²㣩
{
	 flag_Gray_A=1;
		if(flag_Gray_A==1)
		{
				STOP;
				BUZZER_ON;
				delay_ms(700);
				BUZZER_OFF;
		  	Uart5_Send_string("#004P0570T0500!");

	    	delay_ms(100);
		  	Uart5_Send_string("#000P0500T0800!");
				delay_ms(100);
				Uart5_Send_string("#005P0850T0800!");
				delay_ms(50);
				//Uart5_Send_string("#003P1300T0800!"); 
				delay_s(1);

				Uart5_Send_string("#002P1070T0800!");
				delay_ms(50);
				
				Uart5_Send_string("#001P0935T0800!");
				delay_ms(50);
				Uart5_Send_string("#003P1145T0800!");
				delay_ms(500);
				
				Uart5_Send_string("#005P1468T0800!");   //��ȡ
				delay_ms(50);
				delay_s(2);
			
				Uart5_Send_string("#002P1100T0800!");   //̧��
				delay_ms(800);
				Uart5_Send_string("#003P1345T0800!");
				delay_ms(800);
				
				Uart5_Send_string("#000P1120T0800!");   //��ת
				delay_ms(800);
				
				
				Uart5_Send_string("#002P0950T0800!");   
				delay_ms(50);
				
				Uart5_Send_string("#003P1370T0800!");
				delay_ms(1000);
				Uart5_Send_string("#003P1490T0400!");
				delay_ms(50);
				Uart5_Send_string("#002P0805T0800!");   
				delay_ms(800);
				
				Uart5_Send_string("#002P0780T0800!");   
				delay_ms(1200);
				Uart5_Send_string("#005P1058T0900!");  //����
				delay_ms(800);
			  Uart5_Send_string("#002P0820T0800!"); 
				delay_ms(800);
				Uart5_Send_string("#002P1100T0800!");   //̧��
				delay_ms(800);
				Uart5_Send_string("#003P1345T0800!");
				delay_ms(800);
			
				flag_Gray_A++;
				while(1);
		}
}

void Rotate_DOWN()
{
				if(board_flag[5]==10 || board_flag[5]==9 || board_flag[5]==8 || board_flag[5]==7 || board_flag[5]==6)
				{
						Uart5_Send_string("#004P0520T1000!");    //����̨  6
						delay_ms(1000);
						Run_motor(180,180);
						if(board_flag[5]==9)                      //ǰ��
								delay_ms(100);
						else if(board_flag[5]==8)                   //ǰ��
								delay_ms(200);
						else if(board_flag[5]==7)                   //ǰ��
								delay_ms(300);
						else if(board_flag[5]==6)                    //ǰ��
								delay_ms(400);
						STOP;	
						Dropt_down();
						delay_ms(1000);
						Run_motor(-180,-180);
						if(board_flag[5]==9)                      
								delay_ms(100);
						else if(board_flag[5]==8)                   
								delay_ms(200);
						else if(board_flag[5]==7)                  
								delay_ms(300);
						else if(board_flag[5]==6)                   
								delay_ms(400);
						STOP;					
				}
			  if(board_flag[4]==9 || board_flag[4]==8 || board_flag[4]==7 || board_flag[4]==6 || board_flag[4]==5)
				{
					  Uart5_Send_string("#004P0520T1000!");    //����̨  5
						delay_ms(1000);
						Run_motor(180,180);
						if(board_flag[4]==9)                   
								delay_ms(100);
						else if(board_flag[4]==8)                     
								delay_ms(200);
						else if(board_flag[4]==7)                
								delay_ms(300);
						else if(board_flag[4]==6)                 
								delay_ms(400);
						if(board_flag[4]!=5)
						{
								STOP;	
								Dropt_down();
						}
						else 
						{
								STOP;	
								Dropt_up();
						}
						delay_ms(1000);
						Run_motor(-180,-180);
						if(board_flag[4]==9)                     
								delay_ms(100);
						else if(board_flag[4]==8)                   
								delay_ms(200);
						else if(board_flag[4]==7)                  
								delay_ms(300);
						else if(board_flag[4]==6)                   
								delay_ms(400);
						STOP;
				}
				if(board_flag[3]==4 || board_flag[3]==8 || board_flag[3]==7 || board_flag[3]==6 || board_flag[3]==5)
				{
					  Uart5_Send_string("#004P0520T1000!");    //����̨  4
						delay_ms(1000);
						Run_motor(180,180);
						if(board_flag[3]==4)                     
								delay_ms(100);
						else if(board_flag[3]==8)                     
								delay_ms(200);
						else if(board_flag[3]==7)                  
								delay_ms(300);
						else if(board_flag[3]==6)                   
								delay_ms(400);
						if(board_flag[3]!=5 && board_flag[3]!=4)
						{
								STOP;	
								Dropt_down();
						}
						else
						{
								STOP;	
								Dropt_up();
						}
						delay_ms(1000);
						Run_motor(-180,-180);
						if(board_flag[3]==4)                      
								delay_ms(100);
						else if(board_flag[3]==8)                      
								delay_ms(200);
						else if(board_flag[3]==7)                   
								delay_ms(300);
						else if(board_flag[3]==6)                   
								delay_ms(400);
						STOP;
				}
		
				if(board_flag[2]==4 || board_flag[2]==3 || board_flag[2]==7 || board_flag[2]==6 || board_flag[2]==5)
				{
					  Uart5_Send_string("#004P0520T1000!");    //����̨  3
						delay_ms(1000);
						Run_motor(180,180);
						if(board_flag[2]==4)                   
								delay_ms(100);
						else if(board_flag[2]==3)                  
								delay_ms(200);
						else if(board_flag[2]==7)                
								delay_ms(300);
						else if(board_flag[2]==6)                  
								delay_ms(400);
						if(board_flag[2]!=5 && board_flag[2]!=4 && board_flag[2]!=3)
						{
								STOP;	
								Dropt_down();
						}
						else
						{
								STOP;	
								Dropt_up();
						}
						delay_ms(1000);
						Run_motor(-180,-180);
						if(board_flag[2]==4)                      
								delay_ms(100);
						else if(board_flag[2]==3)                    
								delay_ms(200);
						else if(board_flag[2]==7)                
								delay_ms(300);
						else if(board_flag[2]==6)                   
								delay_ms(400);
						STOP;
				}
				if(board_flag[1]==4 || board_flag[1]==3 || board_flag[1]==2 || board_flag[1]==6 || board_flag[1]==5)
				{
					  Uart5_Send_string("#004P0520T1000!");    //����̨  2
						delay_ms(1000);
						Run_motor(180,180);
						if(board_flag[1]==4)                    
								delay_ms(100);
						else if(board_flag[1]==3)                      
								delay_ms(200);
						else if(board_flag[1]==2)                   
								delay_ms(300);
						else if(board_flag[1]==6)
								delay_ms(400);
						if(board_flag[1]!=6 && board_flag[1]!=5)
						{
								STOP;	
								Dropt_up();
						}
						else
						{
								STOP;	
								Dropt_down();
						}
						delay_ms(1000);
						Run_motor(-180,-180);
						if(board_flag[1]==4)                     
								delay_ms(100);
						else if(board_flag[1]==3)                      
								delay_ms(200);
						else if(board_flag[1]==2)                  
								delay_ms(300);
						else if(board_flag[1]==6)                  
								delay_ms(400);
						STOP;
				}
				if(board_flag[1]==4 || board_flag[1]==3 || board_flag[1]==2 || board_flag[1]==1 || board_flag[1]==5)
				{
					  Uart5_Send_string("#004P0520T1000!");    //����̨  1
						delay_ms(1000);
						Run_motor(180,180);
						if(board_flag[0]==4)                      
								delay_ms(100);
						else if(board_flag[0]==3)                  
								delay_ms(200);
						else if(board_flag[0]==2)              
								delay_ms(300);
						else if(board_flag[0]==1)                 
								delay_ms(400);
						STOP;	
						Dropt_down();
						delay_ms(1000);
						Run_motor(-180,-180);
						if(board_flag[1]==4)                      
								delay_ms(100);
						else if(board_flag[1]==3)                      
								delay_ms(200);
						else if(board_flag[1]==2)                 
								delay_ms(300);
						else if(board_flag[1]==1)                   
								delay_ms(400);
						STOP;
					}
}
void GO(u8 i)
{
	while(i--)
	{
		PID_run(180);
		delay_ms(17);
	}
}

int main(void)
{
		u8 NUM;    
		flag_NUM=0;
		flag_Gray_A=0;
	  flag_now=0;
		Init();			             //��ʼ��
	  while(!HMI_Receive_over)  //�ȴ�����������ָ��
		{
			Arrange();  
			if(HMI_Receive_over)
			{
				HMI_Receive_over=0;
				break;
			}
		}
		BUZZER_ON;
		delay_ms(700);
		BUZZER_OFF;
		delay_s(1);            //����ѹ
		Uart5_Send_string("#003P1720T0800!");
	while(1)                   //���������ϲ���ֹͣ
	{		
		PID_run(190);
		delay_ms(19);
		GET();        //����
		if(!Gray_STOP_A)
		{	
				delay_ms(8);
				if(!Gray_STOP_A)
				{
					STOP;
					BUZZER_ON;
					delay_ms(700);
					BUZZER_OFF;
					break;	
				}
		}
	};	
	while(1)                   //ɨ��ץȡľ�� �׶�
	{
			if(flag_Gray_A==1)
			{
					
					while(1)
					{
						  if(!(flag_now%2))
							{
									Recognition_1();	         //ǰ��ʶ���� 
									flag_go=0;
							}
							else   
							{								
									Recognition_2();					//����ɨ�趯��
									flag_go++;		
							}
							Judge();							     //ʶ��*/
							if(couch_flag && !flag_go)             //ǰ������  ץȡ����ǰ��	
							{
									Ahold_1();	
									couch_flag=0;								
							}
							else if(couch_flag && flag_go)       //��������  
							{
									Ahold_2();	
									couch_flag=0;																	
							}
							if(flag_go)
							{
										GO(22);  //ǰ��
										//delay_ms(325);
										STOP;			
							}				          		
							if(flag_NUM==6)
									break;	
					}
			}
			if(flag_NUM==6)
						break;
	}
	while(1)   // ���� �׶�
	{	
		PID_run(170);
		delay_ms(22);		
		if(!Gray_BIZHANG)
		{
			delay_ms(10);
			if(!Gray_BIZHANG)
			{	
				STOP;
				BUZZER_ON;
				delay_ms(700);
				BUZZER_OFF;
				
				Avoiding();			
				break;			
			}
		}			
	};		
	while(1)   //���Ϻ� Ѱ�߱�־����
	{		
		PID_run(190);
		delay_ms(20);	
		NUM++;
		if(NUM==30)
		{
			flag_Gray_A=0;
			break;
		}
	};	
	/*	while(1)  //�س̲�������
	{		
		PID_run(190);
		delay_ms(20);	
		GET();           //����
		if(flag_Gray_A==1)
				Rotate_DOWN();
		if(flag_Gray_A==4)
			break;	
	};	*/
    //���Ϻ�ص��յ�		
		BACK_STOP();
		delay_ms(850);
		STOP;
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
*   @}��������ѧԺ ����ѵ������ 
       2018.10.29
  */

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
