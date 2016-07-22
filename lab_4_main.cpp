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
//#include <WyzBee_kit.h>
#include <WyzBee.h>
#include <stdio.h>
#include <math.h>
//#include <WyzBee_bt.h>
#include <string.h>
#include <rsi_global.h>
#include <rsi_api.h>
#include <WyzBeeWiFi.h>
#include <variant.h>
#include <delay.h>

WyzBeeSpi_Config_t config_stc={									//configuring screen configuration
		4000000,
		SpiMaster,
		SpiSyncWaitZero,
		SpiEightBits,
		SpiMsbBitFirst,
		SpiClockMarkHigh,
		SpiNoUseIntrmode,
		NULL,                                                                
		NULL
};

void printer(char** name,int strLen, int color){
	Adafruit_SSD1351 myOled = Adafruit_SSD1351();//Constructor																
	myOled.begin(); //@ OLED screen Initialization																						
	myOled.fillScreen(RED); //@ fills the OLED screen with black pixels
	myOled.setTextSize(1); //@ Set the text size on the OLED
	myOled.setTextColor(color); // set text color

	for (int j = 0; j<strLen; j++) {																														
		for (int i = 0; i<strlen(name[j]);i++) {
			myOled.setCursor(8*i, 2+10*j); //@ Set the print cursor to the initial location
			myOled.write(name[j][i]); //@ Write a character to the screen
		}
	}
	return;
}

volatile uint8 fflag = 0;

int main(void)
{

	for(int i=0; i<10000000; i++){}
		
	char* name[1];
	
	char ar1[] = "Bruce";
	name[0] = ar1;	
	
	WyzBeeSpi_Init(&config_stc);
	printer(name, 1, BLACK);
		
	for(int i=0; i<10000000; i++){}
	
	sys_ticks_init();	
	HttpRequest HR;
	uint8 my_p_headers[] = "Superheroes";
	uint8 my_p_data[] = "Encrypted Files";
	uint8 my_p_username[] = "Batman";
	uint8 my_p_password[] = "Batcomputer";
	HR.p_data = my_p_data;
	HR.p_username = my_p_username;
	HR.p_password = my_p_password;
	HR.p_headers = my_p_headers;
	
	WyzBeeWiFi_Init();
	
	int8 ssid[] = "ucd-guest"; 	//"ATT762"
	const int8* passphrase = "NULL";	 //"8184165604"
	

	WyzBeeWiFi_ConnectAccessPoint(ssid, passphrase);
	uint8 status = WyzBeeWiFi_status();
		
	int8 p_post_url[] = "http://calbeedemo.appspot.com/greetings?msg=";
	int8 p_get_url[] = "http://calbeedemo.appspot.com/status";
	
	//WyzBeeWiFi_HttpPost(p_post_url, (HttpRequest*)&HR);
	
	int8 p_resp[1000];
	uint16 resp_size = 10000;
	//WyzBeeWiFi_HttpGet(p_get_url, &HR, p_resp, resp_size);
	
	char ar2[25];
	sprintf(ar2, "%d", status); 
	name[0] = ar2;
	
	for(int i=0; i<10000000; i++){}
	printer(name, 1, BLUE);

		
}
/*
 *********************************************************************************************************
 *                                           END
 *********************************************************************************************************
 */
