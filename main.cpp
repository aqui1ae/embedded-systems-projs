/*
 *********************************************************************************************************
 *
 *                        (c) Copyright 2015-2020 RedPine Signals Inc. HYDERABAD, INDIA
 *                                            All rights reserved.
 *
 *               This file is protected by international copyright laws. This file can only be used in
 *               accordance with a license and should not be redistributed in any way without written
 *               permission by Redpine Signals.
 *
 *                                            www.redpinesignals.com
 *
 *********************************************************************************************************
 */
 /*
 *********************************************************************************************************
 *
 *											  main file
 *
 * File           : main.cpp
 * Programmer(s)  :
 * Description    : IR interrupt
 *********************************************************************************************************
 * Note(s)		 :
 *********************************************************************************************************
 */
 
 /*
 * Includes
 */
#include <WyzBee_gpio.h>
#include <WyzBee_ext.h>
#include <WyzBee_timer.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1351.h"
#include <SPI_OLED.h>
#include <WyzBee_spi.h>
#include <stdio.h>
#include <WyzBee_kit.h>
#include <WyzBee.h>

extern Adafruit_SSD1351 oled = Adafruit_SSD1351(); //@  OLED class variable

/*TODO: Insert function to print to OLED display (Lab1).*/

/*IR receiver call back.
Let the blue LED blink.*/
void Main_ExtIntCallback1()
{
	static uint8 flag;

	if(!flag)
	{
		WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_LOW);
		flag=1;
	}
	else
	{
		WyzBeeGpio_Put(GPIO_LED4,GPIO_HIGH);
		flag =0;
	}
}

int main(void)
{
	WyzBee_exint_config_t WyzBeeExtIntConfig;
	uint8_t ext_port = 2;
	
	//setup an IR interrupt
	WyzBee_PDL_ZERO(WyzBeeExtIntConfig);
	WyzBeeExtIntConfig.abEnable[ext_port] = FALSE;   // INT2
	WyzBeeExtIntConfig.aenLevel[ext_port] = ExIntRisingEdge;
	WyzBeeExtIntConfig.apfnExintCallback[ext_port] = &Main_ExtIntCallback1;
	WyzBee_Exint_IR_Init(&WyzBeeExtIntConfig);
	WyzBee_Exint_EnableChannel(ext_port);

	/*TODO: Initialize the OLED display according to Lab1.*/
	
	/*TODO: Setup a timer according to the TIMER example on the Wyzbee webpage.*/

	while(1)
	{}

}
/*
 *********************************************************************************************************
 *                                           END
 *********************************************************************************************************
 */
