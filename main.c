#include "public.h"

/**
* @}绍兴文理学院 工程训练竞赛 
    电气161  王家宝 第三版改
		2018.10.29
  */
#define START_SHIBIE Send_string(start_shibie,9,UART_2)  //Send_string(start_shibie,9,UART_2)  //识别

uint8 flag_NUM;
uint8 flag_Gray_A;
uint8 flag_now;
uint8 flag_go;

u8 shibie_over=0;//shangpo_over=0;
u8 HMI_Receive_over=0;


/*		Uart5_Send_string("#004P0520T0800!");    载物台  1


*/
void uart_init(u32 pclk2,u32 bound);

char WuLiao[2]={0};

char start_shibie[9]={0x7E,0x00,0x08,0x01,0x00,0x02,0x01,0xAB,0xCD};  //识别指令
char board_1[10]={0};       //串口屏接收 ，抓取的物块
char board_2[6]={0};        //串口屏接收 ，抓取的物块整合
char board_flag[6]={0X31};     //标记抓取的物块
 
bool couch_flag = 0;        //是否吻合标志
bool Choose_flag = 0;       //HMI是否选中

void  Start_clock()  //时钟初始化和外设时钟的开启
{
		uint8  HSEStartUpStatus;   
		RCC_DeInit();    //RCC 复位
		RCC_HSEConfig(RCC_HSE_ON); //开启外部时钟 并执行初始化
		HSEStartUpStatus = RCC_WaitForHSEStartUp();	//等待外部时钟准备好
		while(HSEStartUpStatus == ERROR);	  //启动失败 在这里等待
	
		RCC_HCLKConfig(RCC_SYSCLK_Div1);		//设置内部总线时钟
		RCC_PCLK1Config(RCC_HCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);//外部时钟为8M 这里倍频到72M
	
		RCC_PLLCmd(ENABLE); 
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);  //等待PLL配置就绪 
	
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);       
		while(RCC_GetSYSCLKSource() != 0x08);       		//切换系统时钟为PLL，等待完成
	
		RCC_HSICmd(DISABLE);			         //关闭内部时钟HSI
	
	  ///////******************时钟开启******************/////
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);   							//TIM5,TIM3的时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_UART4|RCC_APB1Periph_UART5,ENABLE);  //开启串口时钟	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE); //GPIO的时钟
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);   //DMA的时钟
}


void test()
{
		while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)==1);
		TIM5->CCR3=150;
		TIM5->CCR4=150;
		TIM5->CCR2=150;
}

void Init()         //初始化
{
	//	GPIO_InitTypeDef GPIO_InitStructure;
	Start_clock();
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);           //抢占为0-1，响应为0-7
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);   //映射引脚

	GPIO_PWM_INIT();      //PWM_GPIO初始化
	TIM1_PWM_INIT();      //PWM初始化
	GRAY_GPIO_INIT();     //灰度条初始化
	GPIO_Gray_INIT();	    //单灰度（红外）初始化
	UART_GPIO_INIT();     //串口GPIO初始化
	UART2_INIT();         //扫描模块初始化
	UART5_INIT();         //机械臂初始化
	UART4_INIT();         //串口屏初始化
	USART_Cmd(USART2, ENABLE);//扫描模块使能
	USART_Cmd(UART5, ENABLE);	//串口屏使能	
	delay_ms(1000);				//延1s

	Uart5_Send_string("#000P0520T0800!");
	delay_ms(50);
	Uart5_Send_string("#001P1170T0800!");
	delay_ms(50);
	Uart5_Send_string("#002P0750T0800!");   //减小向上
	delay_ms(50);
	Uart5_Send_string("#003P1955T0800!");
	delay_ms(50);
	Uart5_Send_string("#004P0520T0500!");
	delay_ms(50);
	Uart5_Send_string("#005P0900T0800!");      //0800张开
	delay_ms(50);                        //初始动作*/
	BUZZER_ON;
	delay_ms(700);
	BUZZER_OFF;
}

void GET()          //捕捉线
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
void Avoiding()     //避障
{
				Run_motor(-200,-200);  //退
				delay_ms(400);
				STOP;
				
				Run_motor(-300,300); 	    //左转
				delay_s(1);
				delay_ms(200);
				STOP;
				
				Run_motor(300,300);      //直走  
				delay_ms(700);
				STOP;
				
				Run_motor(300,-300);     //右转
				delay_s(1);
				delay_ms(310);
				STOP;
				/*
				Run_motor(300,300);      //直走
				delay_s(1);
				delay_ms(80);
				STOP;
				
				Run_motor(300,-300);     //右转
				delay_s(1);
				delay_ms(300);
				STOP;*/
				turn_left();
				
				Run_motor(300,300);      //直走  
				delay_ms(150);
			
				STOP;
				
				Run_motor(-300,300); 	    //左转
				delay_s(1);
				delay_ms(200);
			//	delay_ms(1050);
				STOP;	

}
void Arrange()      //将木块与载物台匹配
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
void Rotate_UP()     //载物台装货动作
{
	if(WuLiao[1]==board_flag[0])
		Uart5_Send_string("#004P0570T0800!");    //载物台  1
	else if(WuLiao[1]==board_flag[1])
		Uart5_Send_string("#004P0940T0800!");    //载物台  2
	else if(WuLiao[1]==board_flag[2])
		Uart5_Send_string("#004P1380T0800!");    //载物台  3
	else if(WuLiao[1]==board_flag[3])
		Uart5_Send_string("#004P1850T0800!");    //载物台  4
	else if(WuLiao[1]==board_flag[4])
		Uart5_Send_string("#004P2310T0800!");    //载物台  5
	//else if(WuLiao[1]==board_flag[5])
	//	Uart5_Send_string("#004P1520T0800!");    //载物台  6
}
void Judge()         //扫描判断  
{
	static  u8 n=0;
	START_SHIBIE;
	delay_ms(1000);
	flag_now++;
	if((WuLiao[1]==board_flag[0]) || (WuLiao[1]==board_flag[1]) || (WuLiao[1]==board_flag[2]))   //核对
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
void Recognition_1() //前排扫描动作
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
void Recognition_2() //后排扫描动作
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
void Ahold_1()       //抓取动作1 （前排）
{   
	
		Uart5_Send_string("#000P1120T0800!");   //开始抓取
		delay_ms(500);
		Uart5_Send_string("#001P1450T0800!");
		delay_ms(100);
		Uart5_Send_string("#002P1280T0800!");
		delay_ms(100);
		Uart5_Send_string("#003P1355T0800!");
		delay_ms(300);
		Uart5_Send_string("#005P1465T0800!");
		delay_ms(100);                           //前排抓取动作结束
			
		delay_s(2);  
		
		Uart5_Send_string("#002P1250T0500!");//抬臂
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
		Uart5_Send_string("#003P1300T0500!");  //装车
		delay_ms(500);		
		couch_flag=0;	
		flag_NUM++;
}

void Ahold_2()       //抓取动作2 （后排）
{
		Uart5_Send_string("#005P0940T0800!");
		delay_ms(100); 
	
		Uart5_Send_string("#000P1120T0800!");   //开始抓取
		delay_ms(500);
		Uart5_Send_string("#001P1640T1000!");
		delay_ms(100);
		Uart5_Send_string("#003P1575T0500!");
		delay_ms(50);
		Uart5_Send_string("#002P1280T0500!");
		delay_ms(800);

		Uart5_Send_string("#005P1465T0800!");
		delay_ms(100);                           //前排抓取动作结束
			
		delay_s(2);  
		
		Uart5_Send_string("#003P1600T0800!");//抬臂
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
		Uart5_Send_string("#003P1300T0500!");  //装车		
		delay_ms(500);		
		couch_flag=0;	
		flag_NUM++;
}

void Dropt_up()      //下料动作1 （上层）
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
				
				Uart5_Send_string("#005P1460T0800!");   //自取
				delay_ms(50);
				delay_s(2);
			
				Uart5_Send_string("#002P1100T0800!");   //抬臂
				delay_ms(800);
				Uart5_Send_string("#003P1570T0800!");
				delay_ms(800);
				
				Uart5_Send_string("#000P1110T0800!");
				delay_ms(800);
				
				Uart5_Send_string("#002P0890T0800!");   //抬臂
				delay_ms(1000);
				
				Uart5_Send_string("#001P1030T0800!");
				delay_ms(800);
				Uart5_Send_string("#005P1058T0800!");  //放手
				delay_ms(800);
				Uart5_Send_string("#001P0935T0800!");
				
				flag_Gray_A++;
				while(1);
		}
}
void Dropt_down()    //下料动作2 （下层）
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
				
				Uart5_Send_string("#005P1468T0800!");   //自取
				delay_ms(50);
				delay_s(2);
			
				Uart5_Send_string("#002P1100T0800!");   //抬臂
				delay_ms(800);
				Uart5_Send_string("#003P1345T0800!");
				delay_ms(800);
				
				Uart5_Send_string("#000P1120T0800!");   //旋转
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
				Uart5_Send_string("#005P1058T0900!");  //放手
				delay_ms(800);
			  Uart5_Send_string("#002P0820T0800!"); 
				delay_ms(800);
				Uart5_Send_string("#002P1100T0800!");   //抬臂
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
						Uart5_Send_string("#004P0520T1000!");    //载物台  6
						delay_ms(1000);
						Run_motor(180,180);
						if(board_flag[5]==9)                      //前进
								delay_ms(100);
						else if(board_flag[5]==8)                   //前进
								delay_ms(200);
						else if(board_flag[5]==7)                   //前进
								delay_ms(300);
						else if(board_flag[5]==6)                    //前进
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
					  Uart5_Send_string("#004P0520T1000!");    //载物台  5
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
					  Uart5_Send_string("#004P0520T1000!");    //载物台  4
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
					  Uart5_Send_string("#004P0520T1000!");    //载物台  3
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
					  Uart5_Send_string("#004P0520T1000!");    //载物台  2
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
					  Uart5_Send_string("#004P0520T1000!");    //载物台  1
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
		Init();			             //初始化
	  while(!HMI_Receive_over)  //等待串口屏发送指令
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
		delay_s(1);            //待稳压
		Uart5_Send_string("#003P1720T0800!");
	while(1)                   //出发到坡上测线停止
	{		
		PID_run(190);
		delay_ms(19);
		GET();        //测线
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
	while(1)                   //扫描抓取木块 阶段
	{
			if(flag_Gray_A==1)
			{
					
					while(1)
					{
						  if(!(flag_now%2))
							{
									Recognition_1();	         //前排识别动作 
									flag_go=0;
							}
							else   
							{								
									Recognition_2();					//后排扫描动作
									flag_go++;		
							}
							Judge();							     //识别*/
							if(couch_flag && !flag_go)             //前排若是  抓取动作前排	
							{
									Ahold_1();	
									couch_flag=0;								
							}
							else if(couch_flag && flag_go)       //后排若否  
							{
									Ahold_2();	
									couch_flag=0;																	
							}
							if(flag_go)
							{
										GO(22);  //前进
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
	while(1)   // 避障 阶段
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
	while(1)   //避障后 寻线标志清零
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
	/*	while(1)  //回程测线下料
	{		
		PID_run(190);
		delay_ms(20);	
		GET();           //测线
		if(flag_Gray_A==1)
				Rotate_DOWN();
		if(flag_Gray_A==4)
			break;	
	};	*/
    //下料后回到终点		
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
*   @}绍兴文理学院 工程训练竞赛 
       2018.10.29
  */

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
