#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "Encoder.h"
#include "CountSensor.h"

int16_t Speed = 0;

typedef struct
{
    uint16_t mode;
    uint16_t value;
} user_info;
user_info info = { 0, 0 };

int main(void)
{
    OLED_Init();
    Timer_Init();
    Encoder_Init(); //编码器初始化
    CountSensor_Init();		//计数传感器初始化

    OLED_ShowString(1, 1, "Speed:");
    while (1)
    {
        OLED_ShowSignedNum(1, 7, Speed, 5);
        OLED_ShowString(2, 1, "Mode:");
        OLED_ShowNum(2, 7, CountSensor_Get(), 1);
        OLED_ShowString(3, 1, "value:");
        OLED_ShowNum(3, 8, info.value, 3);

    }
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        Speed = Encoder_Get();
        if (Speed > 0)
        {
            if (Speed + info.value > 100)
            {
                info.value = 100;
            }
            else
            {
                info.value += Speed;
            }


        }
        else if (Speed < 0)
        {
            if (info.value + Speed < 0)
            {
                info.value = 0;
            }
            else
            {
                info.value += Speed;
            }
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
