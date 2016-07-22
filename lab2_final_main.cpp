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

//extern Adafruit_SSD1351 myOled = Adafruit_SSD1351(); //@  OLED class variable

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
			myOled.setCursor(10*i, 2+10*j); //@ Set the print cursor to the initial location
			myOled.write(name[j][i]); //@ Write a character to the screen
		}
	}
	return;
}

//Global variables
uint8 flag;
uint32_t times[50];
int times_index=0;
uint32_t now;
uint32_t last = 0xFFFFFFFF;
uint32_t difference=0;
int ns[34];
int whichButton = -9;

// Configuration for channel 0 - this is configuring timer for future use
static const stc_dt_channel_config_t stcDtChannelConfig0 = { 
DtPeriodic, // Periodic mode 
DtPrescalerDiv256, // Prescaler dividor f/256
DtCounterSize32 // 32bits counter size
}; 

void Main_ExtIntCallback1()
{
	
	now = Dt_ReadCurCntVal(Dt_Channel0); //get time for current rising edge
	
	difference = last-now;
	times[times_index] = difference; //get difference between times
	last = now; //reset 'last' so that next time we get a new 'now', we can get another difference
	
	times_index = times_index+1; //increment index for array of times
	
}

static void Dt0Callback(void)
{
	Dt_ClrIntFlag(DtChannel0); // Clear Irq
}

int main(void)
{
	
	// Initialize dual timer channel 0 	
	Dt_Init((stc_dt_channel_config_t*)&stcDtChannelConfig0, DtChannel0);
	Dt_EnableCount(DtChannel0); 
	

	
	//setup an IR interrupt
	WyzBee_exint_config_t WyzBeeExtIntConfig;
	uint8_t ext_port = 2;
	
	WyzBee_PDL_ZERO(WyzBeeExtIntConfig);
	WyzBeeExtIntConfig.abEnable[ext_port] = FALSE;   // INT2
	WyzBeeExtIntConfig.aenLevel[ext_port] = ExIntRisingEdge;
	WyzBeeExtIntConfig.apfnExintCallback[ext_port] = &Main_ExtIntCallback1;
	WyzBeeSpi_Init(&config_stc);
	WyzBee_Exint_IR_Init(&WyzBeeExtIntConfig);
	WyzBee_Exint_EnableChannel(ext_port);

	/*TODO: Initialize the OLED display according to Lab1.*/
	times;
	ns; //ns is newString
	
	while(1)
	{
		Adafruit_SSD1351 myOled = Adafruit_SSD1351();		
		
		//below, convert all array values to 1 (for range 700), or 0 (anything else)		
		if(times_index >= 34){
			for(int i=2; i<34; i++){
				
				if(times[i] >= 650 && times[i] <= 800)
					ns[i] = 1;
				else
					ns[i] = 0;
				
			} //for(int i=2)
		
		
		
		//NOW analyze newString and categorize as from library, or foreign
		
		//below check all the unique 1's for Button1 - all these 8 indices should contain 1
		//basically, was button 1 pressed?
		if( (ns[18]+ ns[22]+ ns[27]+ ns[28]+ ns[29]+ ns[31]+ ns[32]+ ns[33]) == 8 ){

			//myOled.setCursor(2, 100);
			//myOled.write('1'); //print the button itself if incorrect
			whichButton = 1;
		}
		//below check Button2's indices
		else if( (ns[19]+ ns[22]+ ns[26]+ ns[28]+ ns[29]+ ns[31]+ ns[32]+ ns[33]) == 8){
			//myOled.setCursor(2, 100);
			//myOled.write('2'); //print the button itself if incorrect
			whichButton = 2;
		} 
		//below check Button3's unique indices to all contain 1
		else if( (ns[18]+ ns[19]+ ns[22]+ ns[28]+ ns[31]+ ns[32]+ ns[33]) == 7){
			//myOled.setCursor(2, 100);
			//myOled.write('Y'); //print Y (for YES) if correct
			//for(int i=0; i<100000000; i++){}
			whichButton = 100;
		}
		//below check ButtonOK's indices
		else if( (ns[20]+ ns[24]+ ns[26]+ ns[27]+ ns[29]+ ns[30]+ ns[31]+ ns[33]) == 8){
			//myOled.setCursor(2, 100);
			//myOled.write('K'); //print the button itself if incorrect
			whichButton = 4;
		}			
		//below check ButtonGuide's unique indices to all contain 1
		else if( (ns[18]+ ns[20]+ ns[23]+ ns[27]+ ns[29]+ ns[30]+ ns[32]+ ns[33]) == 8){
			//myOled.setCursor(2, 100);
			//myOled.write('G'); //print the button itself if incorrect
			whichButton = 5;
		}
		//below check ButtonMenu's indices
		//else if( (ns[18]+ ns[19]+ ns[24]+ ns[28]+ ns[29]+ ns[30]+ ns[31]+ ns[33]) == 8){}
		
		else{
			whichButton = -99;
			//myOled.setCursor(2, 100);
			//myOled.write('X'); // X means button not recognized
		}		
		
		for(int i=0; i<10000000; i++){}
			
		WyzBeeSpi_Init(&config_stc);
		myOled.begin(); 																						
		myOled.fillScreen(RED);
		myOled.setTextSize(1); 
		myOled.setTextColor(WHITE);
		
		
		char *name[7];
		char line1[] = "Number Guess!";  //Correct answer is 3
		char line2[] = "Button1 is 1";
		char line3[] = "Button2 is 2";
		char line4[] = "Button3 is 3";
		char line5[] = "'OK' is 4";
		char line6[] = "'Guide' is 5";
		char line7[2];
		sprintf(line7, "%d", whichButton);
			
		name[0] = line1;
		name[1] = line2;
		name[2] = line3;
		name[3] = line4;
		name[4] = line5;
		name[5] = line6;
		name[6] = line7;
		
		WyzBeeSpi_Init(&config_stc);	//Initializes the screen
		printer(name,7, WHITE);
		
		for(int i=0; i<10000000; i++){}
		
		times_index=0;
		} //if(times_index)
		
	}//while(1)
	
}//main()
/*
 *********************************************************************************************************
 *                                           END
 *********************************************************************************************************
 */
