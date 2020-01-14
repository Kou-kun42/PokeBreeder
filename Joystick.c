/*
Nintendo Switch Fightstick - Proof-of-Concept

Based on the LUFA library's Low-Level Joystick Demo
	(C) Dean Camera
Based on the HORI's Pokken Tournament Pro Pad design
	(C) HORI

This project implements a modified version of HORI's Pokken Tournament Pro Pad
USB descriptors to allow for the creation of custom controllers for the
Nintendo Switch. This also works to a limited degree on the PS3.

Since System Update v3.0.0, the Nintendo Switch recognizes the Pokken
Tournament Pro Pad as a Pro Controller. Physical design limitations prevent
the Pokken Controller from functioning at the same level as the Pro
Controller. However, by default most of the descriptors are there, with the
exception of Home and Capture. Descriptor modification allows us to unlock
these buttons for our use.
*/

// make && sudo dfu-programmer atmega16u2 erase && sudo dfu-programmer atmega16u2 flash Joystick.hex

// make && ./teensy_loader_cli -mmcu=atmega32u4 -w Joystick.hex

#include "Joystick.h"

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	X,
	Y,
	A,
	B,
	L,
	R,
	HOME,
	NOTHING,
	PLUS,
	MINUS,
	TRIGGERS,
	DTOP,
	DTOPR,
	DRIGHT,
	DBOTR,
	DBOT,
	DBOTL,
	DLEFT,
	DTOPL
} Buttons_t;

typedef struct {
	Buttons_t button;
	uint16_t duration;
} command; 

// First set of instructions
static const command first[] = {
	// Setup controller
	{ NOTHING,  100 },
	{ TRIGGERS,   5 },

	// Run on bridge while pressing A to trigger hatches
	{ RIGHT,      50 },
	{ A,           5 },
	{ RIGHT,      50 },
	{ A,           5 },
	{ RIGHT,      50 },
	{ A,           5 },
	{ LEFT,       50 },
	{ A,           5 },
	{ LEFT,       50 },
	{ A,           5 },
};

// Second set of instructions
static const command second[] = {
	// Fly back to breeder
	{ NOTHING,  140 },
	{ X,          5 },
	{ NOTHING,   30 },
	{ PLUS,       5 },
	{ NOTHING,  110 },
	{ DTOPR,      1 },
	{ NOTHING,   15 },
	{ A,          5 },
	{ NOTHING,   25 },
	{ A,          5 },
	{ NOTHING,  120 },
	{ LEFT,      22 },
	{ UP,         3 },

	// Pick up new egg from breeder
	{ NOTHING,    5 },
	{ A,          5 },
	{ NOTHING,   50 },
	{ A,          5 },
	{ NOTHING,   35 },
	{ B,          5 },
	{ NOTHING,   25 },
	{ B,          5 },
	{ NOTHING,   85 },
	{ DOWN,       5 }, 
	{ A,          5 },
	{ NOTHING,  100 },
	{ A,          5 },
	{ NOTHING,  100 },
	{ A,          5 },
	{ DOWN,      20 },
	

};

// Third set of instructions
static command third[] = {
	// Swap out newborns for eggs
	{ NOTHING,   10 },
	{ X,          5 },
	{ NOTHING,   30 },
	{ A,          5 },
	{ NOTHING,   60 },
	{ R,          5 },
	{ NOTHING,   70 },
	{ L,          5 },
	{ NOTHING,   20 },
	{ LEFT,       5 },
	{ NOTHING,   10 },
	{ DOWN,       5 },
	{ NOTHING,   10 },
	{ Y,          5 },
	{ NOTHING,    5 },
	{ Y,          5 },
	{ NOTHING,    5 },
	{ A,          5 },
	{ NOTHING,   10 },
	{ DOWN,      30 },
	{ NOTHING,   10 },
	{ A,          5 },
	{ NOTHING,   10 },
	{ DRIGHT,     5 },
	{ NOTHING,    5 },
	{ NOTHING,    0 }, // second column in box
	{ NOTHING,    0 },
	{ NOTHING,    0 }, // third column in box
	{ NOTHING,    0 },
	{ NOTHING,    0 }, // forth column in box
	{ NOTHING,    0 },
	{ NOTHING,    0 }, // fifth column in box
	{ NOTHING,    0 },
	{ NOTHING,    0 }, // sixth column in box
	{ NOTHING,    0 },
	{ UP,         5 },
	{ NOTHING,   10 },
	{ A,          5 },
	{ NOTHING,   20 },
	{ R,          5 },
	{ NOTHING,   20 },
	{ A,          5 },
	{ NOTHING,   10 },
	{ DOWN,      30 },
	{ NOTHING,   10 },
	{ A,          5 },
	{ NOTHING,   20 },
	{ LEFT,      30 },
	{ NOTHING,   10 },
	{ DOWN,       5 },
	{ NOTHING,   10 },
	{ A,          5 },
	{ NOTHING,   10 },
	{ B,          5 },
	{ NOTHING,   50 },
	{ B,          5 },
	{ NOTHING,   70 },
	{ B,          5 },
	{ NOTHING,   60 },
	{ B,          5 },
	{ NOTHING,   30 },
	{ B,          5 },	
	
};

static const command forth[] = {
	// Move eggs and change box
	{ X,          5 },
	{ NOTHING,   30 },
	{ A,          5 },
	{ NOTHING,   60 },
	{ R,          5 },
	{ NOTHING,   70 },
	{ Y,          5 },
	{ NOTHING,    5 },
	{ Y,          5 },
	{ NOTHING,    5 },
	{ A,          5 },
	{ DOWN,      30 },
	{ RIGHT,     30 },
	{ A,          5 },
	{ NOTHING,   10 },
	{ R,          5 },
	{ NOTHING,   20 },
	{ A,          5 },
	{ NOTHING,   10 },
	{ B,          5 },
	{ NOTHING,   50 },
	{ B,          5 },
	{ NOTHING,   50 },
	{ B,          5 },
	{ NOTHING,   60 },
	{ B,          5 },
	{ NOTHING,   30 },
};

// Main entry point.
int main(void) {
	// We'll start by performing hardware and peripheral setup.
	SetupHardware();
	// We'll then enable global interrupts for our use.
	GlobalInterruptEnable();
	// Once that's done, we'll enter an infinite loop.
	for (;;)
	{
		// We need to run our task to process and deliver data for our IN and OUT endpoints.
		HID_Task();
		// We also need to run the main USB management task.
		USB_USBTask();
	}
}

// Configures hardware and peripherals, such as the USB peripherals.
void SetupHardware(void) {
	// We need to disable watchdog if enabled by bootloader/fuses.
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// We need to disable clock division before initializing the USB hardware.
	clock_prescale_set(clock_div_1);
	// We can then initialize our hardware and peripherals, including the USB stack.

	#ifdef ALERT_WHEN_DONE
	// Both PORTD and PORTB will be used for the optional LED flashing and buzzer.
	#warning LED and Buzzer functionality enabled. All pins on both PORTB and \
PORTD will toggle when printing is done.
	DDRD  = 0xFF; //Teensy uses PORTD
	PORTD =  0x0;
                  //We'll just flash all pins on both ports since the UNO R3
	DDRB  = 0xFF; //uses PORTB. Micro can use either or, but both give us 2 LEDs
	PORTB =  0x0; //The ATmega328P on the UNO will be resetting, so unplug it?
	#endif
	// The USB stack should be initialized last.
	USB_Init();
}

// Fired to indicate that the device is enumerating.
void EVENT_USB_Device_Connect(void) {
	// We can indicate that we're enumerating here (via status LEDs, sound, etc.).
}

// Fired to indicate that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void) {
	// We can indicate that our device is not ready (via status LEDs, sound, etc.).
}

// Fired when the host set the current configuration of the USB device after enumeration.
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	// We setup the HID report endpoints.
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);

	// We can read ConfigSuccess to indicate a success or failure at this point.
}

// Process control requests sent to the device from the USB host.
void EVENT_USB_Device_ControlRequest(void) {
	// We can handle two control requests: a GetReport and a SetReport.

	// Not used here, it looks like we don't receive control request from the Switch.
}

// Process and deliver data from IN and OUT endpoints.
void HID_Task(void) {
	// If the device isn't connected and properly configured, we can't do anything here.
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;

	// We'll start with the OUT endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
	// We'll check to see if we received something on the OUT endpoint.
	if (Endpoint_IsOUTReceived())
	{
		// If we did, and the packet has data, we'll react to it.
		if (Endpoint_IsReadWriteAllowed())
		{
			// We'll create a place to store our data received from the host.
			USB_JoystickReport_Output_t JoystickOutputData;
			// We'll then take in that data, setting it up in our storage.
			while(Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL) != ENDPOINT_RWSTREAM_NoError);
			// At this point, we can react to this data.

			// However, since we're not doing anything with this data, we abandon it.
		}
		// Regardless of whether we reacted to the data, we acknowledge an OUT packet on this endpoint.
		Endpoint_ClearOUT();
	}

	// We'll then move on to the IN endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
	// We first check to see if the host is ready to accept data.
	if (Endpoint_IsINReady())
	{
		// We'll create an empty report.
		USB_JoystickReport_Input_t JoystickInputData;
		// We'll then populate this report with what we want to send to the host.
		GetNextReport(&JoystickInputData);
		// Once populated, we can output this data to the host. We do this by first writing the data to the control stream.
		while(Endpoint_Write_Stream_LE(&JoystickInputData, sizeof(JoystickInputData), NULL) != ENDPOINT_RWSTREAM_NoError);
		// We then send an IN packet on this endpoint.
		Endpoint_ClearIN();
	}
}

typedef enum {
	SYNC_CONTROLLER,
	SYNC_POSITION,
	BREATHE,
	PROCESS,
	CLEANUP,
	DONE
} State_t;
State_t state = SYNC_CONTROLLER;

#define ECHOES 2
int echoes = 0;
USB_JoystickReport_Input_t last_report;

int report_count = 0;
int xpos = 0;
int ypos = 0;
int bufindex = 0;
int bufindex2 = 0;
int bufindex3 = 0;
int bufindex4 = 0;
int duration_count = 0;
int duration_count2 = 0;
int duration_count3 = 0;
int duration_count4 = 0;
int portsval = 0;

// Number of times the sets will loop before moving to the next
// These will vary per pokemon
int f_set = 6;
int s_set = 6;

// Currently set to complete when a box is filled
int t_set = 6;

// Loops will end once this is achieved
// Number of boxes you want to breed
int fo_set = 1;

// Number of loops completed for each set;
int floop = 0;
int sloop = 0;
int tloop = 0;
int foloop = 0;

// Current set of commands
int setnum = 1;

// Button Identifier
Buttons_t butt;

// Prepare the next report for the host.
void GetNextReport(USB_JoystickReport_Input_t* const ReportData) {

	// Prepare an empty report
	memset(ReportData, 0, sizeof(USB_JoystickReport_Input_t));
	ReportData->LX = STICK_CENTER;
	ReportData->LY = STICK_CENTER;
	ReportData->RX = STICK_CENTER;
	ReportData->RY = STICK_CENTER;
	ReportData->HAT = HAT_CENTER;

	// Repeat ECHOES times the last report
	if (echoes > 0)
	{
		memcpy(ReportData, &last_report, sizeof(USB_JoystickReport_Input_t));
		echoes--;
		return;
	}

	// States and moves management
	switch (state)
	{

		case SYNC_CONTROLLER:
			state = BREATHE;
			break;

		case SYNC_POSITION:
			bufindex = 0;
			bufindex2 = 0;
			bufindex3 = 0;
			bufindex4 = 0;


			ReportData->Button = 0;
			ReportData->LX = STICK_CENTER;
			ReportData->LY = STICK_CENTER;
			ReportData->RX = STICK_CENTER;
			ReportData->RY = STICK_CENTER;
			ReportData->HAT = HAT_CENTER;


			state = BREATHE;
			break;

		case BREATHE:
			state = PROCESS;
			break;

		case PROCESS:

			// End infinite grind after box is filled
			if (tloop == t_set)
			{
				while (1) {}
			}


			if (setnum == 1)
			{
				butt = first[bufindex].button;
			}
			else if (setnum == 2)
			{
				butt = second[bufindex2].button;
			}
			else if (setnum == 3)
			{
				butt = third[bufindex3].button;
			}
			else
			{
				butt = forth[bufindex4].button;
			}

			switch (butt)
			{

				case UP:
					ReportData->LY = STICK_MIN;				
					break;

				case LEFT:
					ReportData->LX = STICK_MIN;				
					break;

				case DOWN:
					ReportData->LY = STICK_MAX;				
					break;

				case RIGHT:
					ReportData->LX = STICK_MAX;				
					break;

				case PLUS:
					ReportData->Button |= SWITCH_PLUS;
					break;

				case MINUS:
					ReportData->Button |= SWITCH_MINUS;
					break;

				case A:
					ReportData->Button |= SWITCH_A;
					break;

				case B:
					ReportData->Button |= SWITCH_B;
					break;

				case X:
					ReportData->Button |= SWITCH_X;
					break;

				case Y:
					ReportData->Button |= SWITCH_Y;
					break;

				case R:
					ReportData->Button |= SWITCH_R;
					break;

				case L:
					ReportData->Button |= SWITCH_L;
					break;

				case HOME:
					ReportData->Button |= SWITCH_HOME;			
					break;

				case TRIGGERS:
					ReportData->Button |= SWITCH_L | SWITCH_R;
					break;

				case DTOP:
					ReportData->HAT = HAT_TOP;				
					break;

				case DTOPR:
					ReportData->HAT = HAT_TOP_RIGHT;
					break;

				case DRIGHT:
					ReportData->HAT = HAT_RIGHT;
					break;

				case DBOTR:
					ReportData->HAT = HAT_BOTTOM_RIGHT;
					break;

				case DBOT:
					ReportData->HAT = HAT_BOTTOM;
					break;

				case DBOTL:
					ReportData->HAT = HAT_BOTTOM_LEFT;
					break;

				case DLEFT:
					ReportData->HAT = HAT_LEFT;
					break;

				case DTOPL:
					ReportData->HAT = HAT_TOP_LEFT;
					break;

				default:
					ReportData->LX = STICK_CENTER;
					ReportData->LY = STICK_CENTER;
					ReportData->RX = STICK_CENTER;
					ReportData->RY = STICK_CENTER;
					ReportData->HAT = HAT_CENTER;
					break;
			}

			if (setnum == 1)
			{
				duration_count++;
			}
			else if (setnum == 2)
			{
				duration_count2++;
			}
			else if (setnum == 3)
			{
				duration_count3++;
			}
			else
			{
				duration_count4++;
			}

			if (duration_count > first[bufindex].duration)
			{
				bufindex++;
				duration_count = 0;
			}

			if (duration_count2 > second[bufindex2].duration)
			{
				bufindex2++;
				duration_count2 = 0;	
			}
			
			if (duration_count3 > third[bufindex3].duration)
			{
				bufindex3++;
				duration_count3 = 0;
			}

			if (duration_count4 > forth[bufindex4].duration)
			{
				bufindex4++;
				duration_count4 = 0;
			}

			if (bufindex > (int)( sizeof(first) / sizeof(first[0])) - 1)
			{
				// state = CLEANUP;
				bufindex = 2;
				duration_count = 0;
				floop++;
				// Switch set of instructions
				if (floop == f_set)
				{
					setnum = 2;
					floop = 0;	
				}
	
					state = BREATHE;
	
				ReportData->LX = STICK_CENTER;
				ReportData->LY = STICK_CENTER;
				ReportData->RX = STICK_CENTER;
				ReportData->RY = STICK_CENTER;
				ReportData->HAT = HAT_CENTER;
			}

			if (bufindex2 > (int)( sizeof(second) / sizeof(second[0])) - 1)
			{
				bufindex2 = 0;
				duration_count2 = 0;
				sloop++;

				if (sloop == s_set)
				{
					setnum = 3;
					// Change column in box system
					if (tloop != 0)
					{
						if (tloop == 1)
						{
							third[25].duration = 5;
							third[25].button = DRIGHT;
							third[26].duration = 5;
						}
						if (tloop == 2)
						{
							third[27].duration = 5;
							third[27].button = DRIGHT;
							third[28].duration = 5;
						}
						if (tloop == 3)
						{
							third[29].duration = 5;
							third[29].button = DRIGHT;
							third[30].duration = 5;
						}
						if (tloop == 4)
						{
							third[31].duration = 5;
							third[31].button = DRIGHT;
							third[32].duration = 5;
						}
						if (tloop == 5)
						{
							third[33].duration = 5;
							third[33].button = DRIGHT;
							third[34].duration = 5;
						}
					}					
					sloop = 0;
				}
				else
				{
					setnum = 1;
				}

					state = BREATHE;
	
				ReportData->LX = STICK_CENTER;
				ReportData->LY = STICK_CENTER;
				ReportData->RX = STICK_CENTER;
				ReportData->RY = STICK_CENTER;
				ReportData->HAT = HAT_CENTER;
			}

			if (bufindex3 > (int)( sizeof(third) / sizeof(third[0])) - 1)
			{
				bufindex3 = 0;
				duration_count3 = 0;
				tloop++;
				
				if (tloop == t_set)
				{
					setnum = 4;
					tloop = 0;

					third[25].duration = 0;
					third[25].button = NOTHING;
					third[26].duration = 0;

					third[27].duration = 0;
					third[27].button = NOTHING;
					third[28].duration = 0;

					third[29].duration = 0;
					third[29].button = NOTHING;
					third[30].duration = 0;

					third[31].duration = 0;
					third[31].button = NOTHING;
					third[32].duration = 0;

					third[33].duration = 0;
					third[33].button = NOTHING;
					third[34].duration = 0;
				}
				else
				{
					setnum = 1;
				}

					state = BREATHE;
	
				ReportData->LX = STICK_CENTER;
				ReportData->LY = STICK_CENTER;
				ReportData->RX = STICK_CENTER;
				ReportData->RY = STICK_CENTER;
				ReportData->HAT = HAT_CENTER;
			}

			if (bufindex4 > (int)( sizeof(forth) / sizeof(forth[0])) - 1)
			{
				// state = CLEANUP;
				bufindex4 = 0;
				duration_count4 = 0;
				foloop++;
				setnum = 1;

					state = BREATHE;
	
				ReportData->LX = STICK_CENTER;
				ReportData->LY = STICK_CENTER;
				ReportData->RX = STICK_CENTER;
				ReportData->RY = STICK_CENTER;
				ReportData->HAT = HAT_CENTER;
			}

			break;

		case CLEANUP:
			state = DONE;
			break;

		case DONE:
			#ifdef ALERT_WHEN_DONE
			portsval = ~portsval;
			PORTD = portsval; //flash LED(s) and sound buzzer if attached
			PORTB = portsval;
			_delay_ms(250);
			#endif
			return;
	}


	// Prepare to echo this report
	memcpy(&last_report, ReportData, sizeof(USB_JoystickReport_Input_t));
	echoes = ECHOES;

}
