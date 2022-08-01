#include "stm32f10x.h"                  // Device header
#include "usart.h"

#define FRAMELENGTH 5
#define SUCCESS 1
#define ERROR 0

uint8_t state = ERROR;
uint8_t Buffer[FRAMELENGTH]={0};

void usart_init(uint32_t BaudRate)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = BaudRate;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);
}

void getTargetData(void)
{
		
}

void sendCommand(uint8_t command)
{
	USART_SendData(USART1, command);
}

/**
  * @brief  得到数据（将数据写入数组）
  * @param  data写入的数据
  * @retval 无
 **/
void writeBuffer(uint8_t data)
{
	static uint8_t num=0;
	
	if(state==ERROR)
		num=0;
	
	Buffer[num]=data;
	
	if(num==0){
		if(Buffer[0]==0xA5)
			state=SUCCESS;
		else
			state=ERROR;
	}
	else if(num==FRAMELENGTH-1){
		if(Buffer[FRAMELENGTH-1]==0x5A)
			state=SUCCESS;
		else
			state=ERROR;
	}
	num++;
}

void USART1_IRQHandler()
{
	uint8_t data;
	if(USART_GetITStatus(USART1,USART_IT_RXNE))										//获取中断标志位
	{
		data=USART_ReceiveData(USART1);												//接收数据
//		if(command == READY){
			writeBuffer(data);
			if(state == SUCCESS){
				getTargetData();
			}
//		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}
