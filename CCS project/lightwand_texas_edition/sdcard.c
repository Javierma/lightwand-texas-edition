/*
 * sdcard.c
 *
 *  Created on: 15/09/2016
 *  Author: Javier Martínez Arrieta
 *  Version: 1.0
 *  This is part of the sdcard library, with functions that will allow you to read and (in the future) write in an SD card formatted using FAT32 (single partition).
 */

 /*  Copyright (C) 2016 Javier Martínez Arrieta
 *
 *  This project is licensed under Creative Commons Attribution-Non Commercial-Share Alike 4.0 International (CC BY-NC-SA 4.0). According to this license you are free to:
 *  Share & copy and redistribute the material in any medium or format.
 *  Adapt & remix, transform, and build upon the material.
 *  The licensor cannot revoke these freedoms as long as you follow the license terms.
 *	Complete information about this license can be found at: https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
 *
 */


/* Part of this example (partially modified functions rcvr_datablock, rcvr_spi_m, disk_timerproc, Timer5A_Handler, Timer5_Init, is_ready, send_command and part of initialize_sd) accompanies the books
   Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers, Volume 3,
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2013

   Volume 3, Program 6.3, section 6.6   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file (concretely the functions mentioned)
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "sdcard.h"
#include "inc/tm4c123gh6pm.h"
#include "1wire.h"
#include "Nokia5110.h"
#include <stdio.h>


unsigned char Timer1,Timer2;
unsigned long lba_begin_address,number_of_sectors,lba_addr,cluster_start,file_size,cluster_begin_lba,fat_begin_lba,sectors_per_fat=0,root_dir_first_cluster;
unsigned long previous_cluster=0,cluster_dir=0,reserved_sectors=0;
unsigned char sectors_per_cluster=0;
char fd_count=0,current_count=0;
char finish=0,first_file=1;
int row=0,column=0,number=0,rows=0,columns=0,total=0,current=0,BrightLimit,l=0,num=0,num2=0;
//int l=0;
uint8_t pixel_content[NUM_LEDS*3];

typedef struct
{
	char hour;
	char minute;
	unsigned int year;
	char month;
	char day;
	long size;
	long first_cluster;
}tfile_info;

typedef struct
{
	tfile_info info;
	unsigned char file_dir_name[255];
}tfile_name;

typedef struct
{
	tfile_name name;
	enum type_of_file
	{
		IS_NONE,
		IS_DIR,
		IS_FILE
	}type;
}tfile_dir;

tfile_dir file_dir[40];

/**
 * Writes to the SD card. Yet to be tested
 */
void sd_write(char message)
{
	unsigned short volatile rcvdata;
	while((SSI3_SR_R&SSI_SR_TNF)==0){};
	SSI3_DR_R = message;
	while((SSI3_SR_R&SSI_SR_RNE)==0){};
	rcvdata = SSI3_DR_R;
}

/*
 * Reads from the SD card
 */
unsigned char sd_read()
{
	while((SSI3_SR_R&SSI_SR_TNF)==0){};    // wait until room in FIFO
	SSI3_DR_R = 0xFF;                      // data out, garbage
	while((SSI3_SR_R&SSI_SR_RNE)==0){};    // wait until response
	return (unsigned char) SSI3_DR_R;      // read received data
}

/*
 * Wait until sd card is ready
 */
unsigned char is_ready(enum SSI SSI_number){
  unsigned char response;
  Timer2 = 50;    /* Wait for ready in timeout of 500ms */
  do
  {
    response = sd_read();
  }while ((response != 0xFF) && Timer2);
  return response;
}


/*
 * Sends the command, preparing the packet to be sent
 */
unsigned char send_command(unsigned char command, unsigned long argument, enum SSI SSI_number)
{
	/* Argument */
	unsigned char crc, response,n;
	if (is_ready(SSI_number) != 0xFF) return 0xFF;

    /* Send command packet */
	sd_write(command);                        /* Command */
	sd_write((unsigned char)(argument >> 24));        /* Argument[31..24] */
	sd_write((unsigned char)(argument >> 16));        /* Argument[23..16] */
	sd_write((unsigned char)(argument >> 8));            /* Argument[15..8] */
	sd_write((unsigned char)argument);                /* Argument[7..0] */
	crc = 0;
	if (command == CMD0)
	{
		crc = 0x95;            /* CRC for CMD0(0) */
	}
	if (command == CMD8)
	{
		crc = 0x87;            /* CRC for CMD8(0x1AA) */
	}
	sd_write(crc);

    /* Receive command response */
	if (command == CMD12) sd_write(0xFF);        /* Skip a stuff byte when stop reading */
	n = 10;                                /* Wait for a valid response in timeout of 10 attempts */
	do
	{
		response = sd_read();
	}while ((response & 0x80) && --n);
	return response;
}

/*
 * Initializes the SD card
 */
void initialise_sd(enum SSI SSI_number)
{
	unsigned char i;
	unsigned char ocr[4];
	unsigned char sd_type;
	//Sends a 1 through CS and MOSI lines for at least 74 clock cycles
	cs_high();
	dummy_clock();
	cs_low();
	i=0;
	/*Checks if SD card is in IDLE mode. If so, response will be 1*/
	if(send_command(CMD0, 0,SSI_number) == 1)
	{
		Timer1 = 100; /* Initialization timeout of 1000 msec */
		if(send_command(CMD8, 0x1AA,SSI_number) == 1)
		{
			/* SDC Ver2+ */
			for(i=0;i<4;i++)
			{
				ocr[i]=sd_read();
			}
			if(ocr[2]==0x01&&ocr[3]==0xAA)
			{
				//sends ACMD41, which is a command sequence of CMD55 and CMD41
				do
				{
					if(send_command(CMD55, 0, SSI_number) <= 1 && send_command(CMD41, 1UL << 30,SSI_number) == 0)
					{
						break; //R1 response is 0x00
					}
				}while(Timer1);
				if(Timer1 && send_command(CMD58, 0,SSI_number) == 0)
				{
					for(i=0;i<4;i++)
					{
						ocr[i]=sd_read();
						sd_type = (ocr[0] & 0x40) ? 6 : 2;
					}
				}
			}
		}
		else
		{
			/*It is not SD version 2 or upper*/
			sd_type=(send_command(CMD55, 0,SSI_number)<=1 &&send_command(CMD41, 0,SSI_number) <= 1) ? 2 : 1;    /*Check if SD or MMC*/
			do
			{
				if(sd_type==2)
				{
					if(send_command(CMD55, 0,SSI_number)<=1&&send_command(CMD41, 0,SSI_number)==0) /*ACMD41*/
			        {
						break;
			        }
				}
				else
				{
					if (send_command(CMD1, 0,SSI_number) == 0) /*CMD1*/
					{
						break;
					}
			    }
			}while(Timer1);
			if(!Timer1 || send_command(CMD16, 512,SSI_number) != 0)    /*Select R/W block length if timeput not reached*/
			{
				sd_type=0;
			}
		}
	}
}

void sd_startSSI3()
{
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000008;   		// activate clock for Port D
	SYSCTL_RCGCSSI_R|= SYSCTL_RCGCSSI_R3;	// activate clock for SSI3
	delay = SYSCTL_RCGC2_R;         		// allow time for clock to stabilize
	GPIO_PORTD_LOCK_R=0x4C4F434B;
	GPIO_PORTD_CR_R=0x80;
	GPIO_PORTD_DIR_R |= 0x08;             // make PD3 out
	GPIO_PORTD_AFSEL_R |= 0x0D;           // enable alt funct on PD0, PD2 and PD3
	GPIO_PORTD_AFSEL_R &= ~0xF2;          // disable alt funct on PD1
	GPIO_PORTD_DEN_R |= 0x0F;             // enable digital I/O on PD0, PD1, PD2 and PD3
	// configure PD0, PD2 and PD3 as SSI
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF00F0)+0x00001101;
	// configure PD1 as GPIO
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFFFFF0F)+0x00000000;
	                                        // configure PD0, PD1 and PD3 as SSI
	GPIO_PORTD_AMSEL_R &= ~0xCF;          // disable analog functionality on PD0, PD1, PD2, PD3, PD6 and PD7
	SSI3_CR1_R&=~SSI_CR1_SSE;		  		// Disable SSI while configuring it
	SSI3_CR1_R&=~SSI_CR1_MS;		  		// Set as Master
	SSI3_CC_R|=SSI_CC_CS_M; 				// Configure clock source
	SSI3_CC_R|=SSI_CC_CS_SYSPLL; 			// Configure clock source
	SSI3_CC_R|=SSI_CPSR_CPSDVSR_M;		// Configure prescale divisor
	SSI3_CPSR_R = (SSI3_CPSR_R&~SSI_CPSR_CPSDVSR_M)+10; // must be even number
	SSI3_CR0_R |=0x0300;
	SSI3_CR0_R &= ~(SSI_CR0_SPH | SSI_CR0_SPO);
	SSI3_CR0_R = (SSI3_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO;
	                                        // DSS = 8-bit data
	SSI3_CR0_R = (SSI3_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_8;
	SSI3_CR1_R|=SSI_CR1_SSE;		  // 3)Enable SSI
}

/*
 * Makes use of the clock along with CS and MOSI to make the SD card readable using SPI
 */
void dummy_clock()
{
	unsigned int i;
	//In order to initialize and set SPI mode, there should be at least 74 clock cycles with MOSI and CS set to 1
	for ( i = 0; i < 2; i++);
	//CS set high
	cs_high();
	//Disables SSI on TX/MOSI pin to send a 1
	tx_high();
	for ( i = 0; i < 10; i++)
	{
		sd_write(0xFF);
	}
	tx_SSI();
}

/*
 * Gets the first cluster of a file or directory
 */
long get_first_cluster(int pos)
{
	return file_dir[pos].name.info.first_cluster;
}

/*
 * Gets the first cluster of the root directory
 */
long get_root_dir_first_cluster(void)
{
	return root_dir_first_cluster;
}

/*
 * Makes chip select line high
 */
void cs_high()
{
	GPIO_PORTD_DATA_R|=0x02;
}

/*
 * Makes chip select line low
 */
void cs_low()
{
	GPIO_PORTD_DATA_R&=~0x02;
}

/*
 * Writes a '1' in the transmission line of the SSI that is being used
 */
void tx_high()
{
	GPIO_PORTD_AFSEL_R &= ~0x08;           // disable alt funct on PD3
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0FFF);
	GPIO_PORTD_DATA_R |= 0x08;            // PD3 high
}

/*
 * Configure again the transmission line of the SSI that is being used
 */
void tx_SSI()
{
	GPIO_PORTD_AFSEL_R |= 0x08;           // enable alt funct on PD3
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0FFF)+0x00001000;
}

/*Change speed to (?) 8 MHz*/
void change_speed()
{
	SSI3_CC_R|=SSI_CPSR_CPSDVSR_M;// Configure prescale divisor
	SSI3_CPSR_R = (SSI3_CPSR_R&~SSI_CPSR_CPSDVSR_M)+2; // must be even number
	SSI3_CR0_R |=0x0000;
}

void read_csd(enum SSI SSI_number)
{
	unsigned char csd[16];
	send_command(CMD9,0,SSI_number);
	rcvr_datablock(csd,16);
}

/*
 * Verify if file system is FAT32
 */
void read_first_sector(enum SSI SSI_number)
{
	unsigned char buffer[512];
	if (send_command(CMD17, 0x00000000,SSI_number) == 0)
	{
		rcvr_datablock(buffer, 512);
	}
	if((buffer[450] == 0x0B || buffer[450] == 0x0C) && buffer[510] == 0x55 && buffer[511] == 0xAA)
	{
		lba_begin_address=(unsigned long)buffer[454]+(((unsigned long)buffer[455])<<8)+(((unsigned long)buffer[456])<<16)+(((unsigned long)buffer[457])<<24);
		number_of_sectors=(unsigned long)buffer[458]+(((unsigned long)buffer[459])<<8)+(((unsigned long)buffer[460])<<16)+(((unsigned long)buffer[461])<<24);
		screen_write("FS is FAT32",ALIGN_CENTRE_CENTRE/*,SSI0*/);
		SysTick_Wait50ms(40);
	}
	else
	{
		screen_write("FS is not FAT32",ALIGN_CENTRE_CENTRE/*,SSI0*/);
		SysTick_Wait50ms(2000);
		exit(1);
	}

}

/*
 * Reads the necessary data so as to be able to access the files and directories
 */
void read_disk_data(enum SSI SSI_number)
{
	unsigned char buffer[512];
	if (send_command(CMD17, lba_begin_address,SSI_number) == 0)
	{
		rcvr_datablock(buffer, 512);
	}
	reserved_sectors=(unsigned long)buffer[14] + (((unsigned long)buffer[15])<<8);
	fat_begin_lba = lba_begin_address + reserved_sectors/*(unsigned long)buffer[14] + (((unsigned long)buffer[15])<<8)*/; //Partition_LBA_BEGIN + Number of reserved sectors
	sectors_per_fat=((unsigned long)buffer[36]+(((unsigned long)buffer[37])<<8)+(((unsigned long)buffer[38])<<16)+(((unsigned long)buffer[39])<<24));
	cluster_begin_lba = fat_begin_lba + ((unsigned long)buffer[16] * ((unsigned long)buffer[36]+(((unsigned long)buffer[37])<<8)+(((unsigned long)buffer[38])<<16)+(((unsigned long)buffer[39])<<24)));//Partition_LBA_Begin + Number_of_Reserved_Sectors + (Number_of_FATs * Sectors_Per_FAT);
	sectors_per_cluster = (unsigned char) buffer[13];//BPB_SecPerClus;
	root_dir_first_cluster = (unsigned long)buffer[44]+(((unsigned long)buffer[45])<<8)+(((unsigned long)buffer[46])<<16)+(((unsigned long)buffer[47])<<24);//BPB_RootClus;
	lba_addr = cluster_begin_lba + ((root_dir_first_cluster/*cluster_number*/ - 2) * (unsigned long)sectors_per_cluster);
}

/*
 * List directories and files using the long name (if it has) or the short name, listing subdirectories as well if asked by the user
 */
long get_files_and_dirs(long next_cluster,enum name_type name, enum get_subdirs subdirs, enum SSI SSI_number)
{
    unsigned char buffer[512];
    char filename[255] = "";
    int position=0,filename_position=0;
    int n=0;
    unsigned long count=10,sectors_to_be_read=sectors_per_cluster;//Calculate this
    long address=cluster_begin_lba + ((next_cluster - 2) * (unsigned long)sectors_per_cluster);
    if(cluster_dir == next_cluster)
    {
        cluster_dir=0;
    }
    if(send_command(CMD18,address,SSI_number)==0)
    {
        do
        {
            rcvr_datablock(buffer, 512);
            do
            {
                if(position<512 && filename_position<255)
                {//Long filename text - 11th byte is 0x0F
                    if(position%32==0)
                    {//Check if file has a long filename text, normal record with short filename, unused or end of a directory
                        if(buffer[position]==0x00 || buffer[position]==0x2E)
                        {//End of directory
                            position=position+32;
                        }
                        else
                        {
                            if(buffer[position]==0xE5)
                            {//Deleted file or directory that is maintained until overriden
                                position=position+32;
                            }
                            else
                            {
                                if(name==LONG_NAME)//Review this
                                {//Review this as there are files which long name are not read, probably because they only have one sequence for the name
                                    short keep_counting=1,do_not_continue=0,is_dir=0, keep_reading = 1;
                                    uint8_t seq_num = 0, filename_read_finished = 0;
                                    if(buffer[position+11]==0x0F)
                                    {
                                        do
                                        {
                                            //Check if it is last record group of the filename
                                            if(buffer[position+11]!=0x0F)
                                            {
                                                keep_reading = 0;
                                                if(position == 512)
                                                {
                                                    filename_read_finished = 0;
                                                }
                                                else
                                                {
                                                    filename_read_finished = 1;
                                                }
                                            }
                                            //Get the sequence number
                                            seq_num=buffer[position]&0x1F;

                                            uint8_t k=0,l=0;
                                            while(k<32 && keep_reading == 1)
                                            {
                                                if((k>0 && k<11) || (k>13 && k<26) || (k>27 && k<32))
                                                {
                                                    filename[(32*(seq_num-1))+l] = buffer[position];
                                                    l++;
                                                }
                                                k++;
                                                position++;
                                            }
                                        }while(keep_reading == 1);
                                    }
                                    else
                                    {
                                        //Filename exactly has a length of 8 bytes and either the base name or the extension have all its characters in capital letters, so we need to check
                                        if((buffer[position+12]&0x18)>0)
                                        {
                                            if((buffer[position+12]&0x10)>0 && (buffer[position+12]&0x08)>0)
                                            {
                                                uint8_t k=0;
                                                while(k<11)
                                                {
                                                    if(k < 8)
                                                    {
                                                        filename[k] = buffer[position] + 32;
                                                    }
                                                    else
                                                    {
                                                        if(k >= 8)
                                                        {
                                                            filename[k+1] = buffer[position] + 32;
                                                        }
                                                    }
                                                    k++;
                                                    position++;
                                                }
                                                filename[8] = '.';
                                            }
                                            else
                                            {
                                                //Extension is in lowercase, basename is in uppercase
                                                if((buffer[position+12]&0x10)>0)
                                                {
                                                    uint8_t k=0;
                                                    while(k<11)
                                                    {
                                                        if(k<8)
                                                        {
                                                            filename[k] = buffer[position];
                                                        }
                                                        else
                                                        {
                                                            filename[k+1] = buffer[position] + 32;
                                                        }
                                                        k++;
                                                        position++;
                                                    }
                                                    filename[8] = '.';
                                                }
                                                else
                                                {
                                                    //Extension in uppercase, basename in lowercase
                                                    if((buffer[position+12]&0x08)>0)
                                                    {
                                                        uint8_t k=0;
                                                        while(k<11)
                                                        {
                                                            if(k<8)
                                                            {
                                                                filename[k] = buffer[position] + 32;
                                                            }
                                                            else
                                                            {
                                                                filename[k+1] = buffer[position];
                                                            }
                                                            k++;
                                                            position++;
                                                        }
                                                        filename[8] = '.';
                                                    }
                                                }
                                            }
                                            filename_read_finished = 1;
                                            position = position - 11;
                                        }
                                        else
                                        {
                                            //Both basename and extension are uppercase
                                            if((buffer[position+12]&0x18)==0)
                                            {
                                                uint8_t k=0;
                                                while(k<11)
                                                {
                                                    if(k < 8)
                                                    {
                                                        filename[k] = buffer[position];
                                                    }
                                                    else
                                                    {
                                                        if(k >= 8)
                                                        {
                                                            filename[k+1] = buffer[position];
                                                        }
                                                    }
                                                    k++;
                                                    position++;
                                                }
                                                filename[8] = '.';
                                                filename_read_finished = 1;
                                                position = position - 11;
                                            }
                                        }
                                    }
                                    //Check if filename is a System file and the filename reading was completed
                                    if((buffer[position+11]&0x0E)==0x00 && filename_read_finished == 1)
                                    {
                                        if((buffer[position+11]&0x30)==0x10)
                                        {
                                            file_dir[fd_count].type=IS_DIR;
                                        }
                                        else
                                        {
                                            if((buffer[position+11]&0x30)==0x20)
                                            {
                                                file_dir[fd_count].type=IS_FILE;
                                            }
                                        }
                                        uint8_t k = 0;
                                        while(k<255)
                                        {
                                            file_dir[fd_count].name.file_dir_name[k]=filename[k];
                                            k++;
                                        }
                                        //Reset filename
                                        k = 0;
                                        while(k<255)
                                        {
                                            filename[k]=0x00;
                                            k++;
                                        }
                                        int time=(((int)(buffer[position+23]))<<8) + ((int)buffer[position+22]);
                                        int date=(((int)(buffer[position+25]))<<8) + ((int)buffer[position+24]);
                                        file_dir[fd_count].name.info.minute=(time&0x07E0)>>5;
                                        file_dir[fd_count].name.info.hour=(time&0xF800)>>11;
                                        file_dir[fd_count].name.info.month=((date&0x01E0)>>5);
                                        file_dir[fd_count].name.info.year=((date&0xFE00)>>9)+1980;
                                        file_dir[fd_count].name.info.day=date&0x001F;
                                        file_dir[fd_count].name.info.size=(long)((buffer[position+31])<<24)+(long)((buffer[position+30])<<16)+(long)((buffer[position+29])<<8)+(long)(buffer[position+28]);
                                        file_dir[fd_count].name.info.first_cluster=(long)((buffer[position+21])<<24)+(long)((buffer[position+20])<<16)+(long)((buffer[position+27])<<8)+(long)(buffer[position+26]);
                                        position = position + 32;
                                    }
                                    else
                                    {
                                        if(filename_read_finished == 1)
                                        {
                                            //Reset filename
                                            uint8_t k = 0;
                                            while(k<255)
                                            {
                                                filename[k]=0x00;
                                                k++;
                                            }
                                            position++;
                                        }
                                    }
                                }
                                else
                                {//Normal record with short filename
                                    //Check it is not a system file or directory and names to be read are short names
                                    if((buffer[position+11]&0x0E)==0x00 && name==SHORT_NAME)
                                    {
                                        if((buffer[position+11]&0x30)==0x10)
                                        {
                                            file_dir[fd_count].type=IS_DIR;
                                        }
                                        else
                                        {
                                            if((buffer[position+11]&0x30)==0x10)
                                            {
                                                file_dir[fd_count].type=IS_FILE;
                                            }
                                        }
                                        for(n=0;n<11;n++)
                                        {
                                            file_dir[fd_count].name.file_dir_name[n]=buffer[position];
                                            position++;
                                        }
                                        int time=(((int)(buffer[position-11+23]))<<8) + ((int)buffer[position-11+22]);
                                        int date=(((int)(buffer[position-11+25]))<<8) + ((int)buffer[position-11+24]);
                                        file_dir[fd_count].name.info.minute=(time&0x07E0)>>5;
                                        file_dir[fd_count].name.info.hour=(time&0xF800)>>11;
                                        file_dir[fd_count].name.info.month=((date&0x01E0)>>5);
                                        file_dir[fd_count].name.info.year=((date&0xFE00)>>9)+1980;
                                        file_dir[fd_count].name.info.day=date&0x001F;
                                        file_dir[fd_count].name.info.size=(long)((buffer[position-11+31])<<24)+(long)((buffer[position-11+30])<<16)+(long)((buffer[position-11+29])<<8)+(long)(buffer[position-11+28]);
                                        file_dir[fd_count].name.info.first_cluster=(long)((buffer[position-11+21])<<24)+(long)((buffer[position-11+20])<<16)+(long)((buffer[position-11+27])<<8)+(long)(buffer[position-11+26]);
                                    }
                                    else
                                    {
                                        if(position==512)
                                        {
                                            //position=0;
                                        }
                                        else
                                        {
                                            position++;
                                        }
                                    }
                                }
                            }
                        }
                        clean_name();
                        if(file_dir[fd_count].name.file_dir_name[0]!=0xFF && file_dir[fd_count].name.file_dir_name[0]!=0x00)
                        {
                            if(file_dir[fd_count].type==IS_DIR)
                            {
                                printf("%d. (DIR)\t", number);
                            }
                            else
                            {
                                printf("%d. (FILE)\t", number);
                            }
                            char i;
                            for(i=0;i<255;i++)
                            {
                                if(file_dir[fd_count].name.file_dir_name[i]!=0x00)
                                {
                                    printf("%c",file_dir[fd_count].name.file_dir_name[i]);
                                }
                            }
                            printf("\t\t");
                            printf("%d/%d/%d    %d:%d\n",file_dir[fd_count].name.info.day,file_dir[fd_count].name.info.month,file_dir[fd_count].name.info.year,file_dir[fd_count].name.info.hour,file_dir[fd_count].name.info.minute);
                            fd_count++;
                            number++;
                        }
                    }
                    else
                    {
                        if(position==512)
                        {
                            //position=0;
                        }
                        else
                        {
                            position++;
                        }
                    }
                }
                else
                {
                    if(position==512)
                    {
                        count--;
                    }
                    else
                    {
                        position++;
                    }
                }
            } while (position<512);
            position=0;
            sectors_to_be_read--;
        }while(sectors_to_be_read>0);
    }
    send_command(CMD12,0,SSI_number);
    sectors_to_be_read=(next_cluster*4)/512;
    long sector=0;
    if(send_command(CMD18,fat_begin_lba,SSI_number)==0)
    {
        do
        {
            sector++;
            rcvr_datablock(buffer, 512);
        }while(sectors_to_be_read>0);
        sector--;
    }
    send_command(CMD12,0,SSI_number);
    next_cluster=(((long)(buffer[((next_cluster*4)-(sector*512))+3]))<<24)+(((long)(buffer[((next_cluster*4)-(sector*512))+2]))<<16)+(((long)(buffer[((next_cluster*4)-(sector*512))+1]))<<8)+(long)(buffer[((next_cluster*4)-(sector*512))]);
    if((next_cluster==0x0FFFFFFF || next_cluster==0xFFFFFFFF) && current_count<40 && subdirs==GET_SUBDIRS)
    {
        while(current_count<40&&file_dir[current_count].type!=IS_DIR)
        {
            current_count++;
        }
        if(current_count<40 && file_dir[current_count].type==IS_DIR)
        {
            printf("Content of ");
            char i;
            for(i=0;i<255;i++)
            {
                if(file_dir[current_count].name.file_dir_name[i]!=0x00)
                {
                    printf("%c",file_dir[current_count].name.file_dir_name[i]);
                }
            }
            next_cluster=file_dir[current_count].name.info.first_cluster;
            current_count++;
            printf("\n\t");
        }
    }
    if(current_count==40)
    {
        number=0;
        next_cluster=0x0FFFFFFF;
    }
    return next_cluster;
}

/*
 * List directories and files using the long name (if it has) or the short name, listing subdirectories as well if asked by the user
 */
long list_dirs_and_files(long next_cluster,enum name_type name, enum get_subdirs subdirs, enum SSI SSI_number)
{
	unsigned char buffer[512];
	int position=0,filename_position=0;
	int n=0;
	unsigned long count=10,sectors_to_be_read=sectors_per_cluster;//Calculate this
	long address=cluster_begin_lba + ((next_cluster - 2) * (unsigned long)sectors_per_cluster);
	if(cluster_dir == next_cluster)
	{
		cluster_dir=0;
	}
	if(send_command(CMD18,address,SSI_number)==0)
	{
		do
		{
			rcvr_datablock(buffer, 512);
			do
			{
				if(position<512 && filename_position<255 /*&& position%32==0 && buffer[position]!=0x00 && buffer[position]!=0x05 /*&& buffer[position]!=0x00*/)
				{//Long filename text - 11th byte is 0x0F
					if(position%32==0)
					{//Check if file has a long filename text, normal record with short filename, unused or end of a directory
						if(buffer[position]==0x00 || buffer[position]==0x2E)
						{//End of directory
							position=position+32;
						}
						else
						{
							if(buffer[position]==0xE5)
							{//Deleted file or directory that is maintained until overriden
								position=position+32;
							}
							else
							{
								if(buffer[position+11]==0x0F && name==LONG_NAME)//Review this
								{//Long filename text (to be ignored?)
									//Get the number of groups of 32 bytes corresponding to the name of file or directory
									short keep_counting=1,do_not_continue=0,is_dir=0;
									int pos=position+32;
									do
									{
										if(buffer[pos+11]==0x0F)
										{
											pos=pos+32;
										}
										else
										{
											//Is it hidden, operating system or Volume ID?
											if((buffer[pos+11]&0x0E)>0x00)
											{
												do_not_continue=1;
											}
											else
											{
												if((buffer[pos+11]&0x10)==0x10)
												{
													is_dir=1;
												}
											}
											keep_counting=0;
										}
									}while(keep_counting==1);
									if(do_not_continue==0)
									{
										int num_blocks=(pos-position)/32;
										int current_block=0;
										if(is_dir)
										{
											file_dir[fd_count].type=IS_DIR;
										}
										else
										{
											file_dir[fd_count].type=IS_FILE;
										}
										//Get sequence number
										do
										{
											int seq_num=buffer[position]&0x1F;
											filename_position=32*(seq_num-1);
											position++;
											for(n=1;n<32;n++)
											{
												if((n<11 || n>13) && n!=26)
												{
													file_dir[fd_count].name.file_dir_name[filename_position+n]=buffer[position];
												}
												position++;
											}
											current_block++;
											num_blocks--;
										}while(num_blocks>0);
										clean_name();
										int time=(((int)(buffer[position/*-11*/+23]))<<8) + ((int)buffer[position/*-11*/+22]);
										int date=(((int)(buffer[position/*-11*/+25]))<<8) + ((int)buffer[position/*-11*/+24]);
										file_dir[fd_count].name.info.minute=(time&0x07E0)>>5;
										file_dir[fd_count].name.info.hour=(time&0xF800)>>11;
										file_dir[fd_count].name.info.month=((date&0x01E0)>>5);
										file_dir[fd_count].name.info.year=((date&0xFE00)>>9)+1980;
										file_dir[fd_count].name.info.day=date&0x001F;
										file_dir[fd_count].name.info.size=(long)((buffer[position/*-11*/+31])<<24)+(long)((buffer[position/*-11*/+30])<<16)+(long)((buffer[position/*-11*/+29])<<8)+(long)(buffer[position/*-11*/+28]);
										file_dir[fd_count].name.info.first_cluster=(long)((buffer[position/*-11*/+21])<<24)+(long)((buffer[position/*-11*/+20])<<16)+(long)((buffer[position/*-11*/+27])<<8)+(long)(buffer[position/*-11*/+26]);
										position=position+32;
									}
									else
									{
										position=position+(32*(pos/32));
									}
								}
								else
								{//Normal record with short filename
									//Is it a directory (not system's)?
									if((buffer[position+11]&0x30)==0x10 && (buffer[position+11]&0x0E)==0x00)
									{
										file_dir[fd_count].type=IS_DIR;
										for(n=0;n<11;n++)
										{
											file_dir[fd_count].name.file_dir_name[n]=buffer[position];
											position++;
										}
										int time=(((int)(buffer[position-11+23]))<<8) + ((int)buffer[position-11+22]);
										int date=(((int)(buffer[position-11+25]))<<8) + ((int)buffer[position-11+24]);
										file_dir[fd_count].name.info.minute=(time&0x07E0)>>5;
										file_dir[fd_count].name.info.hour=(time&0xF800)>>11;
										file_dir[fd_count].name.info.month=((date&0x01E0)>>5);
										file_dir[fd_count].name.info.year=((date&0xFE00)>>9)+1980;
										file_dir[fd_count].name.info.day=date&0x001F;
										file_dir[fd_count].name.info.size=(long)((buffer[position-11+31])<<24)+(long)((buffer[position-11+30])<<16)+(long)((buffer[position-11+29])<<8)+(long)(buffer[position-11+28]);
										file_dir[fd_count].name.info.first_cluster=(long)((buffer[position-11+21])<<24)+(long)((buffer[position-11+20])<<16)+(long)((buffer[position-11+27])<<8)+(long)(buffer[position-11+26]);
									}
									else
									{
										if(((buffer[position+11])&0x30)==0x20 && ((buffer[position+11])&0x0E)==0x00)
										{
											file_dir[fd_count].type=IS_FILE;
											for(n=0;n<11;n++)
											{
												file_dir[fd_count].name.file_dir_name[n]=buffer[position];
												position++;
											}
											int time=(((int)(buffer[position-11+23]))<<8) + ((int)buffer[position-11+22]);
											int date=(((int)(buffer[position-11+25]))<<8) + ((int)buffer[position-11+24]);
											file_dir[fd_count].name.info.minute=(time&0x07E0)>>5;
											file_dir[fd_count].name.info.hour=(time&0xF800)>>11;
											file_dir[fd_count].name.info.month=((date&0x01E0)>>5);
											file_dir[fd_count].name.info.year=((date&0xFE00)>>9)+1980;
											file_dir[fd_count].name.info.day=date&0x001F;
											file_dir[fd_count].name.info.size=(long)((buffer[position-11+31])<<24)+(long)((buffer[position-11+30])<<16)+(long)((buffer[position-11+29])<<8)+(long)(buffer[position-11+28]);
											file_dir[fd_count].name.info.first_cluster=(long)((buffer[position-11+21])<<24)+(long)((buffer[position-11+20])<<16)+(long)((buffer[position-11+27])<<8)+(long)(buffer[position-11+26]);
											//files++;
										}
										else
										{
											if(position==512)
											{
												//position=0;
											}
											else
											{
												position++;
											}
										}
									}
								}
							}
						}
						clean_name();
						if(file_dir[fd_count].name.file_dir_name[0]!=0xFF && file_dir[fd_count].name.file_dir_name[0]!=0x00)
						{
							if(file_dir[fd_count].type==IS_DIR)
							{
								//printf("%d. (DIR)\t\t", number);
							}
							else
							{
								//printf("%d. (FILE)\t\t", number);
							}
							char i;
							for(i=0;i<255;i++)
							{
								if(file_dir[fd_count].name.file_dir_name[i]!=0x00)
								{
									//printf("%c",file_dir[fd_count].name.file_dir_name[i]);
								}
							}
							//printf("\t\t");
							//printf("%d/%d/%d	%d:%d\n",file_dir[fd_count].name.info.day,file_dir[fd_count].name.info.month,file_dir[fd_count].name.info.year,file_dir[fd_count].name.info.hour,file_dir[fd_count].name.info.minute);
							fd_count++;
							number++;
						}
					}
					else
					{
						if(position==512)
						{
							//position=0;
						}
						else
						{
							position++;
						}
					}
				}
				else
				{
					if(position==512)
					{
						count--;
					}
					else
					{
						position++;
					}
				}
			} while (position<512);
			position=0;
			sectors_to_be_read--;
		}while(sectors_to_be_read>0);
	}
	send_command(CMD12,0,SSI_number);
	sectors_to_be_read=(next_cluster*4)/512;
	long sector=0;
	if(send_command(CMD18,fat_begin_lba,SSI_number)==0)
	{
		do
		{
			sector++;
			rcvr_datablock(buffer, 512);
		}while(sector<=sectors_to_be_read/*sectors_to_be_read>0*/);
		sector--;
	}
	send_command(CMD12,0,SSI_number);
	next_cluster=(((long)(buffer[((next_cluster*4)-(sector*512))+3]))<<24)+(((long)(buffer[((next_cluster*4)-(sector*512))+2]))<<16)+(((long)(buffer[((next_cluster*4)-(sector*512))+1]))<<8)+(long)(buffer[((next_cluster*4)-(sector*512))]);
	if((next_cluster==0x0FFFFFFF || next_cluster==0xFFFFFFFF) && current_count<40 && subdirs==GET_SUBDIRS)
	{
		while(current_count<40&&file_dir[current_count].type!=IS_DIR)
		{
			current_count++;
		}
		if(current_count<40 && file_dir[current_count].type==IS_DIR)
		{
			//printf("Content of ");
			char i;
			for(i=0;i<255;i++)
			{
				if(file_dir[current_count].name.file_dir_name[i]!=0x00)
				{
					//printf("%c",file_dir[current_count].name.file_dir_name[i]);
				}
			}
			next_cluster=file_dir[current_count].name.info.first_cluster;
			current_count++;
			//printf("\n\t");
		}
	}
	if(current_count==40)
	{
		number=0;
		next_cluster=0x0FFFFFFF;
	}
	return next_cluster;
}

/*
 *Reads file content.
 *Please note that this method should be modified if the file to be opened is not a txt file (concretely the content inside the for loop)
 */
long open_file(long next_cluster,enum SSI SSI_number)
{

	unsigned char buffer[512];
	long sector=0;
	long sectors_to_be_read=sectors_per_cluster;
	long address=cluster_begin_lba + ((next_cluster - 2) * (unsigned long)sectors_per_cluster);
	if(send_command(CMD18,address,SSI_number)==0)
	{
		do
		{
			rcvr_datablock(buffer, 512);
			int c=0;
			if(first_file)
			{
				columns=0;
				rows=0;
				BrightLimit = map(get_brightness(),0,100,0,255);
				int count=2;
				if(buffer[0]==0x50 && buffer[1]==0x36)
				{
					first_file=0;
					int newlines=0;
					while(newlines<2)
					{
						if(buffer[count]==0x0A)
						{
							newlines++;
						}
						count++;
					}
					do
					{
						if(columns==0)
						{
							columns=buffer[count]-'0';
						}
						else
						{
							columns=(columns*10)+(buffer[count]-'0');
						}
						count++;
					}while(buffer[count]!=0x20);
					count++;
					do
					{
						if(rows==0)
						{
							rows=rows+(buffer[count]-'0');
						}
						else
						{
							rows=(rows*10)+(buffer[count]-'0');
						}
						count++;
					}while(buffer[count]!=0x0A);
					count++;
					total=rows*columns;
					while(buffer[count]!=0x0A)
					{
						count++;
					}
					count++;
				}
				c=count;
			}
			uint8_t green,red,blue;
			do
			{
				switch(num2)
				{
					case 0:
					{//red
						pixel_content[num+1]=map(buffer[c],0,255,0,BrightLimit);
						c++;
						num2++;
						break;
					}
					case 1:
					{//green
						pixel_content[num]=map(buffer[c],0,255,0,BrightLimit);
						c++;
						num2++;
						break;
					}
					case 2:
					{//blue
						pixel_content[num+2]=map(buffer[c],0,255,0,BrightLimit);
						num=num+3;
						column++;
						c++;
						num2=0;
						break;
					}
				}
				if(column==columns)
				{
					if(row==8 /*&& c==400*/)
					{
						int g=0;
					}
					num=0;
					to_leds(pixel_content);
					SysTick_Wait(get_delay()*16000);
					GPIO_PORTB_DATA_R &= ~0x80;
					SysTick_Wait(802);
					row++;
					column=0;
				}
			}while(row<rows && c<512);
			sectors_to_be_read--;
			if(row==rows)
			{
				finish=1;
				row=0;
				column=0;
			}
		}while(sectors_to_be_read>0 && finish!=1);
	}
	send_command(CMD12,0,SSI_number);
	sectors_to_be_read=(next_cluster*4)/512;
	if(send_command(CMD18,fat_begin_lba,SSI_number)==0)
	{
		do
		{
			sector++;
			rcvr_datablock(buffer, 512);
		}while(sector<=sectors_to_be_read/*sectors_to_be_read>0*/);
		sector--;
	}
	send_command(CMD12,0,SSI_number);
	next_cluster=(((long)(buffer[((next_cluster*4)-(sector*512))+3]))<<24)+(((long)(buffer[((next_cluster*4)-(sector*512))+2]))<<16)+(((long)(buffer[((next_cluster*4)-(sector*512))+1]))<<8)+(long)(buffer[((next_cluster*4)-(sector*512))]);
	if(next_cluster==0x0FFFFFFF || next_cluster==0x0FFFFFFF)
	{
		first_file=1;
		l=0;
		finish=0;

	}
	return next_cluster;
}

/*
 * Removes null or other non-printable characters from the file or directory name string
 * Also 'translate' accented characters so they can be printed
 */
void clean_name()
{
    char temp_name[255] = "";
    char j=0, k=0;
    //Remove all non-rintable characters
    for(j=0;j<255;j++)
    {
        if(file_dir[fd_count].name.file_dir_name[j]>=0x20 && file_dir[fd_count].name.file_dir_name[j]<=0xFC)
        {
            temp_name[k] = file_dir[fd_count].name.file_dir_name[j];
            k++;
        }
    }
    for(j=0;j<255;j++)
    {
        file_dir[fd_count].name.file_dir_name[j] = temp_name[j];
    }


    //Uncomment the following line if using my Nokia5110 library
    //Translate Unicode extended characters like accented characters
    char original_char[] = {'\xe1','\xe9', '\xed', '\xf3', '\xfa', '\xfc', '\xf1', '\xd1'};
    char translated_char[] = {'\xa1', '\xa9', '\xad', '\xb3', '\xba', '\xbc', '\xb1', '\x91'};
    for(j=0;j<255;j++)
    {
        uint8_t found = 0;
        k=0;
        while(found == 0 && k < 8)
        {
            if(file_dir[fd_count].name.file_dir_name[j] == original_char[k])
            {
                found = 1;
                file_dir[fd_count].name.file_dir_name[j] = translated_char[k];
            }
            k++;
        }
    }
}

void get_name(char pos,unsigned char name[])
{
	char i;
	for(i=0;i<25;i++)
	{
		name[i]=file_dir[pos].name.file_dir_name[i];
	}
}

char is_file_or_dir(char pos)
{
	return file_dir[pos].type;
}

/*
 * Initialises Timer 5
 */
void Timer5_Init(void)
{

	volatile unsigned short delay;
	SYSCTL_RCGCTIMER_R |= 0x20;
	delay = SYSCTL_SCGCTIMER_R;
	delay = SYSCTL_SCGCTIMER_R;
	TIMER5_CTL_R = 0x00000000;       // 1) disable timer5A during setup
	TIMER5_CFG_R = 0x00000000;       // 2) configure for 32-bit mode
	TIMER5_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
	TIMER5_TAILR_R = 159999;         // 4) reload value, 10 ms, 16 MHz clock
	TIMER5_TAPR_R = 0;               // 5) bus clock resolution
	TIMER5_ICR_R |= 0x00000001;       // 6) clear timer5A timeout flag
	TIMER5_IMR_R |= 0x00000001;       // 7) arm timeout interrupt
	NVIC_PRI23_R = (NVIC_PRI23_R&0xFFFFFF00)|0x00000040; // 8) priority 2
	// interrupts enabled in the main program after all devices initialized
	// vector number 108, interrupt number 92
	NVIC_EN2_R = 0x10000000;         // 9) enable interrupt 92 in NVIC
	TIMER5_CTL_R = 0x00000001;       // 10) enable timer5A
}

// Executed every 10 ms
void Timer5A_Handler(void){
  TIMER5_ICR_R = 0x00000001;       // acknowledge timer5A timeout
  disk_timerproc();
}

void disk_timerproc(void){   /* Decrements timer */
  if(Timer1) Timer1--;
  if(Timer2) Timer2--;
}

/*
 * Receives a block from a read of an SD card
 */
unsigned int rcvr_datablock (
    unsigned char *buff,         /* Data buffer to store received data */
    unsigned int btr/*, enum SSI SSI_number*/){          /* Byte count (must be even number) */
	unsigned char token;
  Timer1 = 10;
  do {                            /* Wait for data packet in timeout of 100ms */
    token = sd_read();
  } while ((token == 0xFF) && Timer1);
  if(token != 0xFE) return 0;    /* If not valid data token, retutn with error */

  do {                            /* Receive the data block into buffer */
    rcvr_spi_m(buff++);
    rcvr_spi_m(buff++);
  } while (btr -= 2);
  sd_write(0xFF);                        /* Discard CRC */
  sd_write(0xFF);

  return 1;                    /* Return with success */
}

void rcvr_spi_m(unsigned char *dst){
  *dst = sd_read();
}

unsigned long get_reserved_sectors(void)
{
	return reserved_sectors;
}

void filter_by_extension(unsigned char extension[],char remove_dirs)
{
	char i,j;
	for(i=0;i<40;i++)
	{
		if(file_dir[i].name.file_dir_name[0]!=0x00 && file_dir[i].type==IS_FILE)
		{
			j=0;
			while(j<256 && file_dir[i].name.file_dir_name[j]!=0x00)
			{
				j++;
			}
			if(j==255 || file_dir[i].name.file_dir_name[j-3]!=extension[0] || file_dir[i].name.file_dir_name[j-2]!=extension[1] || file_dir[i].name.file_dir_name[j-1]!=extension[2])
			{
				char k;
				for(k=0;k<255;k++)
				{
					file_dir[i].name.file_dir_name[k]=0x00;
				}
			}
		}
		else
		{
			if(file_dir[i].type==IS_DIR && remove_dirs)
			{
				for(j=0;j<255;j++)
				{
					file_dir[i].name.file_dir_name[j]=0x00;
				}
				file_dir[i].type=IS_NONE;
			}
		}
	}
	for(i=0;i<40;i++)
	{
		if(file_dir[i].name.file_dir_name[0]==0x00)
		{
			char s=i+1;
			while(s<39 && file_dir[s].name.file_dir_name[0]==0x00)
			{
				s++;
			}
			if(s<39)
			{
				for(j=0;j<255;j++)
				{
					file_dir[i].name.file_dir_name[j]=file_dir[s].name.file_dir_name[j];
					file_dir[s].name.file_dir_name[j]=0x00;
				}
				file_dir[i].type=file_dir[s].type;
				file_dir[i].name.info.day=file_dir[s].name.info.day;
				file_dir[i].name.info.first_cluster=file_dir[s].name.info.first_cluster;
				file_dir[i].name.info.hour=file_dir[s].name.info.hour;
				file_dir[i].name.info.minute=file_dir[s].name.info.minute;
				file_dir[i].name.info.month=file_dir[s].name.info.month;
				file_dir[i].name.info.size=file_dir[s].name.info.size;
				file_dir[i].name.info.year=file_dir[s].name.info.year;
				file_dir[s].type=IS_NONE;
			}
		}
	}
	char k=0;
	fd_count=0;
	while(k<40 && file_dir[k].type!=IS_NONE)
	{
		k++;
		fd_count++;
	}
}

void to_leds(unsigned char pixel_content[NUM_LEDS*3])
{
	int i;
	char k;
	//Send reset condition
	GPIO_PORTB_DATA_R |= 0x80;
	GPIO_PORTB_DATA_R &= ~0x80;
	SysTick_Wait(805);
	for(i=0;i<NUM_LEDS*3;i++)
	{
		for(k=0;k<8;k++)
		{
			if(pixel_content[i] & (0x80 >> k))
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
