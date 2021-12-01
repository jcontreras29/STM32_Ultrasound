// Joseph Contreras
// Grad Project - Ultrasound

#include <string.h>
#include <stdio.h>
#include "stm32l476xx.h"
#include "UART.h"
#include "SysClock.h"
#include "math.h"


uint32_t time = 0;
int measurements[100]; // This is where I will put the readings
uint8_t numReadings = 0;
uint8_t FLAG = 0;
uint32_t runningTotal = 0;
uint32_t  LARGEST = 0;
uint32_t  SMALLEST = 1000;
char Booffer[64];



//
void config_GPIO_pins(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;		// enable clock for A group of GPIO

	// PA0 Pin
	GPIOA->MODER &= ~3;									// clear out bits
	GPIOA->MODER |=  2;									// Enable alternate function mode (binary 10) for PA0

	// PA1 Pin
	GPIOA->MODER &= ~12;
	GPIOA->MODER |= 8;									// Alernate function mode for PA1

	GPIOA->AFR[0] &= ~15;								// Set up AFR Register 2 for TIM2
	GPIOA->AFR[0] |= 1;

	GPIOA->AFR[0] &= ~(15<<4);
	GPIOA->AFR[0] |= (2<<4);
}


void setup_timer2() // Input Capture mode
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;	// enable clock for this timer in the clock control
	TIM2->PSC = 80-1;										// load the prescaler value
	TIM2->ARR = 4294967295;									// 32 bit counter
	TIM2->EGR |= TIM_EGR_UG;							// force an update event to make the prescaler load take effect

	TIM2->CCER &= ~TIM_CCER_CC1E;
	TIM2->CCER &= ~TIM_CCER_CC1NP;		// Clear bits
	TIM2->CCER &= ~TIM_CCER_CC1P;


	TIM2->CCMR1 &= ~3;						// Clear first two bits
	TIM2->CCMR1 |= 1;						// Set to channel 1
	TIM2->CCMR1 &= ~(TIM_CCMR1_IC1F);

	TIM2->CCER |= TIM_CCER_CC1P;
	TIM2->CCER |= TIM_CCER_CC1NP;		// non inverted both edges
	TIM2->CCER |= TIM_CCER_CC1E;
}



void setup_timer5() // PWM
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN; // enable clock

	TIM5->CCER |= TIM_CCER_CC2E;

	TIM5->CCMR1 &= ~TIM_CCMR1_OC2M_0;
	TIM5->CCMR1 &= ~TIM_CCMR1_OC2M_1;
	TIM5->CCMR1 &= ~TIM_CCMR1_OC2M_2;

	TIM5->CCMR1 |= TIM_CCMR1_OC2M_1;
	TIM5->CCMR1 |= TIM_CCMR1_OC2M_2;
	TIM5->CCMR1 |= TIM_CCMR1_OC2PE;

	TIM5->CR1 |= TIM_CR1_ARPE;

	TIM5->PSC = 80-1;
	TIM5->EGR |= TIM_EGR_UG;
	TIM5->ARR = 20;
	TIM5->CCR2 = 10;

	TIM5->EGR |= TIM_EGR_UG;

}
void start_timer2(){
	TIM2->CR1 |= TIM_CR1_CEN;
}
void start_timer5(){
	TIM5->CR1 |= TIM_CR1_CEN;
}
void stop_timer2(){
	TIM2->CR1 &= ~TIM_CR1_CEN;
}
void stop_timer5(){
	TIM5->CR1 &= ~TIM_CR1_CEN;
}
void reset_timer2(){
	TIM2->CNT = 0;
}
void reset_timer5(){
	TIM5->CNT = 0;
}
int get_time2(){
	return TIM2->CNT;
}
int get_time5(){
	return TIM5->CNT;
}
void update_dutycycle5(x){
	TIM5->CCR2 = x;
}

void sendWave()
{
	start_timer5();
	while (1)
	{
		if (get_time5() >= 12)
		{
			stop_timer5();
			reset_timer5();
			break;
		}
	}
	start_timer2();
}

//Finds distance between rising edge and falling edge of return wave length
int calc_time()
{
	int x;
	int y;
	while((TIM2->SR & TIM_SR_CC1IF) == 0){}
		x = TIM2->CCR1;
	while((TIM2->SR & TIM_SR_CC1IF) == 0)
	{
		if (abs((get_time2() - x >= 180000)))
			return 180000;
	}
	y = TIM2->CCR1;
	time = abs(y-x);
	stop_timer2();
	reset_timer2();
	return (int)time;
}

void wait10()
{
	start_timer2();
	while(1)
	{
		int x = get_time2();
		if (x >= 100000)
			break;
	}
	stop_timer2();
	reset_timer2();
}


int calc_distance(time)
{
	// centimeters
	float cm = time/58;
	//cm = floor(cm);
	return (int)(cm*10); //millimeters
}

int sendReceive()
{
	sendWave();
	int microseconds = calc_time();
	int millimeters = calc_distance(microseconds);
	wait10();
	return millimeters;
}

void POST()
{
	//USART_Write(USART2, (uint8_t *)"\e[2J\e[1;1HPOST?", 21);
	int n = sprintf((char *)Booffer, "\e[2J\e[1;1HPOST?\r\n");
	USART_Write(USART2, Booffer, n);
	char rxByte;
	while(1)
	{
		rxByte = USART_Read(USART2);
		if (rxByte == '\r')
		{
			//USART_Write(USART2, (uint8_t *)"Work\r\n", 13);
			int x = sendReceive();
			if (x >= 50 && x <= 1000)
			{
				n = sprintf((char *)Booffer, "Distance: %d mm\r\nSuccess!\r\n", x);
				USART_Write(USART2, Booffer, n);
				break;
			}
			else
			{
				n = sprintf((char *)Booffer, "Distance: *** \r\nFailed!\r\n");
				USART_Write(USART2, Booffer, n);
			}

		}
		else
		{
			n = sprintf((char *)Booffer, "Press Enter\r\n");
			USART_Write(USART2, Booffer, n);
		}
	}
}

void hunnitReadings() // one hundred readings sir
{
	for(int i = 0; i <= 100; i++)
		measurements[i] = 0;
	int x;
	int n = sprintf((char *)Booffer, "Would you like to run 100 times? \r\n");
	USART_Write(USART2, Booffer, n);
	char rxByte;
	while(1)
	{
		rxByte = USART_Read(USART2);
		if (rxByte == '\r')
		{
			n = sprintf((char *)Booffer, "Loading...\r\n");
			USART_Write(USART2, Booffer, n);
			for (int i=0; i<100; i++)
			{
				if (USART_KeyPress(USART2) == 1) // stops with a key press
					break;
				x = sendReceive();
				measurements[i] = x;
				numReadings++;
			}
			break;
		}
		else
		{
			n = sprintf((char *)Booffer, "Press Enter\r\n");
			USART_Write(USART2, Booffer, n);
		}
	}
}

void printReadings()
{
	short tabs = 0;
	int n = sprintf((char *)Booffer,"\r\n");
	USART_Write(USART2, Booffer, n);
	for (int i=0; i<numReadings; i++)
	{
		if (tabs == 9)
		{
			n = sprintf((char *)Booffer,"\r\n");
			USART_Write(USART2, Booffer, n);
			tabs = 0;
		}
		if (measurements[i] >= 50 && measurements[i] <= 1000)
		{
			n = sprintf((char *)Booffer,"%d\t", measurements[i]);
			USART_Write(USART2, Booffer, n);
			tabs++;
			runningTotal += measurements[i];
			if (measurements[i] < SMALLEST)
				SMALLEST = measurements[i];
			if (measurements[i] > LARGEST)
				LARGEST = measurements[i];
		}
		else
		{
			if (tabs == 9)
			{
				n = sprintf((char *)Booffer,"\r\n");
				USART_Write(USART2, Booffer, n);
				tabs = 0;
			}
			n = sprintf((char *)Booffer,"***\t");
			USART_Write(USART2, Booffer, n);
		}
	}
	n = sprintf((char *)Booffer,"\r\n\r\n");
	USART_Write(USART2, Booffer, n);

	// Print the stats here too
	int average = (int)runningTotal/numReadings;

	n = sprintf((char *)Booffer,"Average -> %d\r\n",average);
	USART_Write(USART2, Booffer, n);

	n = sprintf((char *)Booffer,"Minimum -> %d\r\n",SMALLEST);
	USART_Write(USART2, Booffer, n);

	n = sprintf((char *)Booffer,"Maximum -> %d\r\n",LARGEST);
	USART_Write(USART2, Booffer, n);

	n = sprintf((char *)Booffer,"\r\nPress a key to clear screen..");
	USART_Write(USART2, Booffer, n);

	while (USART_KeyPress(USART2) == 0){}
	n = sprintf((char *)Booffer,"\e[2J\e[1;1H");
	USART_Write(USART2, Booffer, n);

	numReadings = 0;
	SMALLEST = 1000;
	LARGEST = 0;
	runningTotal = 0;
}
