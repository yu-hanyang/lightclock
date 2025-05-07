#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "Encoder.h"
#include "CountSensor.h"
#include "MyRTC.h"

int16_t Speed = 0;
int16_t count_past = 0;

typedef struct
{
    uint16_t mode;
    int16_t value[10];
} user_info;
user_info info = { 0, 0 };

int main(void)
{
    //模块初始化
    OLED_Init();
    Timer_Init();
    Encoder_Init(); //编码器初始化
    CountSensor_Init();		//计数传感器初始化
    MyRTC_Init();		//RTC初始化
    
    
    OLED_ShowString(1, 1, "XX:XX:XX/XX-XXXX"); //时间显示
    
    while (1)
    {
        MyRTC_ReadTime();
        
        OLED_ShowNum(1, 1, MyRTC_Time[3], 2);		//时
		OLED_ShowNum(1, 4, MyRTC_Time[4], 2);		//分
		OLED_ShowNum(1, 7, MyRTC_Time[5], 2);		//秒
        
        OLED_ShowNum(1, 10, MyRTC_Time[0], 2);		//显示MyRTC_Time数组中的时间值，年
		OLED_ShowNum(1, 13, MyRTC_Time[1], 2);		//月
		OLED_ShowNum(1, 15, MyRTC_Time[2], 2);		//日
        
        
        
        OLED_ShowString(2, 1, "Mode:");
        if (count_past != CountSensor_Get() )
        {
            count_past  = CountSensor_Get();
            info.mode ++;
            info.mode %= 10;
        }
        OLED_ShowNum(2, 7,info.mode , 1);
        OLED_ShowString(3, 1, "value:");
        OLED_ShowNum(3, 8, info.value[info.mode], 3);

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
