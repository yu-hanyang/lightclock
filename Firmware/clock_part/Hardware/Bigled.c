#include "Bigled.h"                  // Device header

void Bigled_Set(int8_t value)
{
    PWM_SetCompare1(value);
}
