
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
 * Programmer(s)  : IBRAHIM AHMED and YIYANG BAI
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
#include <wyzbee_bt.h>
#include <string.h>

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
			myOled.setCursor(8*i, 2+10*j); //@ Set the print cursor to the initial location
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

void Main_ExtIntCallback1()			//function for interrupts
{
	
	now = Dt_ReadCurCntVal(Dt_Channel0); 	//get time for current rising edge
	
	difference = last-now;								
	times[times_index] = difference; 			//get difference between times
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
	WyzBeeExtIntConfig.abEnable[ext_port] = TRUE;   // INT2
	WyzBeeExtIntConfig.aenLevel[ext_port] = ExIntRisingEdge;
	WyzBeeExtIntConfig.apfnExintCallback[ext_port] = &Main_ExtIntCallback1;
	WyzBeeSpi_Init(&config_stc);
	WyzBee_Exint_IR_Init(&WyzBeeExtIntConfig);
	WyzBee_Exint_EnableChannel(ext_port);

	times; //times and ns mentioned only for debugging purposes
	ns; //ns is newString
	
	
	char seq_string[25];  //to store button_presses, as a string
	for(int i=0; i<strlen(seq_string); i++){
			seq_string[i] = ' ';
	}
	int button_presses[50];  //to hold all the buttons entered 
	int button_presses_index = 0;
	
	//Set up Bluetooth configuration
	sys_ticks_init();
	RSI_BT_EVENT_INQUIRY_RESPONSE info_array[1];
					
	uint8 board_name[] = "BATMAN";
	WyzBee_BT_init();
	WyzBee_SetLocalName(board_name);
	WyzBee_SetDiscoverMode(1, 5000); //1 for enable discover, 10000ms
	WyzBee_SetConnMode(1);
	WyzBee_InitSppProfile();
	
	WyzBee_GetInquiryResults((RSI_BT_EVENT_INQUIRY_RESPONSE*)&info_array, 1);
	
	//Initialize for printing
	WyzBeeSpi_Init(&config_stc);
	
	char whichButton_string[2];  //remote button converted to string for printing

	char *name[1];	// Create array to hold lines of text
					
	char* printing[6];
	char* printing2[1];
	
	char addr0[] = "addr0";
	char addr1[] = "addr1";
	char addr2[] = "addr2";
	char addr4[] = "addr4";
	char addr5[] = "addr5";
	
	char* device_name;	//variables to hold found devices through Bluetooth
	char* name_length;
	
	//Below set BT info to pair with Nexus 5 phone specifically
	uint8 pair_mode = 1;
	WyzBee_SetPairMode(pair_mode);
	uint8 nexus5_addr[] =	"C4:43:8F:A0:7F:3C";
	uint8 p_data[] = "penguin";
	uint8 p_receive[] = "back";
		
	while(1)
	{
			
			bool finished_sequence = FALSE; //this will be TRUE when the GUIDE button is pressed
		
			//below, convert all array values to 1 (for range 700), or 0 (anything else)		
			if(times_index >= 34){
				
				for(int i=2; i<34; i++){
					if(times[i] >= 650 && times[i] <= 800)			//Binary code of '1' for this signal range
						ns[i] = 1;
					else																				//Binary code of '0' for any other range
						ns[i] = 0;
				} //for(int i=2)
			
				if((ns[18]+ ns[20]+ ns[23]+ ns[27]+ ns[29]+ ns[30]+ ns[32]+ ns[33]) == 8){ //GUIDE button pressed, end the button sequence
					whichButton = 71;
					finished_sequence = TRUE;
				}	
				
				else{  //label whichButton value according to hardcoded library
						
					if( (ns[20]+ ns[24]+ ns[26]+ ns[27]+ ns[29]+ ns[30]+ ns[31]+ ns[33]) == 8){ 			//Button ENTER Pattern
						whichButton = 69;
					}
					else if( (ns[22]+ ns[26]+ ns[27]+ ns[28]+ ns[29]+ ns[31]+ ns[32]+ ns[33]) == 8){ 	//Button 0/SPACE Pattern
						whichButton = 0;
					}
					else if( (ns[18]+ ns[22]+ ns[27]+ ns[28]+ ns[29]+ ns[31]+ ns[32]+ ns[33]) == 8 ){	//Button 1 pattern
						whichButton = 1;
					}
					else if( (ns[19]+ ns[22]+ ns[26]+ ns[28]+ ns[29]+ ns[31]+ ns[32]+ ns[33]) == 8){	//Button 2 pattern
						whichButton = 2;
					} 
					else if( (ns[18]+ ns[19]+ ns[22]+ ns[28]+ ns[31]+ ns[32]+ ns[33]) == 7){ 					//Button 3 Pattern
						whichButton = 3;
					}
					else if( (ns[20]+ ns[22]+ ns[26]+ ns[27]+ ns[29]+ ns[31]+ ns[32]+ ns[33]) == 8){ 	//Button 4 Pattern
						whichButton = 4;
					}
					else if( (ns[18]+ ns[20]+ ns[22]+ ns[27]+ ns[29]+ ns[31]+ ns[32]+ ns[33]) == 8){ 	//Button 5 Pattern
						whichButton = 5;
					}
					else if( (ns[19]+ ns[20]+ ns[22]+ ns[26]+ ns[29]+ ns[31]+ ns[32]+ ns[33]) == 8){	//Button 6 Pattern
						whichButton = 6;
					}
					else if( (ns[18]+ ns[19]+ ns[20]+ ns[22]+ ns[29]+ ns[31]+ ns[32]+ ns[33]) == 8){	//Button 7 Pattern
						whichButton = 7;
					}
					else if( (ns[21]+ ns[22]+ ns[26]+ ns[27]+ ns[28]+ ns[31]+ ns[32]+ ns[33]) == 8){	//Button 8 Pattern
						whichButton = 8;
					}
					else if( (ns[18]+ ns[21]+ ns[22]+ ns[27]+ ns[28]+ ns[31]+ ns[32]+ ns[33]) == 8){ 	//Button 9 Pattern
						whichButton = 9;
					}
					else{
						whichButton = -99;							//Default value for any Button outside of our set
					}		
							
					//Place whichButton into array
					button_presses[button_presses_index] = whichButton;					
					button_presses_index++;
					
					//PRINTING -------------------------
					
					for(int i=0; i<10000000; i++){}		//delay to help adjust printing

					sprintf(whichButton_string, "%d", whichButton); 	//convert whichButton to a string for printing
					name[0] = whichButton_string;
						
					WyzBeeSpi_Init(&config_stc);	//Initializes the screen
					printer(name, 1, WHITE);				//Command for printing 
		
					for(int i=0; i<10000000; i++){}	//delay to adjust printing
				} //else label whichButton
						
				times_index=0;									// reset our array of remote signal
			} //if(times_index)
				
			
			if(finished_sequence == TRUE){
				
			//Here, GUIDE has been pressed and we have to decode button_presses into a readable string with letters		
				
			//CONVERSION START -------------
					
					char letters[10];
					letters[0] = ' ';
					letters[1] = '!';
					letters[2] = 'A';
					letters[3] = 'D';
					letters[4] = 'G';
					letters[5] = 'J';
					letters[6] = 'M';
					letters[7] = 'P';
					letters[8] = 'T';
					letters[9] = 'W';

					int index = 0;
					int number = -99;
					int seq_string_index=0;
					while(index < 50){
					
						int index2 = index;
						int count = 0;
						if(button_presses[index] == 69){
							index++;
							continue;
						}
						while(button_presses[index2] != 69 && index2 < 50){
							count++;
							number = button_presses[index];
							index2++;
						}
						if(button_presses[index]==0){
							seq_string[seq_string_index] = letters[number];
						}
						else{
							seq_string[seq_string_index] = letters[number]+count-1;
						}
						seq_string_index++;
						index=index2; //pick up at 69 where we left off
						index++;
						
					} //while(index < 50)
					
			//CONVERSION FINISH-------------
					
		
		//There was a bug in the conversion function that led to a 'Hard Fault' access of array outside of its bounds 			
		/*
		sprintf(name_length, "%d", info_array[0].NameLength);
		printing[0] = name_length;
		
		device_name = info_array[0].RemoteDeviceName;
		printing[1] = device_name;
	
		sprintf(addr1, "%02X%02X%02X", info_array[0].BDAddress[5], info_array[0].BDAddress[4], info_array[0].BDAddress[3]);	
		printing[2] = addr1;
		
		sprintf(addr2, "%02X%02X%02X", info_array[0].BDAddress[2], info_array[0].BDAddress[1], info_array[0].BDAddress[0]);
		printing[3] = addr2;
	
		//Now combine addr1 and addr2 into a complete address string
		char addr3[12];
		strcpy(addr3, addr1);
		strcat(addr3, addr2);
		printing[4] = addr3;
		
		uint8 local_address[] = "";
		WyzBee_GetLocalBDAddress(local_address);
	
		sprintf(addr4, "%s", local_address);
		printing[5] = addr4; //addr length is 17 - 00:23:A7:80:59:F7
		
		printer(printing, 6, WHITE); 	// Print discovered device information
		*/
		
		
		//Part B	- Feed the IR Remote text from seq_string into the Bluetooth signal to Nexus 5 mobile device
		
		
		//Place whichButton (char line7[2]) into uint8 p_data[]
		for(int i=0; i<strlen(seq_string); i++){
			p_data[i] = seq_string[i];
		}
		
		uint16 size = strlen(seq_string);	//get size of seq_string 
		
		//Below connect with mobile device, send it the Remote-entered text, and receive back text from mobile
		if(WaitForSPPConnComplete() == 0){	
			WyzBee_SPPTransfer(nexus5_addr, p_data, size); //size 7, for example, means 7 characters
			WyzBee_SPPReceive(p_receive, 7);
		}
		
		//Below print on screen the text mobile sent back
		sprintf(addr5, "%s", p_receive);
		printing2[0] = addr5;
		printer(printing2, 1, BLACK);
		for(int i=0; i<10000000; i++){}	
		
		} //if(sequence_finished==TRUE)
	} //outer while loop
} //main()
/*
 *********************************************************************************************************
 *                                           END
 *********************************************************************************************************
 */
