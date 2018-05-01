/*
 * main.c
 */

#include "Nokia5110.h"
#include "sdcard.h"
#include "1wire.h"
#include "uart.h"
#include <stdint.h>
#include <string.h>

#define SW1 0

#define MIN_BRIGHT 5
#define MIN_DELAY 5
#define MAX_BRIGHT 100
#define MAX_DELAY 995
#define BL_ON 1
#define BL_OFF 0
#define MAIN_MENU 2
#define FILE_MENU 3
#define BUZZER_ON 4
#define BUZZER_OFF 5
#define INIT_MENU 6
#define PAINT_MENU 7
#define COLOR_MENU 8
#define CURRENTLY_PAINTING 9
#define RED 10
#define GREEN 11
#define BLUE 12
#define RESET_COLORS 13
#define LIGHTS_OFF 14
#define LIGHTS_ON 15


void initialize_buttons(void);
void SysTick_Init(void);
void init_buzzer(uint16_t period, uint16_t high);
void SysTick_Wait50ms(unsigned long delay);
uint8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max);
void SysTick_Wait(unsigned long delay);
void set_init_menu(void);
void set_main_picture_menu(void);
void manage_picture_menu(void);
void set_paint_menu(void);
void manage_paint_menu(void);


char menu=INIT_MENU;
char backlight=BL_ON;
char buzzer=BUZZER_OFF,subdir=0,max_menu_items=0;
int8_t file_pos=-1;
int bright=5,delay=5,current_position=1;
int main(void)
{
	SysTick_Init();
	//init_buzzer(16000, 0);
	init_1wire_pin();
	startSSI0();
	initialize_screen(BACKLIGHT_ON/*,SSI0*/);
	clear_screen();
	/*screen_write("Testing\ncolors...",ALIGN_CENTRE_CENTRE,SSI0);*/
	//test_colors();
	/*clear_screen();*/
	sd_startSSI3();
	initialise_sd(SD_SSI3);
	cs_high(/*SD_SSI3*/);
	change_speed(SD_SSI3);
	cs_low(/*SD_SSI3*/);
	read_first_sector(SD_SSI3);
	read_disk_data(SD_SSI3);
	unsigned long reserved_sectors=get_reserved_sectors();
	if(reserved_sectors==1)
	{
		//Message to be shown if not FAT32 (single format)
		screen_write("Invalid\nSD card\nformat",ALIGN_CENTRE_CENTRE/*,SSI0*/);
	}
	else
	{

		long next_cluster=get_root_dir_first_cluster();
		screen_write("Initialising...",ALIGN_CENTRE_CENTRE/*,SSI0*/);
		do
		{
			next_cluster=list_dirs_and_files(next_cluster,SHORT_NAME,GET_SUBDIRS,SD_SSI3);
		}while(next_cluster!=0x0FFFFFFF && next_cluster!=0xFFFFFFFF);
		filter_by_extension("PNM",1);
		clear_screen();
		set_init_menu();
		initialize_buttons();
		current_position=2;
		show_menu(2/*,SSI0*/);
		while(1)
		{
			char keep_going=0;
			while(keep_going==0)
			{
				char portB_data=GPIO_PORTB_DATA_R&0x33;
				char portE_data=GPIO_PORTE_DATA_R&0x32;
				if(portB_data > 0x00)
				{
					switch(portB_data)
					{
						case 0x01: //PB0 (Change backlight status)
						{
							if(backlight==BL_ON)
							{
								disable_backlight(/*SSI0*/);
								backlight=BL_OFF;
							}
							else
							{
								enable_backlight(/*SSI0*/);
								backlight=BL_ON;
							}
						break;
						}
						case 0x02: //PB1 (Select element at current position)
						{
							if(current_position==2)
							{
								menu = PAINT_MENU;
							}
							else
							{
								menu = MAIN_MENU;
							}
							keep_going=1;
							break;
						}

					}
				}
				if(portE_data > 0x00)
				{
					switch(portE_data)
					{
						case 0x02: // PE1 (down)
						{
							if(current_position==2)
							{
								current_position=3;
							}
							break;
						}
						case 0x20: // PE5 (up)
						{
							if(current_position==3)
							{
								current_position=2;
							}
							break;
						}
					}
					show_menu(current_position/*,SSI0*/);
				}
			}
			clear_screen();
			delete_menu();
			if(menu==MAIN_MENU)
			{
				initialise_uart();
				//ask_for_bluetooth();
				//send_files_info();
				clear_screen();
				set_main_picture_menu();
				show_menu(1/*,SSI0*/);
				current_position=1;
				manage_picture_menu();
			}
			else
			{
				set_paint_menu();
				show_menu(1/*,SSI0*/);
				current_position=1;
				manage_paint_menu();
			}
		}
		/*initialize_I2C();
		initialize_accelerometer();
		read_accelerometer_data();*/
	}
	return 0;
}

void send_files_info()
{
	int i=0,j=0;
	UART_OutChar('<');
	for(i=0;i<12;i++)
	{
		unsigned char name[25];
		get_name(i,name);
		if(name[0]!=0x00)
		{
			for(j=0;j<25;j++)
			{
				UART_OutChar(name[j]);
			}
			UART_OutChar('\n');
			//UART_OutChar('');
		}
	}
	UART_OutChar('<');
}

void ask_for_bluetooth()
{
	screen_write("Set the\nsettings\nthrough\nBluetooh?",ALIGN_CENTRE_CENTRE/*,SSI0*/);
	bright=(int) UART_InChar();
	delay=(int) UART_InChar();
	UART_OutChar('1');
}

void set_init_menu()
{
	add_menu_element("Lightwand");
	add_menu_element("Select option");
	add_menu_element("1. Paint");
	add_menu_element("2. Picture");
	max_menu_items=4;
}

void set_main_picture_menu()
{
	add_menu_element("<-Main menu");
	/*char cad[14]="Bright: ";
	convert_value_to_string(cad, bright);
	strcat(cad," %");
	add_menu_element(cad);
	strcpy(cad,"Delay: ");
	convert_value_to_string(cad, delay);
	strcat(cad," ms");
	add_menu_element(cad);*/
	add_menu_element("Bright: 5 %");
	add_menu_element("Delay: 5 ms");
	add_menu_element("Current file:");
	add_menu_element("None");
	add_menu_element("Buzzer: off");
	max_menu_items=6;
}

void set_paint_menu()
{
	add_menu_element("Select color:");
	add_menu_element("Red");
	add_menu_element("Green");
	add_menu_element("Blue");
	add_menu_element("Random");
	add_menu_element("Monochrome");
	add_menu_element("Reset");
	max_menu_items=7;
}

void set_color_menu()
{
	add_menu_element("Select led");
	add_menu_element("LED: 1");
	add_menu_element("Bright: 5 %");
	add_menu_element("<- Back");
	max_menu_items=4;
}

void set_color_brightness_menu()
{
	add_menu_element("Select brightness");
	add_menu_element("Bright: 5 %");
	add_menu_element("<- Back");
	max_menu_items=3;
}

void initialize_buttons(void)
{
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000012;   		//  activate clock for Ports B and E
	delay = SYSCTL_RCGC2_R;         		//  allow time for clock to stabilize
	GPIO_PORTB_DIR_R &= ~0x33;             // make PB0,1,4 and 5 in
	GPIO_PORTB_AFSEL_R &= ~0x33;           // disable alt funct on PA2,3,5
	GPIO_PORTB_DEN_R |= 0x33;             // enable digital I/O on PB0,1,4 and 5

	GPIO_PORTE_DIR_R &= ~0x32;             // make PE1,4 and 5 in
	GPIO_PORTE_AFSEL_R &= ~0x32;           // disable alt funct on PE1,4 and 5
	GPIO_PORTE_DEN_R |= 0x32;             // enable digital I/O on PE1,4 and 5
}

/*
 * It does not work yet. Further development still required
 */
void initialize_I2C()
{
	volatile unsigned long delay;
	//Port B has already been initialized, so clock does not have to be set again
	SYSCTL_RCGCI2C_R |=SYSCTL_RCGCI2C_R0; //Enable clock for the I2C module
	delay=SYSCTL_RCGCI2C_R;
	GPIO_PORTB_AFSEL_R |= 0x0C;           // disable alt funct on PB2 and 3
    // configure PB2 and 3 I2C
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00003300;
    // configure PA6,7 as GPIO
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFF00FF0F)+0x00000000;
	GPIO_PORTB_DEN_R |= 0x0C;   // enable digital I/O on PB2 and 3
	GPIO_PORTB_ODR_R |=0x08;	// enable SDA for open drain operation
	I2C0_MCR_R |=I2C_MCR_MFE;	//Initialization of I2C master
	I2C0_MTPR_R |=0x00000007;	//Sets I2C clock speed to 100Kbps with 16MHz oscillator
}

/*
 * Not working yet. Further development and testing still required
 */
void initialize_accelerometer()
{
	//Establishes data format
	I2C0_MSA_R |=0x000000A6;	//Specify the slave address of the master and that the next operation is a Transmit
	I2C0_MDR_R=0x31;	//Data to be sent (1 byte) 0x32
	while(I2C0_MCS_R&I2C_MCS_BUSBSY);	//Wait until the transmission completes by polling the I2CMCS register's BUSBSY bit until it has been cleared.
	I2C0_MCS_R=0x00000003;	// (STOP, START, RUN). 7 instead of 3 originally
	//while(I2C0_MCS_R&I2C_MCS_BUSBSY);
	SysTick_Wait50ms(1);
	I2C0_MDR_R=0x01;	//Data to be sent (1 byte)
	I2C0_MCS_R=0x00000007;	// (STOP, START, RUN);
	SysTick_Wait50ms(1);
	//Establishes
	I2C0_MDR_R=0x2D;	//Data to be sent (1 byte)
	I2C0_MCS_R=0x00000007;	// (STOP, START, RUN). 7 instead of 3 originally
	while(I2C0_MCS_R&I2C_MCS_BUSBSY);	//Wait until the transmission completes by polling the I2CMCS register's BUSBSY bit until it has been cleared.
	if(I2C0_MCS_R&I2C_MCS_ERROR)	//Check the ERROR bit in the I2CMCS register to confirm the transmit was acknowledged.
	{

	}
	I2C0_MDR_R=0x08;	//Data to be sent (1 byte)
	I2C0_MCS_R=0x00000007;	// (STOP, START, RUN);
}

/*
 * Not working yet. Further development and testing still required
 */
void read_accelerometer_data()
{
	SysTick_Wait50ms(1);
	//Establishes normal operation and 100 Hz data output rate
	I2C0_MSA_R |=0x000000A6;	//Specify the slave address of the master and that the next operation is a Transmit
	I2C0_MDR_R=0x32;	//Data to be sent (1 byte) 0x32
	I2C0_MCS_R=0x00000007;	// (STOP, START, RUN). 7 instead of 3 originally
	while(I2C0_MCS_R&I2C_MCS_BUSBSY);	//Wait until the transmission completes by polling the I2CMCS register's BUSBSY bit until it has been cleared.
	if(I2C0_MCS_R&I2C_MCS_ERROR)	//Check the ERROR bit in the I2CMCS register to confirm the transmit was acknowledged.
	{

	}
	//SysTick_Wait50ms(1);
	I2C0_MSA_R |=0x000000A7;	//Specify the slave address of the master and that the next operation is a Receive
	//I2C0_MDR_R=0x00;	//Data to be sent (1 byte)
	I2C0_MCS_R=0x00000007;	// (STOP, START, RUN);
}

// The delay parameter is in units of the 16 MHz core clock.
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}


void SysTick_Wait50ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 50ms
  }
}


void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

void init_buzzer(uint16_t period, uint16_t high)
{

	volatile unsigned long delay;
	SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
	SYSCTL_RCGC2_R |= 0x00000002;   //  activate clock for Port B
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTB_DIR_R |= 0x40;             // make PB6 out
	GPIO_PORTB_AFSEL_R |= 0x40;           // enable alt funct on PB6
	GPIO_PORTB_AMSEL_R &= ~0x40;          // disable analog functionality on PB6
	GPIO_PORTB_DEN_R |= 0x40;

	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x07000000;
	TIMER0_CTL_R &=~TIMER_CTL_TAEN; //disable timer0A during setup
	TIMER0_CFG_R = 0x00000004;// configure for 16-bit timer mode
	                                   // configure for alternate (PWM) mode
	TIMER0_TAMR_R = (0x00000008|0x00000002);
	TIMER0_TAILR_R = period-1;       // timer start value
	TIMER0_TAMATCHR_R = period-high-1; // duty cycle = high/period
	TIMER0_CTL_R |= 0x00000001;  // enable timer0A 16-b, PWM
}

uint8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void GPIOPortB_Handler(void){
//GPIO_RIS
  GPIO_PORTB_ICR_R |= 0x10; // ack, clear interrupt flag4

  // stuff

}

void manage_picture_menu()
{
	char go_back=0;
	while(go_back==0)
	{
		// PE5 (SW6) - Up
		// PE1 (SW3) - Down
		// PE4 (SW2) - Increase
		// PB4 (SW4) - Decrease
		// PB1 (SW1) - Select
		// PB0 (SW0) - Backlight on/off
		// PB5 (SW5) - Start
		//Check which button is selected
		char portB_data=GPIO_PORTB_DATA_R&0x33;
		char portE_data=GPIO_PORTE_DATA_R&0x32;
		if(portB_data > 0x00)
		{
			switch(portB_data)
			{
				case 0x01: //PB0 (Change backlight status)
				{
					if(backlight==BL_ON)
					{
						disable_backlight();
						backlight=BL_OFF;
					}
					else
					{
						enable_backlight();
						backlight=BL_ON;
					}
					break;
				}
				case 0x02: //PB1 (Select element at current position)
				{
					if(menu == MAIN_MENU && current_position == 4)
					{
						subdir++;
						backup_main_menu();
						//Change to file_menu
						delete_menu();
						clear_screen();
						char i;
						max_menu_items=0;
						for(i=0;i<12;i++)
						{
							unsigned char name[25];
							get_name(i,name);
							if(name[0]!=0x00)
							{
								add_menu_element(name);
								max_menu_items++;
							}
						}
						menu=FILE_MENU;
						current_position=0;
						show_menu(0/*,SSI0*/);
					}
					else
					{
						if(menu == MAIN_MENU && current_position == 0)
						{
							delete_menu();
							clear_screen();
							set_init_menu();
							current_position=2;
							show_menu(2);
							menu=INIT_MENU;
						}
						else
						{
							if(menu == FILE_MENU)
							{
								//Change to main menu, showing the name of the selected file
								//Is it a file?
								char type=is_file_or_dir(current_position);
								if(type == 2)
								{
									copy_menu();
									delete_menu();
									file_pos=current_position;
									clear_screen();
									subdir=0;
									restore_main_menu();
									unsigned char name[25];
									get_name(file_pos,name);
									modify_menu_element(4,name);
									menu=MAIN_MENU;
									current_position=4;
									reset_min_max();
									show_menu(4/*,SSI0*/);
								}
								else
								{
									copy_menu();
									delete_menu();
									subdir++;
									long next_cluster=get_first_cluster(current_position);
									do
									{
										next_cluster=list_dirs_and_files(next_cluster,SHORT_NAME,NO_SUBDIRS,SD_SSI3);
									}while(next_cluster!=0x0FFFFFFF && next_cluster!=0xFFFFFFFF);
									char i;
									clear_screen();
									max_menu_items=0;
									for(i=0;i<12;i++)
									{
										unsigned char name[25];
										get_name(i,name);
										if(name[0]!=0x00)
										{
											add_menu_element(name);
											max_menu_items++;
										}
									}
									current_position=0;
									clear_screen();
									show_menu(0/*,SSI0*/);
								}
							}
						}
					}
					break;
				}
				case 0x10: //PB4 (Decrease current value)
				{
					if(menu == MAIN_MENU && (current_position == 1 || current_position == 2 || current_position == 5))
					{
						if(current_position == 1 && bright > MIN_BRIGHT && bright <=MAX_BRIGHT)
						{
							bright=bright-5;
							char str[]="Bright: ";
							convert_value_to_string(str, bright);
							strcat(str," %");
							clear_screen();
							modify_menu_element(current_position,str);
							show_menu(current_position);
						}
						else
						{
							if(current_position == 2 && delay > MIN_DELAY && delay <= MAX_DELAY)
							{
								delay=delay-5;
								char str[]="Delay: ";
								convert_value_to_string(str, delay);
								strcat(str," ms");
								clear_screen();
								modify_menu_element(current_position,str);
								show_menu(current_position);
							}
							else
							{
								if(current_position == 5)
								{
									if(buzzer == BUZZER_ON)
									{
										clear_screen();
										modify_menu_element(5,"Buzzer: off");
										show_menu(current_position);
										buzzer = BUZZER_OFF;
									}
									else
									{
										clear_screen();
										modify_menu_element(5,"Buzzer: on");
										show_menu(current_position);
										buzzer = BUZZER_ON;
									}
								}
							}
						}
					}
					else
					{
						if(menu == FILE_MENU)
						{
							subdir--;
							clear_screen();
							if(subdir>0)
							{
								restore_previous_menu();
							}
							else
							{
								restore_main_menu();
							}
							show_menu(0);
							menu=MAIN_MENU;
						}
						if(menu == MAIN_MENU)
						{
							clear_screen();
							delete_menu();
							set_init_menu();
							menu=INIT_MENU;
							show_menu(2);
							current_position=2;
							go_back=1;
						}
					}
					//clear_ledarray();
					break;
				}
				case 0x20: //PB5 (Open file and send data to the LEDs)
				{
					if(file_pos!=-1)
					{
						if(buzzer == BUZZER_ON)
						{
							play_sound(526);
							SysTick_Wait50ms(40);
							play_sound(0);
						}
						disable_backlight(SSI0);
						clear_screen();
						set_brightness(bright);
						set_delay(delay);
						long next_cluster=get_first_cluster(file_pos);
						do
						{
							next_cluster=open_file(next_cluster,SD_SSI3);
						}while(next_cluster!=0x0FFFFFFF && next_cluster!=0xFFFFFFFF);
						//send_to_leds();
						//int i=0;
						clear_ledarray();
						show_menu(4);
						current_position=4;
						enable_backlight(/*SSI0*/);
						if(buzzer == BUZZER_ON)
						{
							GPIO_PORTB_DATA_R|=0x40;
							SysTick_Wait50ms(40);
							GPIO_PORTB_DATA_R&=~0x40;
						}
					}
					else
					{
						GPIO_PORTB_DATA_R|=0x40;
						SysTick_Wait50ms(20);
						GPIO_PORTB_DATA_R&=~0x40;
					}
					break;
				}
			}
		}
		else
		{
			if(portE_data > 0x00)
			{
				//PE1 (down), PE4 (increase) and PE5 (up)
				switch(portE_data)
				{
					case 0x02: // PE1 (down)
					{
						if(menu==MAIN_MENU && current_position==2)
						{
							current_position=4;
						}
						else
						{
							if(current_position<11 && current_position<(max_menu_items-1))
							{
								current_position++;
							}
						}
						clear_screen();
						show_menu(current_position);
						break;
					}
					case 0x10: // PE4 (increase value)
					{
						if(menu == MAIN_MENU && (current_position == 1 || current_position == 2 || current_position == 5))
						{
							if(current_position == 1 && bright >= MIN_BRIGHT && bright < MAX_BRIGHT)
							{
								bright=bright+5;
								char str[]="Bright: ";
								convert_value_to_string(str, bright);
								strcat(str," %");
								clear_screen();
								modify_menu_element(1,str);
								show_menu(1);
							}
							else
							{
								if(current_position == 2 && delay >= MIN_DELAY && delay < MAX_DELAY)
								{
									delay=delay+5;
									char str[]="Delay: ";
									convert_value_to_string(str, delay);
									strcat(str," ms");
									clear_screen();
									modify_menu_element(2,str);
									show_menu(2);
									show_menu(current_position);
								}
								else
								{
									if(current_position == 5)
									{
										if(buzzer == BUZZER_ON)
										{
											clear_screen();
											modify_menu_element(5,"Buzzer: off");
											show_menu(current_position);
											buzzer = BUZZER_OFF;
										}
										else
										{
											clear_screen();
											modify_menu_element(5,"Buzzer: on");
											show_menu(current_position);
											buzzer = BUZZER_ON;
										}
									}
								}
							}
						}
						break;
					}
					case 0x20: // PE5 (up)
					{
						if(current_position>0)
						{
							if(menu==MAIN_MENU && current_position==4)
							{
								current_position=2;
							}
							else
							{
								if(menu!=MAIN_MENU || (menu==MAIN_MENU && current_position>0))
								{
									current_position--;
								}
							}
						}
						clear_screen();
						show_menu(current_position);
						break;
					}
				}
			}
		}
		SysTick_Wait50ms(10);
	}
}

void manage_paint_menu()
{
	char led_colors[144][3];
	int led_position=0;
	char c,c2,color=-1,go_back=0,just_came=0,lights=LIGHTS_OFF;
	while(GPIO_PORTB_DATA_R==0x02);
	clear_ledarray();
	set_brightness(5);
	current_position=1;
	for(c=0;c<144;c++)
	{
		for(c2=0;c2<3;c2++)
		{
			led_colors[c][c2]=0;
		}
	}
	while(go_back==0)
	{
		// PE5 (SW6) - Up
		// PE1 (SW3) - Down
		// PE4 (SW2) - Increase
		// PB4 (SW4) - Decrease
		// PB1 (SW1) - Select
		// PB0 (SW0) - Backlight on/off
		// PB5 (SW5) - Start
		//Check which button is selected
		char portB_data=GPIO_PORTB_DATA_R&0x33;
		char portE_data=GPIO_PORTE_DATA_R&0x32;
		if(portB_data > 0x00)
		{
			switch(portB_data)
			{
				case 0x01: //PB0 (Change backlight status)
				{
					if(backlight==BL_ON)
					{
						disable_backlight(/*SSI0*/);
						backlight=BL_OFF;
					}
					else
					{
						enable_backlight(/*SSI0*/);
						backlight=BL_ON;
					}
					break;
				}
				case 0x02: //PB1 (Select element at current position)
				{
					if(menu==PAINT_MENU)
					{
						/*if(color!=-1)
						{
							delete_menu();
							set_color_menu();
							show_menu(1);
							menu=COLOR_MENU;
						}*/
						switch(current_position)
						{
							case 1:
							{
								color=RED;
								clear_screen();
								delete_menu();
								set_color_menu();
								show_menu(1);
								menu=COLOR_MENU;
								led_position=1;
								break;
							}
							case 2:
							{
								color=GREEN;
								clear_screen();
								delete_menu();
								set_color_menu();
								show_menu(1);
								current_position=1;
								menu=COLOR_MENU;
								led_position=1;
								break;
							}
							case 3:
							{
								color=BLUE;
								clear_screen();
								delete_menu();
								set_color_menu();
								show_menu(1);
								current_position=1;
								menu=COLOR_MENU;
								led_position=1;
								break;
							}
							case 4:
							{//random
								clear_ledarray();
								clear_screen();
								screen_write("Generating...",ALIGN_CENTRE_CENTRE/*,SSI0*/);
								srand(time(NULL));
								for(c=0;c<144;c++)
								{
									for(c2=0;c2<3;c2++)
									{
										led_colors[c][c2]=rand()%255;
									}
								}
								clear_screen();
								set_paint_menu();
								show_menu(1);
								current_position=1;
								//menu=COLOR_MENU;
								break;
							}
							case 5:
							{//monochrome
								clear_ledarray();
								for(c=0;c<144;c++)
								{
									for(c2=0;c2<3;c2++)
									{
										if(c%2==0)
										{
											led_colors[c][c2]=0;
										}
										else
										{
											led_colors[c][c2]=25;
										}
									}
								}
								break;
							}
							case 6:
							{
								clear_ledarray();
								for(c=0;c<144;c++)
								{
									for(c2=0;c2<3;c2++)
									{
										led_colors[c][c2]=0;
									}
								}
								break;
							}
						}
					}
					else
					{
						if(menu==COLOR_MENU && current_position!=2)
						{
							int BrightLimit = map(get_brightness(),0,100,0,255);
							switch(color)
							{
								case RED:
								{
									led_colors[led_position-1][0]=0;
									led_colors[led_position-1][1]=map(255,0,255,0,BrightLimit);
									led_colors[led_position-1][2]=0;
									break;
								}
								case GREEN:
								{
									led_colors[led_position-1][0]=map(255,0,255,0,BrightLimit);
									led_colors[led_position-1][1]=0;
									led_colors[led_position-1][2]=0;
									break;
								}
								case BLUE:
								{
									led_colors[led_position-1][0]=0;
									led_colors[led_position-1][1]=0;
									led_colors[led_position-1][2]=map(255,0,255,0,BrightLimit);
									break;
								}
								/*case RESET_COLORS:
								{
									clear_ledarray();
									break;
								}*/
							}
						}
						else
						{
							if(menu == COLOR_MENU && current_position==3)
							{
								delete_menu();
								menu=PAINT_MENU;
								clear_screen();
								set_paint_menu();
								show_menu(1);
								current_position=1;
							}
						}
					}
					break;
				}
				case 0x10: //PB4 (Decrease current value / back)
				{
					if(menu == COLOR_MENU && current_position==1 && led_position>1)
					{
						led_position--;
						char str[]="LED: ";
						convert_value_to_string(str, led_position);
						clear_screen();
						modify_menu_element(1,str);
						show_menu(1);
					}
					else
					{
						if(menu == COLOR_MENU && current_position==2 && bright>5)
						{
							bright=bright-5;
							set_brightness(bright);
							char str[]="Bright: ";
							convert_value_to_string(str, bright);
							strcat(str," %");
							clear_screen();
							modify_menu_element(2,str);
							show_menu(2);
						}
						else
						{
							if(menu==CURRENTLY_PAINTING)
							{
								//go_back=1;
								delete_menu();
								menu=COLOR_MENU;
								clear_screen();
								set_color_menu();
								show_menu(1);
							}
							else
							{
								if(menu == COLOR_MENU && current_position==3)
								{
									delete_menu();
									menu=PAINT_MENU;
									clear_screen();
									set_paint_menu();
									show_menu(1);
									current_position=1;
								}
								else
								{
									if(menu==PAINT_MENU)
									{
										delete_menu();
										menu=INIT_MENU;
										clear_screen();
										set_init_menu();
										show_menu(2);
										current_position=2;
										go_back=1;
									}
								}
							}
						}
					}
					break;
				}
				case 0x20: //PB5 (Send data to the LEDs)
				{
					if(backlight==BL_ON)
					{
						disable_backlight(/*SSI0*/);
						backlight=BL_OFF;
					}
					else
					{
						enable_backlight(/*SSI0*/);
						backlight=BL_ON;
					}
					if(lights==LIGHTS_ON)
					{
						clear_ledarray();
						lights=LIGHTS_OFF;
					}
					else
					{
						char k;
						for(c=0;c<144;c++)
						{
							for(c2=0;c2<3;c2++)
							{
								for(k=0;k<8;k++)
								{
									if(led_colors[c][c2] & (0x80 >> k))
									{
										send_bit(1);
									}
									else
									{
										send_bit(0);
									}
								}
							}
						}
						lights=LIGHTS_ON;
						//menu=CURRENTLY_PAINTING;
					}
					break;
				}
			}
		}
		else
		{
			if(portE_data > 0x00)
			{
				//PE1 (down), PE4 (increase) and PE5 (up)
				switch(portE_data)
				{
					case 0x02: // PE1 (down)
					{
						if(menu==PAINT_MENU && current_position<6)
						{
							current_position++;
						}
						else
						{
							if(menu==COLOR_MENU && current_position<3)
							{
								current_position++;
							}
						}
						clear_screen();
						show_menu(current_position);
						break;
					}
					case 0x10: // PE4 (increase value)
					{
						if(menu == COLOR_MENU && current_position==1 && led_position<144)
						{
							led_position++;
							char str[]="LED: ";
							convert_value_to_string(str,led_position);
							clear_screen();
							modify_menu_element(1,str);
							show_menu(1);
						}
						else
						{
							if(menu == COLOR_MENU && current_position==2 && bright<100)
							{
								bright=bright+5;
								set_brightness(bright);
								char str[]="Bright: ";
								convert_value_to_string(str, bright);
								strcat(str," %");
								clear_screen();
								modify_menu_element(2,str);
								show_menu(2);
							}
						}

						break;
					}
					case 0x20: // PE5 (up)
					{
						if(menu==PAINT_MENU && current_position>1)
						{
							current_position--;
						}
						else
						{
							if(menu==COLOR_MENU && current_position>1)
							{
								current_position--;
							}
						}
						clear_screen();
						show_menu(current_position);
						break;
					}
				}
			}
		}
		SysTick_Wait50ms(10);
	}
	go_back=0;
}

void convert_value_to_string(char str[], int value)
{
	if(value<10)
	{
		char str1[2];
		str1[0]=value+'0';
		str1[1]='\0';
		strcat(str,str1);
	}
	else
	{
		if(value>=10 && value <=99)
		{
			char str2[3];
			int value2=value/10;
			str2[0]=value2+'0';
			str2[1]=(value%10)+'0';
			str2[2]='\0';
			strcat(str,str2);
		}
		else
		{
			char str3[4];
			int value2=value;
			str3[0]=(value2/100)+'0';
			str3[1]=((value2/10)-((value2/100)*10))+'0';
			int value3=value%10;
			str3[2]=(value%10)+'0';
			str3[3]='\0';
			strcat(str,str3);
		}
	}
}

/*Play a sound in the SMD buzzer*/
void play_sound(uint16_t high)
{
	TIMER0_TAMATCHR_R = TIMER0_TAILR_R-high; // duty cycle = high/period
}
