#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "Encoder.h"
#include "CountSensor.h"
#include "MyRTC.h"
#include "PWM.h"
#include "Bigled.h"
#include "Motor.h"
#include <time.h>
#include "Serial.h"
#include <string.h>

int16_t Speed = 0;
int16_t count_past = 0;

typedef struct
{
    uint16_t mode;
    uint16_t value[10];
    uint16_t ALarm[6];
    time_t ALarm_cnt;
    uint32_t AL_flag; //用于判断是否是闹钟引发的唤醒
} user_info;
user_info info = {.ALarm = {2023, 1, 2, 0, 2, 55}};
time_t dec_time;//闹钟开始时间和当前时间的差值

int main(void)
{
    //模块初始化
    OLED_Init();
    Timer_Init();
    Encoder_Init(); //编码器初始化
    CountSensor_Init();		//计数传感器初始化
    MyRTC_Init();		//RTC初始化
    PWM_Init();			//PWM初始化
    Motor_Init();        //电机控制引脚
    Serial_Init();		//串口初始化
    
    OLED_ShowString(1, 1, "XX:XX:XX/XX-XXXX"); //时间显示
    OLED_ShowString(4, 1, "XX:XX:XX/XX-XXXX"); //闹钟显示
    while (1)
    {
        MyRTC_ReadTime();
        
        //oled时间显示任务
        OLED_ShowNum(1, 1, MyRTC_Time[3], 2);		//时
		OLED_ShowNum(1, 4, MyRTC_Time[4], 2);		//分
		OLED_ShowNum(1, 7, MyRTC_Time[5], 2);		//秒
        
        OLED_ShowNum(1, 10, MyRTC_Time[0], 2);		//显示MyRTC_Time数组中的时间值，年
		OLED_ShowNum(1, 13, MyRTC_Time[1], 2);		//月
		OLED_ShowNum(1, 15, MyRTC_Time[2], 2);		//日
        
        //oled闹钟显示任务
        OLED_ShowNum(4, 1, info.ALarm[3], 2);		//时
		OLED_ShowNum(4, 4, info.ALarm[4], 2);		//分
		OLED_ShowNum(4, 7, info.ALarm[5], 2);		//秒
        
        OLED_ShowNum(4, 10, info.ALarm[0], 2);		//显示MyRTC_Time数组中的时间值，年
		OLED_ShowNum(4, 13, info.ALarm[1], 2);		//月
		OLED_ShowNum(4, 15, info.ALarm[2], 2);		//日
        
        //oled模式和值显示任务
        OLED_ShowString(2, 1, "Mode:");
        if (count_past != CountSensor_Get() )
        {
            count_past  = CountSensor_Get();
            info.mode ++;
            info.mode %= 5;
        }
        OLED_ShowNum(2, 7,info.mode , 1);
        OLED_ShowString(3, 1, "value:");
        OLED_ShowNum(3, 8, info.value[info.mode], 3);
        
        //oled模式和值对应器件任务
        //bigled mode == 1
        if (info.mode == 1)
        {
            Bigled_Set(info.value[info.mode]);
        }
        else if (info.mode == 0 || info.mode == 3 || info.mode == 2)
        {
            Bigled_Set(0);
        }
        
        
        //Motor mode == 2
        if (info.mode == 2)
        {
            Motor_SetSpeed(info.value[info.mode]);
        }
        else if (info.mode == 0 || info.mode == 1 || info.mode == 3)
        {
            Motor_SetSpeed(0);
        }
        
        //Water mode == 3
        if (info.mode == 3)
        {
            Water_SetSpeed(info.value[info.mode]);
        }
        else if (info.mode == 0 || info.mode == 1 || info.mode == 2)
        {
            Water_SetSpeed(0);
        }
        
        //唤醒程序 mode 4
        if (info.mode == 4)
        {
            if (info.AL_flag == 0 && info.value[info.mode] > 50) //如果不是闹钟执行的唤醒程序 演示唤醒
            {   
                Water_SetSpeed(50);
                Motor_SetSpeed(50);
                Bigled_Set(30);
            }
            else if (info.AL_flag == 0 && info.value[info.mode] < 50) // 演示唤醒
            {   
                Water_SetSpeed(0);
                Motor_SetSpeed(0);
                Bigled_Set(0);
            }
            else if (info.AL_flag == 1 )//是闹钟触发的时间记录和条件开始
            {
                
                
                info.ALarm_cnt = RTC_GetCounter();//记录下当前的闹钟cnt
                info.AL_flag = 2;//到闹钟唤醒状态
                    
                
            }
            else if (info.AL_flag == 2 && info.value[info.mode] > 50)//是闹钟唤醒
            {
                dec_time = RTC_GetCounter() - info.ALarm_cnt; //闹钟响了多久了
                //50秒内
                if (dec_time < 50)
                {
                    //亮灯
                    Water_SetSpeed(0);
                Motor_SetSpeed(0);
                Bigled_Set(dec_time);
                }
                else if (dec_time < 70)
                {
                    //吹风
                    Water_SetSpeed(0);
                Motor_SetSpeed(dec_time);
                Bigled_Set(50);
                }
                else
                {
                    //喷水
                    Water_SetSpeed(70);
                    Motor_SetSpeed(70);
                    Bigled_Set(50);
                }
                    
                
            }
            else if (info.AL_flag == 2 && info.value[info.mode] < 50) // 演示唤醒
            {   
                info.AL_flag = 0;
            }
            
                
        }
        
        
        //串口数据部分
        if (Serial_GetRxFlag() == 1)	//如果接收到数据包
		{   
            
            
            //串口数据处理
            //时间
            //0年 1年 2月 3日 4时 5分 6秒
			MyRTC_Time[0] = (Serial_RxPacket[0] % 16 + (Serial_RxPacket[0] / 16) * 10) * 100 + (Serial_RxPacket[1] % 16 + (Serial_RxPacket[1] / 16) * 10);
            MyRTC_Time[1] = (Serial_RxPacket[2] % 16 + (Serial_RxPacket[2] / 16) * 10);
            MyRTC_Time[2] = (Serial_RxPacket[3] % 16 + (Serial_RxPacket[3] / 16) * 10);
            MyRTC_Time[3] = (Serial_RxPacket[4] % 16 + (Serial_RxPacket[4] / 16) * 10);
            MyRTC_Time[4] = (Serial_RxPacket[5] % 16 + (Serial_RxPacket[5] / 16) * 10);
            MyRTC_Time[5] = (Serial_RxPacket[6] % 16 + (Serial_RxPacket[6] / 16) * 10);
            MyRTC_SetTime();
            //闹钟
            info.ALarm[0] = (Serial_RxPacket[0 + 7] % 16 + (Serial_RxPacket[0 + 7] / 16) * 10) * 100 + (Serial_RxPacket[1 + 7] % 16 + (Serial_RxPacket[1 + 7] / 16) * 10);
            info.ALarm[1] = Serial_RxPacket[2 + 7] % 16 + (Serial_RxPacket[2 + 7] / 16) * 10 ;
            info.ALarm[2] = Serial_RxPacket[3 + 7] % 16 + (Serial_RxPacket[3 + 7] / 16) * 10 ; 
            info.ALarm[3] = Serial_RxPacket[4 + 7] % 16 + (Serial_RxPacket[4 + 7] / 16) * 10 ;
            info.ALarm[4] = Serial_RxPacket[5 + 7] % 16 + (Serial_RxPacket[5 + 7] / 16) * 10 ;
            info.ALarm[5] = (Serial_RxPacket[6 + 7] % 16 + (Serial_RxPacket[6 + 7] / 16) * 10) ;
            info.AL_flag = 1;
            if (Serial_RxPacket[14] != 0) //给0为自由模式，不给0不自由
            {
                info.mode = Serial_RxPacket[14] % 16;
                info.value[info.mode] = Serial_RxPacket[15] % 16 + (Serial_RxPacket[15] / 16) * 10;
                //info.value[info.mode] = MyRTC_Time[0];
            }
            //串口数据回显
            memcpy(Serial_TxPacket, Serial_RxPacket, 18);
           
            Serial_SendPacket();
		}
        
        //时间判断
        if (info.AL_flag == 1)
        {   
            int bj_i = 0;
            int bj_flag = 0;
            for (bj_i = 0; bj_i < 6;bj_i ++)
            {
                if (MyRTC_Time[bj_i] == info.ALarm[bj_i])
                {
                    bj_flag ++;
                }
                 
            }
            if (bj_flag == 6)
            {
                info.mode = 4;
                info.value[info.mode] = 100;
            }
        }

    }
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        Speed = Encoder_Get();
        if (Speed > 0)
        {
            if (Speed + info.value[info.mode] > 100)
            {
                info.value[info.mode] = 100;
            }
            else
            {
                info.value[info.mode] += Speed;
            }


        }
        else if (Speed < 0)
        {
            if (info.value[info.mode] + Speed < 0)
            {
                info.value[info.mode] = 0;
            }
            else
            {
                info.value[info.mode] += Speed;
            }
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
