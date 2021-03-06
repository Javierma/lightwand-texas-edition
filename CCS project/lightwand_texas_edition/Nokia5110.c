/*
 * Nokia5110.c
 *
 *  Created on: 2/10/2015
 *  Author: Javier Martínez Arrieta
 *  Version: 2.0
 *  This is part of the Nokia5110 library, with functions that will allow you to work with the nokia5110 screen, known as well as PCD8544.
 *  You can find a copy of the Datasheet in the parent folder of the project.
 */

 /*  Copyright (C) 2015 Javier Martínez Arrieta
 *
 *  This project is licensed under Creative Commons Attribution-Non Commercial-Share Alike 4.0 International (CC BY-NC-SA 4.0). According to this license you are free to:
 *  Share & copy and redistribute the material in any medium or format.
 *  Adapt & remix, transform, and build upon the material.
 *  The licensor cannot revoke these freedoms as long as you follow the license terms.
 *	Complete information about this license can be found at: https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
 *
 */

#include "Nokia5110.h"
#include "/Users/javiermartinez/TivaWare/inc/tm4c123gh6pm.h"

unsigned char main_menu_items[12][25];
unsigned char menu_items[12][25];
unsigned char menu_items2[12][25];

char string[125];
char first=0,cp=0,item_num=0;
int min=0,max=5;
/**
 * Clears the screen. Please note that if inverse mode is set it will not work as expected.
 */
void clear_screen()
{
	int i;
	for(i=0; i<(MAX_X*MAX_Y); i=i+1)
	{
		lcd_write(DATA, 0x000/**/);
	}

}

/**
 * Sends command or data to the screen
 */
void lcd_write(enum typeOfWrite type,char message)
{
	if(type == COMMAND)
	{
		// wait until SSI0 not busy/transmit FIFO empty
		while((SSI0_SR_R&SSI_SR_BSY)==SSI_SR_BSY){};
		DC = DC_COMMAND;
		SSI0_DR_R = message;
		// wait until SSI0 not busy/transmit FIFO empty
		while((SSI0_SR_R&SSI_SR_BSY)==SSI_SR_BSY){};
	}
	else
	{
		// wait until transmit FIFO not full
		while((SSI0_SR_R&SSI_SR_TNF)==0){};
		DC = DC_DATA;
		SSI0_DR_R = message;
	}
}

/**
 * Writes a character on the screen. Available for English and Spanish.
 */
void char_write(char character)
{
    if(character>='A'&&character<='Z')
    {
        switch(character)
        {
            case 'A':
            {
                //A
                lcd_write(DATA,0xF8);
                lcd_write(DATA,0x24);
                lcd_write(DATA,0x22);
                lcd_write(DATA,0x24);
                lcd_write(DATA,0xF8);
                lcd_write(DATA,0x00);
                break;
            }
            case 'B':
            {
                //B
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x92);
                lcd_write(DATA,0x92);
                lcd_write(DATA,0x92);
                lcd_write(DATA,0x6C);
                lcd_write(DATA,0x00);
                break;
            }
            case 'C':
            {
                //C
                lcd_write(DATA,0x7C);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x44);
                lcd_write(DATA,0x00);
                break;
            }
            case 'D':
            {
                //D
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x44);
                lcd_write(DATA,0x38);
                lcd_write(DATA,0x00);
                break;
            }
            case 'E':
            {
                //E
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x92);
                lcd_write(DATA,0x92);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x00);
                break;
            }
            case 'F':
            {
                //F
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x12);
                lcd_write(DATA,0x12);
                lcd_write(DATA,0x02);
                lcd_write(DATA,0x02);
                lcd_write(DATA,0x00);
                break;
            }
            case 'G':
            {
                //G
                lcd_write(DATA,0x7C);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x92);
                lcd_write(DATA,0x92);
                lcd_write(DATA,0x66);
                lcd_write(DATA,0x00);
                break;
            }
            case 'H':
            {
                //H
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x10);
                lcd_write(DATA,0x10);
                lcd_write(DATA,0x10);
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x00);
                break;
            }
            case 'I':
            {
                //I
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x00);
                break;
            }
            case 'J':
            {
                //J
                lcd_write(DATA,0x70);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x7E);
                lcd_write(DATA,0x00);
                break;
            }
            case 'K':
            {
                //K
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x10);
                lcd_write(DATA,0x28);
                lcd_write(DATA,0x44);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x00);
                break;
            }
            case 'L':
            {
                //L
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x00);
                break;
            }
            case 'M':
            {
                //M
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x04);
                lcd_write(DATA,0x08);
                lcd_write(DATA,0x04);
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x00);
                break;
            }
            case 'N':
            {
                //N
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x08);
                lcd_write(DATA,0x10);
                lcd_write(DATA,0x20);
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x00);
                break;
            }

            case 'O':
            {
                //O
                lcd_write(DATA,0x7C);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0x7C);
                lcd_write(DATA,0x00);
                break;
            }
            case 'P':
            {
                //P
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x12);
                lcd_write(DATA,0x12);
                lcd_write(DATA,0x12);
                lcd_write(DATA,0x0C);
                lcd_write(DATA,0x00);
                break;
            }
            case 'Q':
            {
                //Q
                lcd_write(DATA,0x7C);
                lcd_write(DATA,0x82);
                lcd_write(DATA,0xA2);
                lcd_write(DATA,0xC2);
                lcd_write(DATA,0xFC);
                lcd_write(DATA,0x00);
                break;
            }
            case 'R':
            {
                //R
                lcd_write(DATA,0xFC);
                lcd_write(DATA,0x32);
                lcd_write(DATA,0x32);
                lcd_write(DATA,0x52);
                lcd_write(DATA,0x8C);
                lcd_write(DATA,0x00);
                break;
            }
            case 'S':
            {
                //S
                lcd_write(DATA,0x8C);
                lcd_write(DATA,0x92);
                lcd_write(DATA,0x92);
                lcd_write(DATA,0x92);
                lcd_write(DATA,0x62);
                lcd_write(DATA,0x00);
                break;
            }
            case 'T':
            {
                //T
                lcd_write(DATA,0x02);
                lcd_write(DATA,0x02);
                lcd_write(DATA,0xFE);
                lcd_write(DATA,0x02);
                lcd_write(DATA,0x02);
                lcd_write(DATA,0x00);
                break;
            }
            case 'U':
            {
                //U
                lcd_write(DATA,0x7E);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x7E);
                lcd_write(DATA,0x00);
                break;
            }
            case 'V':
            {
                //V
                lcd_write(DATA,0x3E);
                lcd_write(DATA,0x40);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x40);
                lcd_write(DATA,0x3E);
                lcd_write(DATA,0x00);
                break;
            }
            case 'W':
            {
                //W
                lcd_write(DATA,0x7E);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x40);
                lcd_write(DATA,0x80);
                lcd_write(DATA,0x7E);
                lcd_write(DATA,0x00);
                break;
            }
            case 'X':
            {
                //X
                lcd_write(DATA,0xC6);
                lcd_write(DATA,0x28);
                lcd_write(DATA,0x10);
                lcd_write(DATA,0x28);
                lcd_write(DATA,0xC6);
                lcd_write(DATA,0x00);
                break;
            }
            case 'Y':
            {
                //Y
                lcd_write(DATA,0x06);
                lcd_write(DATA,0x08);
                lcd_write(DATA,0xF0);
                lcd_write(DATA,0x08);
                lcd_write(DATA,0x06);
                lcd_write(DATA,0x00);
                break;
            }
            case 'Z':
            {
                //Z
                lcd_write(DATA,0xD2);
                lcd_write(DATA,0xB2);
                lcd_write(DATA,0x92);
                lcd_write(DATA,0x9A);
                lcd_write(DATA,0x96);
                lcd_write(DATA,0x00);
                break;
            }
        }
    }
    else
    {
        if(character>='a'&&character<='z')
        {
            switch(character)
            {
                case 'a':
                {
                    //a
                    lcd_write(DATA,0x40);
                    lcd_write(DATA,0xA8);
                    lcd_write(DATA,0xA8);
                    lcd_write(DATA,0xA8);
                    lcd_write(DATA,0x70);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'b':
                {
                    //b
                    lcd_write(DATA,0xFE);
                    lcd_write(DATA,0x90);
                    lcd_write(DATA,0x90);
                    lcd_write(DATA,0x90);
                    lcd_write(DATA,0x60);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'c':
                {
                    //c
                    lcd_write(DATA,0x70);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x50);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'd':
                {
                    //d
                    lcd_write(DATA,0x60);
                    lcd_write(DATA,0x90);
                    lcd_write(DATA,0x90);
                    lcd_write(DATA,0x90);
                    lcd_write(DATA,0xFE);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'e':
                {
                    //e
                    lcd_write(DATA,0x70);
                    lcd_write(DATA,0xA8);
                    lcd_write(DATA,0xA8);
                    lcd_write(DATA,0xA8);
                    lcd_write(DATA,0x90);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'f':
                {
                    //f
                    lcd_write(DATA,0x20);
                    lcd_write(DATA,0xF8);
                    lcd_write(DATA,0x24);
                    lcd_write(DATA,0x04);
                    lcd_write(DATA,0x04);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'g':
                {
                    //g
                    lcd_write(DATA,0x08);
                    lcd_write(DATA,0x94);
                    lcd_write(DATA,0x94);
                    lcd_write(DATA,0x94);
                    lcd_write(DATA,0x7C);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'h':
                {
                    //h
                    lcd_write(DATA,0xFE);
                    lcd_write(DATA,0x20);
                    lcd_write(DATA,0x10);
                    lcd_write(DATA,0x10);
                    lcd_write(DATA,0xE0);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'i':
                {
                    //i
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0xFA);
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'j':
                {
                    //j
                    lcd_write(DATA,0x40);
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x7A);
                    lcd_write(DATA,0x00);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'k':
                {
                    //k
                    lcd_write(DATA,0xFE);
                    lcd_write(DATA,0x10);
                    lcd_write(DATA,0x28);
                    lcd_write(DATA,0x44);
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'l':
                {
                    //l
                    lcd_write(DATA,0x00);
                    lcd_write(DATA,0x82);
                    lcd_write(DATA,0xFE);
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0x00);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'm':
                {
                    //m
                    lcd_write(DATA,0xF0);
                    lcd_write(DATA,0x08);
                    lcd_write(DATA,0xF0);
                    lcd_write(DATA,0x08);
                    lcd_write(DATA,0xF0);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'n':
                {
                    //n
                    lcd_write(DATA,0xF8);
                    lcd_write(DATA,0x10);
                    lcd_write(DATA,0x08);
                    lcd_write(DATA,0x08);
                    lcd_write(DATA,0xF0);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'o':
                {
                    //o
                    lcd_write(DATA,0x70);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x70);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'p':
                {
                    //p
                    lcd_write(DATA,0xF8);
                    lcd_write(DATA,0x48);
                    lcd_write(DATA,0x48);
                    lcd_write(DATA,0x48);
                    lcd_write(DATA,0x30);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'q':
                {
                    //q
                    lcd_write(DATA,0x30);
                    lcd_write(DATA,0x48);
                    lcd_write(DATA,0x48);
                    lcd_write(DATA,0x48);
                    lcd_write(DATA,0xF8);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'r':
                {
                    //r
                    lcd_write(DATA,0xF8);
                    lcd_write(DATA,0x10);
                    lcd_write(DATA,0x08);
                    lcd_write(DATA,0x08);
                    lcd_write(DATA,0x10);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 's':
                {
                    //s
                    lcd_write(DATA,0x90);
                    lcd_write(DATA,0xA8);
                    lcd_write(DATA,0xA8);
                    lcd_write(DATA,0xA8);
                    lcd_write(DATA,0x48);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 't':
                {
                    //t
                    lcd_write(DATA,0x08);
                    lcd_write(DATA,0x7C);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x00);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'u':
                {
                    //u
                    lcd_write(DATA,0x78);
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0xF8);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'v':
                {
                    //v
                    lcd_write(DATA,0x38);
                    lcd_write(DATA,0x40);
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0x40);
                    lcd_write(DATA,0x38);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'w':
                {
                    //w
                    lcd_write(DATA,0x78);
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0x40);
                    lcd_write(DATA,0x80);
                    lcd_write(DATA,0x78);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'x':
                {
                    //x
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x50);
                    lcd_write(DATA,0x20);
                    lcd_write(DATA,0x50);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'y':
                {
                    //y
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x50);
                    lcd_write(DATA,0x20);
                    lcd_write(DATA,0x10);
                    lcd_write(DATA,0x08);
                    lcd_write(DATA,0x00);
                    break;
                }
                case 'z':
                {
                    //z
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0xC8);
                    lcd_write(DATA,0xA8);
                    lcd_write(DATA,0x98);
                    lcd_write(DATA,0x88);
                    lcd_write(DATA,0x00);
                    break;
                }
            }
        }
        else
        {
            if((character>='á'&&character<='ú') || character=='\x91' || character=='\xa1' || character=='\xa9' || character=='\xad' || character=='\xb3' || character=='\xba' || character=='\xb1')
            {
                switch(character)
                {
                    case 'á':
                    case '\xa1':
                    {
                        //á
                        lcd_write(DATA,0x40);
                        lcd_write(DATA,0xA8);
                        lcd_write(DATA,0xAA);
                        lcd_write(DATA,0xA9);
                        lcd_write(DATA,0x70);
                        lcd_write(DATA,0x00);
                        break;
                    }
                    case 'é':
                    case '\xa9':
                    {
                        //é
                        lcd_write(DATA,0x70);
                        lcd_write(DATA,0xA8);
                        lcd_write(DATA,0xAA);
                        lcd_write(DATA,0xA9);
                        lcd_write(DATA,0x90);
                        lcd_write(DATA,0x00);
                        break;
                    }
                    case 'í':
                    case '\xad':
                    {
                        //í
                        lcd_write(DATA,0x80);
                        lcd_write(DATA,0x90);
                        lcd_write(DATA,0xF4);
                        lcd_write(DATA,0x82);
                        lcd_write(DATA,0x80);
                        lcd_write(DATA,0x00);
                        break;
                    }
                    case 'ó':
                    case '\xb3':
                    {
                        //ó
                        lcd_write(DATA,0x70);
                        lcd_write(DATA,0x88);
                        lcd_write(DATA,0x8A);
                        lcd_write(DATA,0x89);
                        lcd_write(DATA,0x70);
                        lcd_write(DATA,0x00);
                        break;
                    }
                    case 'ú':
                    case '\xba':
                    {
                        //ú
                        lcd_write(DATA,0x78);
                        lcd_write(DATA,0x80);
                        lcd_write(DATA,0x82);
                        lcd_write(DATA,0x81);
                        lcd_write(DATA,0xF8);
                        lcd_write(DATA,0x00);
                        break;
                    }
                    case 'ñ':
                    case '\xb1':
                    {
                        //ñ
                        lcd_write(DATA,0xF4);
                        lcd_write(DATA,0x22);
                        lcd_write(DATA,0x16);
                        lcd_write(DATA,0x14);
                        lcd_write(DATA,0xE2);
                        lcd_write(DATA,0x00);
                        break;
                    }
                    case 'Ñ':
                    case '\x91':
                    {
                        //Ñ
                        lcd_write(DATA,0xFA);
                        lcd_write(DATA,0x09);
                        lcd_write(DATA,0x13);
                        lcd_write(DATA,0x12);
                        lcd_write(DATA,0x22);
                        lcd_write(DATA,0xF9);
                        lcd_write(DATA,0x00);
                        break;
                    }
                }
            }
            else
            {
                if(character>='0'&&character<='9')
                {
                    switch(character)
                    {
                        case '0':
                        {
                            //O
                            lcd_write(DATA,0x7C);
                            lcd_write(DATA,0x82);
                            lcd_write(DATA,0x82);
                            lcd_write(DATA,0x82);
                            lcd_write(DATA,0x7C);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '1':
                        {
                            //1
                            lcd_write(DATA,0x88);
                            lcd_write(DATA,0x84);
                            lcd_write(DATA,0xFE);
                            lcd_write(DATA,0x80);
                            lcd_write(DATA,0x80);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '2':
                        {
                            //2
                            lcd_write(DATA,0x84);
                            lcd_write(DATA,0xC2);
                            lcd_write(DATA,0xA2);
                            lcd_write(DATA,0x92);
                            lcd_write(DATA,0x8C);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '3':
                        {
                            //3
                            lcd_write(DATA,0x44);
                            lcd_write(DATA,0x82);
                            lcd_write(DATA,0x92);
                            lcd_write(DATA,0x92);
                            lcd_write(DATA,0x6C);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '4':
                        {
                            //4
                            lcd_write(DATA,0x20);
                            lcd_write(DATA,0x30);
                            lcd_write(DATA,0x28);
                            lcd_write(DATA,0x24);
                            lcd_write(DATA,0xFE);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '5':
                        {
                            //5
                            lcd_write(DATA,0x8E);
                            lcd_write(DATA,0x8A);
                            lcd_write(DATA,0x8A);
                            lcd_write(DATA,0x8A);
                            lcd_write(DATA,0x72);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '6':
                        {
                            //6
                            lcd_write(DATA,0x7C);
                            lcd_write(DATA,0xA4);
                            lcd_write(DATA,0x92);
                            lcd_write(DATA,0x92);
                            lcd_write(DATA,0x64);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '7':
                        {
                            //7
                            lcd_write(DATA,0x82);
                            lcd_write(DATA,0x42);
                            lcd_write(DATA,0x22);
                            lcd_write(DATA,0x12);
                            lcd_write(DATA,0x0E);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '8':
                        {
                            //8
                            lcd_write(DATA,0x44);
                            lcd_write(DATA,0xAA);
                            lcd_write(DATA,0x92);
                            lcd_write(DATA,0xAA);
                            lcd_write(DATA,0x44);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '9':
                        {
                            //9
                            lcd_write(DATA,0x0C);
                            lcd_write(DATA,0x12);
                            lcd_write(DATA,0x12);
                            lcd_write(DATA,0x0A);
                            lcd_write(DATA,0xFC);
                            lcd_write(DATA,0x00);
                            break;
                        }
                    }
                }
                else
                {
                    //other characters
                    switch(character)
                    {
                        case 'ü':
                        case '\xbc':
                        {
                            //ü
                            lcd_write(DATA,0x78);
                            lcd_write(DATA,0x82);
                            lcd_write(DATA,0x80);
                            lcd_write(DATA,0x82);
                            lcd_write(DATA,0xF8);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '¡':
                        case '\xa1':
                        {
                            //¡
                            lcd_write(DATA,0xFA);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '!':
                        {
                            //!
                            lcd_write(DATA,0xBE);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '?':
                        {
                            //?
                            lcd_write(DATA,0x04);
                            lcd_write(DATA,0x02);
                            lcd_write(DATA,0xA2);
                            lcd_write(DATA,0x12);
                            lcd_write(DATA,0x0C);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '¿':
                        case '\xbf':
                        {
                            //¿
                            lcd_write(DATA,0x60);
                            lcd_write(DATA,0x90);
                            lcd_write(DATA,0x8A);
                            lcd_write(DATA,0x80);
                            lcd_write(DATA,0x40);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '|':
                        {
                            //|
                            lcd_write(DATA,0xFE);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '"':
                        {
                            //"
                            lcd_write(DATA,0x06);
                            lcd_write(DATA,0x00);
                            lcd_write(DATA,0x06);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '·':
                        {
                            //·
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '#':
                        {
                            //#
                            lcd_write(DATA,0x28);
                            lcd_write(DATA,0x7C);
                            lcd_write(DATA,0x28);
                            lcd_write(DATA,0x7C);
                            lcd_write(DATA,0x28);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '$':
                        {
                            //$
                            lcd_write(DATA,0x48);
                            lcd_write(DATA,0x54);
                            lcd_write(DATA,0xFE);
                            lcd_write(DATA,0x54);
                            lcd_write(DATA,0x24);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '~':
                        {
                            //~
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x08);
                            lcd_write(DATA,0x18);
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x08);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '%':
                        {
                            //%
                            lcd_write(DATA,0x04);
                            lcd_write(DATA,0x4A);
                            lcd_write(DATA,0x24);
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x08);
                            lcd_write(DATA,0x24);
                            lcd_write(DATA,0x52);
                            lcd_write(DATA,0x20);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '€':
                        case '\x82':
                        {
                            //€
                            lcd_write(DATA,0x18);
                            lcd_write(DATA,0x7E);
                            lcd_write(DATA,0x99);
                            lcd_write(DATA,0x81);
                            lcd_write(DATA,0x42);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '&':
                        {
                            //&
                            lcd_write(DATA,0x44);
                            lcd_write(DATA,0xAA);
                            lcd_write(DATA,0x92);
                            lcd_write(DATA,0xAA);
                            lcd_write(DATA,0x44);
                            lcd_write(DATA,0xA0);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '¬':
                        {
                            //¬
                            lcd_write(DATA,0x08);
                            lcd_write(DATA,0x08);
                            lcd_write(DATA,0x18);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '/':
                        {
                            // /
                            lcd_write(DATA,0x40);
                            lcd_write(DATA,0x20);
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x08);
                            lcd_write(DATA,0x04);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '(':
                        {
                            //(
                            lcd_write(DATA,0x3C);
                            lcd_write(DATA,0x42);
                            lcd_write(DATA,0x81);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case ')':
                        {
                            //)
                            lcd_write(DATA,0x81);
                            lcd_write(DATA,0x42);
                            lcd_write(DATA,0x3C);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '=':
                        {
                            //=
                            lcd_write(DATA,0x28);
                            lcd_write(DATA,0x28);
                            lcd_write(DATA,0x28);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '\'':
                        {
                            //'
                            lcd_write(DATA,0x06);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '-':
                        {
                            //-
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '+':
                        {
                            //+
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x38);
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '*':
                        {
                            //*
                            lcd_write(DATA,0x54);
                            lcd_write(DATA,0x38);
                            lcd_write(DATA,0x7C);
                            lcd_write(DATA,0x38);
                            lcd_write(DATA,0x54);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '@':
                        {
                            //@
                            lcd_write(DATA,0x38);
                            lcd_write(DATA,0x44);
                            lcd_write(DATA,0xBA);
                            lcd_write(DATA,0xAA);
                            lcd_write(DATA,0xBA);
                            lcd_write(DATA,0xA4);
                            lcd_write(DATA,0x18);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '>':
                        {
                            //>
                            lcd_write(DATA,0x44);
                            lcd_write(DATA,0x28);
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '<':
                        {
                            //<
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x28);
                            lcd_write(DATA,0x44);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '[':
                        {
                            //[
                            lcd_write(DATA,0xFF);
                            lcd_write(DATA,0x81);
                            lcd_write(DATA,0x81);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case ']':
                        {
                            //]
                            lcd_write(DATA,0x81);
                            lcd_write(DATA,0x81);
                            lcd_write(DATA,0xFF);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '{':
                        {
                            //{
                            lcd_write(DATA,0x18);
                            lcd_write(DATA,0x66);
                            lcd_write(DATA,0x81);
                            lcd_write(DATA,0x81);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '}':
                        {
                            //}
                            lcd_write(DATA,0x81);
                            lcd_write(DATA,0x81);
                            lcd_write(DATA,0x66);
                            lcd_write(DATA,0x18);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case ',':
                        {
                            //,
                            lcd_write(DATA,0x80);
                            lcd_write(DATA,0x40);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case ';':
                        {
                            //;
                            lcd_write(DATA,0x80);
                            lcd_write(DATA,0x50);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case ':':
                        {
                            //:
                            lcd_write(DATA,0x28);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '.':
                        {
                            //.
                            lcd_write(DATA,0x80);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '_':
                        {
                            //_
                            lcd_write(DATA,0x80);
                            lcd_write(DATA,0x80);
                            lcd_write(DATA,0x80);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case 'º':
                        {
                            lcd_write(DATA,0x14);
                            lcd_write(DATA,0x1A);
                            lcd_write(DATA,0x14);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case 'ª':
                        {
                            lcd_write(DATA,0x52);
                            lcd_write(DATA,0x6A);
                            lcd_write(DATA,0x6A);
                            lcd_write(DATA,0x7C);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case '\\':
                        {
                            //\
                            lcd_write(DATA,0x04);
                            lcd_write(DATA,0x08);
                            lcd_write(DATA,0x10);
                            lcd_write(DATA,0x20);
                            lcd_write(DATA,0x40);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case 0x20:
                        {
                            //' '
                            lcd_write(DATA,0x00);
                            lcd_write(DATA,0x00);
                            break;
                        }
                        case 0x7F:
                        {
                            //DEL
                            lcd_write(DATA,0x00);
                            break;
                        }
                    }
                }
            }
        }
    }
}


/*
 * Initializes screen, setting temperature coefficient, contrast and other parameters
 */
void initialize_screen(short backlight)
{
	lcd_write(COMMAND, 0x21);              	// chip active; horizontal addressing mode (V = 0); use extended instruction set (H = 1)
											// set LCD Vop (contrast), which may require some tweaking:
	lcd_write(COMMAND, CONTRAST);          	// 0xB1 for 3.3V red SparkFun, 0xB8 for 3.3V blue SparkFun, 0xBF if your display is too dark or 0x80 to 0xFF if experimenting
	lcd_write(COMMAND, 0x04);              	// set temp coefficient
	lcd_write(COMMAND, 0x14);              	// LCD bias mode 1:48: try 0x13 or 0x14
	lcd_write(COMMAND, 0x20);              	// we must send 0x20 before modifying the display control mode
	lcd_write(COMMAND, 0x0C);              	// Display control (0x0C for normal mode, 0x0D for inverse mode)

	switch(backlight)
	{
		case BACKLIGHT_ON:
		{
			GPIO_PORTA_DATA_R&=~0x10;				//Screen's light on
			break;
		}
		case BACKLIGHT_OFF:
		{
			GPIO_PORTA_DATA_R|=0x10;				//Screen's light off
			break;
		}
	}
}

/**
 * Sets backlight on
 */
void enable_backlight()
{
	GPIO_PORTA_DATA_R&=~0x10;
}

/**
 * Sets backlight off
 */
void disable_backlight()
{
	GPIO_PORTA_DATA_R|=0x10;
}


/**
 * In case you are not willing to send commands to set position, for example, within the same block, you can use this as a custom space.
 */
void clear_columns(char ncolumns)
{
	char i;
	for(i=0;i<ncolumns;i++)
	{
		lcd_write(DATA,0x00);
	}
}

/**
 * Fills the whole screen, leaving all pixels black unless screen is set in inverse mode
 */
void fill_screen(short SSI)
{
	char i;
	for(i=0;i<MAX_X*MAX_Y;i++)
	{
		lcd_write(DATA,0xFF);
	}
}

/**
 * Writes text on the screen with the alignment selected
 */
void screen_write(char string_to_write[],short alignment)
{
	unsigned int word_length=0;
	int max_x_position, x_position,y_position;
	unsigned int lines=0;
	int i=0,j=0,y_pos=0;
	if((alignment!=ALIGN_LEFT_TOP)&&(alignment!=ALIGN_CENTRE_TOP)&&(alignment!=ALIGN_RIGHT_TOP))
	{
		while(string_to_write[i]!='\0')
		{
			if(string_to_write[i]=='\n')
			{
				lines++;
			}
			i++;
		}
	}
	if((alignment==ALIGN_LEFT_CENTRE)||(alignment==ALIGN_CENTRE_CENTRE)||(alignment==ALIGN_RIGHT_CENTRE))
	{
		y_pos=set_Y_position_centre(lines,SSI0);
	}
	else
	{
		if((alignment==ALIGN_LEFT_BOTTOM)||(alignment==ALIGN_CENTRE_BOTTOM)||(alignment==ALIGN_RIGHT_BOTTOM))
		{
			y_pos=set_Y_position_bottom(lines,SSI0);
		}
		else
		{
			lcd_write(COMMAND,0x40);
		}
	}
	i=0;
	//Set (0,0)
	lcd_write(COMMAND,0x80);
	while(string_to_write[i]!='\0')
	{
		while(string_to_write[i]!='\0'&&string_to_write[i]!='\n'&&word_length<84)
		{
			word_length=word_length+get_character_length(string_to_write[i]);
			i++;
		}
		switch (alignment)
		{
			case ALIGN_LEFT_TOP:
			{
				break;
			}
			case ALIGN_RIGHT_TOP:
			{
				lcd_write(COMMAND,0xD3-word_length);
				break;
			}
			case ALIGN_CENTRE_TOP:
			{
				lcd_write(COMMAND,(0xA9)-(word_length/2));
				break;
			}
			case ALIGN_LEFT_CENTRE:
			{
				lcd_write(COMMAND,0x80);
				break;
			}
			case ALIGN_RIGHT_CENTRE:
			{
				lcd_write(COMMAND,0xD3-word_length);
				break;
			}
			case ALIGN_CENTRE_CENTRE:
			{
				lcd_write(COMMAND,(0xA9)-(word_length/2));
				break;
			}
			case ALIGN_LEFT_BOTTOM:
			{
				lcd_write(COMMAND,0x80);
				break;
			}
			case ALIGN_RIGHT_BOTTOM:
			{
				lcd_write(COMMAND,0xD3-word_length);
				break;
			}
			case ALIGN_CENTRE_BOTTOM:
			{
				lcd_write(COMMAND,(0xA9)-(word_length/2));
				break;
			}
			case ALIGN_RANDOM:
			{	//This case is for one line text such as "welcome"
				srand(time(NULL));
				y_position=rand()%5;
				lcd_write(COMMAND,0x40+y_position);
				max_x_position=84-word_length;
				srand(time(NULL));
				x_position=(rand()%(max_x_position));
				lcd_write(COMMAND,0x80+x_position);
				break;
			}
		}
		while(j<i)
		{
			char_write(string_to_write[j]/*,SSI0*/);
			j++;
		}
		if(string_to_write[i]!='\0')
		{
			i++;
		}
		word_length=0;
		lcd_write(COMMAND,0x80);
		if(y_pos==5)
		{
			y_pos=0;
		}
		else
		{
			y_pos++;
		}
		lcd_write(COMMAND,0x40|y_pos);
	}
}

int set_Y_position_bottom(int lines,short SSI)
{
	int y_pos;
	switch(lines)
	{
		case 0:
		{
			lcd_write(COMMAND,0x45);
			y_pos=5;
			break;
		}
		case 1:
		{
			lcd_write(COMMAND,0x44);
			y_pos=4;
			break;
		}
		case 2:
		{
			lcd_write(COMMAND,0x43);
			y_pos=3;
			break;
		}
		case 3:
		{
			lcd_write(COMMAND,0x42);
			y_pos=2;
			break;
		}
		case 4:
		{
			lcd_write(COMMAND,0x41);
			y_pos=1;
			break;
		}
		case 5:
		{
			lcd_write(COMMAND,0x40);
			y_pos=0;
			break;
		}
	}
	return y_pos;
}

int set_Y_position_centre(int lines,short SSI)
{
	int y_pos;
	switch(lines)
	{
		case 0:
		{
			lcd_write(COMMAND,0x42);
			y_pos=2;
			break;
		}
		case 1:
		{
			lcd_write(COMMAND,0x42);
			y_pos=2;
			break;
		}
		case 2:
		{
			lcd_write(COMMAND,0x42);
			y_pos=2;
			break;
		}
		case 3:
		{
			lcd_write(COMMAND,0x41);
			y_pos=1;
			break;
		}
		case 4:
		{
			lcd_write(COMMAND,0x41);
			y_pos=1;
			break;
		}
		case 5:
		{
			lcd_write(COMMAND,0x40);
			y_pos=0;
			break;
		}
	}
	return y_pos;
}

void startSSI0()
{
	  volatile unsigned long delay;
	  SYSCTL_RCGC2_R |= 0x00000001;   		//  activate clock for Port A
	  SYSCTL_RCGCSSI_R|=SYSCTL_RCGCSSI_R0;		  		//  activate clock for SSI0
	  delay = SYSCTL_RCGC2_R;         		//  allow time for clock to stabilize
	  GPIO_PORTA_DIR_R |= 0xD0;             // make PA4,6,7 out
	  GPIO_PORTA_AFSEL_R |= 0x2C;           // enable alt funct on PA2,3,5
	  GPIO_PORTA_AFSEL_R &= ~0xC0;          // disable alt funct on PA6,7
	  GPIO_PORTA_DEN_R |= 0xFC;             // enable digital I/O on PA2,3,4,5,6,7
	                                        // configure PA2,3,5 as SSI
	  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFF0F00FF)+0x00202200;
	                                        // configure PA6,7 as GPIO
	  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0x00FFFFFF)+0x00000000;
	  GPIO_PORTA_AMSEL_R &= ~0xFC;          // disable analog functionality on PA2,3,4,5,6,7
	  SSI0_CR1_R&=~SSI_CR1_SSE;		  // 3)Disable SSI while configuring it
	  SSI0_CR1_R&=~SSI_CR1_MS;		  // 4) Set as Master
	  SSI0_CC_R|=SSI_CC_CS_M; // 5) Configure clock source
	  SSI0_CC_R|=SSI_CC_CS_SYSPLL; // 5) Configure clock source
	  SSI0_CC_R|=SSI_CPSR_CPSDVSR_M;// 6) Configure prescale divisor
	  SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+2; // must be even number
	  SSI0_CR0_R |=0x7000;
	  SSI0_CR0_R &= ~(SSI_CR0_SPH | SSI_CR0_SPO);
	  //SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CR0_SCR_M)+7; // must be even number
	  SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO;
	                                        // DSS = 8-bit data
	  SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_8;
	  SSI0_CR1_R|=SSI_CR1_SSE;		  // 3)Enable SSI
	  RESET = RESET_LOW;                    // reset the LCD to a known state
	  for(delay=0; delay<10; delay=delay+1);// delay minimum 100 ns
	  RESET = RESET_HIGH;                   // negative logic

}

unsigned short get_character_length(char character)
{
    short character_length;
    if(character>='A'&&character<='Z')
    {
        character_length=6;
    }
    else
    {
        if(character>='a'&&character<='z')
        {
            character_length=6;
        }
        else
        {
            if((character>='á'&&character<='ú') || character=='\x91' || character=='\xa1' || character=='\xa9' || character=='\xad' || character=='\xb3' || character=='\xba')
            {
                switch(character)
                {
                    case 'Ñ':
                    case '\x91':
                    {
                        character_length=7;
                        break;
                    }
                    default:
                    {
                        character_length=6;
                        break;
                    }
                }
            }
            else
            {
                if(character>='0'&&character<='9')
                {
                    character_length=6;
                }
                else
                {
                    //other characters
                    switch(character)
                    {
                        case 'Ñ':
                        case '\x91':
                        {
                            character_length=7;
                            break;
                        }
                        case 'ü':
                        case '\xbc':
                        {
                            character_length=6;
                            break;
                        }
                        case 0x20:
                        {
                            character_length=2;
                            break;
                        }
                        case '¡':
                        case '\xa1':
                        {
                            character_length=2;
                            break;
                        }
                        case '!':
                        {
                            character_length=2;
                            break;
                        }
                        case '?':
                        {
                            character_length=6;
                            break;
                        }
                        case '¿':
                        case '\xbf':
                        {
                            character_length=6;
                            break;
                        }
                        case '|':
                        {
                            character_length=2;
                            break;
                        }
                        case '"':
                        {
                            character_length=4;
                            break;
                        }
                        case '·':
                        {
                            character_length=2;
                            break;
                        }
                        case '#':
                        {
                            character_length=6;
                            break;
                        }
                        case '$':
                        {
                            character_length=6;
                            break;
                        }
                        case '~':
                        {
                            character_length=6;
                            break;
                        }
                        case '%':
                        {
                            character_length=9;
                            break;
                        }
                        case '€':
                        case '\x82':
                        {
                            character_length=6;
                            break;
                        }
                        case '&':
                        {
                            character_length=7;
                            break;
                        }
                        case '¬':
                        {
                            character_length=4;
                            break;
                        }
                        case '/':
                        {
                            character_length=6;
                            break;
                        }
                        case '(':
                        {
                            character_length=4;
                            break;
                        }
                        case ')':
                        {
                            character_length=4;
                            break;
                        }
                        case '=':
                        {
                            character_length=4;
                            break;
                        }
                        case '\'':
                        {
                            character_length=2;
                            break;
                        }
                        case '-':
                        {
                            character_length=4;
                            break;
                        }
                        case '+':
                        {
                            character_length=4;
                            break;
                        }
                        case '*':
                        {
                            character_length=6;
                            break;
                        }
                        case '@':
                        {
                            character_length=8;
                            break;
                        }
                        case '>':
                        {
                            character_length=4;
                            break;
                        }
                        case '<':
                        {
                            character_length=4;
                            break;
                        }
                        case '[':
                        {
                            character_length=4;
                            break;
                        }
                        case ']':
                        {
                            character_length=4;
                            break;
                        }
                        case '{':
                        {
                            character_length=5;
                            break;
                        }
                        case '}':
                        {
                            character_length=5;
                            break;
                        }
                        case ',':
                        {
                            character_length=3;
                            break;
                        }
                        case ';':
                        {
                            character_length=3;
                            break;
                        }
                        case ':':
                        {
                            character_length=2;
                            break;
                        }
                        case '.':
                        {
                            character_length=2;
                            break;
                        }
                        case '_':
                        {
                            character_length=4;
                            break;
                        }
                        case 'º':
                        {
                            character_length=4;
                            break;
                        }
                        case 'ª':
                        {
                            character_length=5;
                            break;
                        }
                        case '\\':// \ is a special character,so it must be ignored
                        {
                            character_length=5;
                            break;
                        }
                    }
                }
            }
        }
    }
    return character_length;
}

void modify_menu_element(char pos,unsigned char item[])
{
	char i=0;
	for(i=0;i<25;i++)
	{
		menu_items[pos][i]=0x00;
	}
	for(i=0;i<25;i++)
	{
		menu_items[pos][i]=item[i];
	}
	for(i=0;i<125;i++)
	{
		string[i]=0x00;
	}
}

void delete_menu(void)
{
	char i,j;
	for(i=0;i<12;i++)
	{
		for(j=0;j<25;j++)
		{
			menu_items[i][j]=0x00;
		}
	}
	item_num=0;
}

void copy_menu(void)
{
	char i,j;
	for(i=0;i<12;i++)
	{
		for(j=0;j<25;j++)
		{
			menu_items2[i][j]=menu_items[i][j];
		}
	}
}

void backup_main_menu(void)
{
	char i,j;
	for(i=0;i<12;i++)
	{
		for(j=0;j<25;j++)
		{
			main_menu_items[i][j]=menu_items[i][j];
		}
	}
}

void restore_main_menu(void)
{
	char i,j;
	for(i=0;i<12;i++)
	{
		for(j=0;j<25;j++)
		{
			menu_items[i][j]=main_menu_items[i][j];
		}
	}
}

void restore_previous_menu(void)
{
	char i,j;
	for(i=0;i<12;i++)
	{
		for(j=0;j<25;j++)
		{
			menu_items[i][j]=menu_items2[i][j];
		}
	}
}

void add_menu_element(unsigned char item[])
{
    uint8_t line_length = 0;
    char i=0;
    for(i=0;i<25;i++)
    {
        line_length = line_length + get_character_length(item[i]);
        if(line_length < 84)
        {
            menu_items[item_num][i]=item[i];
        }
    }
    item_num++;
}

/*
 * Shows the element of the menu according to the current position in list
 */
void show_menu(char current_position)
{
	lcd_write(COMMAND,0x40+(cp-min));
	lcd_write(COMMAND,0x80);
	lcd_write(DATA,0x00);
	//Uncomment next line in case you wish to make the selection bar a bit thicker
	//lcd_write(DATA,0x00);
	char i=0,j=0,current_pos=1;
	for(i=0;i<125;i++)
	{
		string[i]=0x00;
	}
	if(current_position>max)
	{
		max=current_position;
		min=max-5;
	}
	else
	{
		if(current_position<min)
		{
			min=current_position;
			max=min+5;
		}
	}
	/*if(current_position<=min || current_position>=max || first==0)
	{*/
		first=1;
		string[0]=' ';
		for(i=0;i<6;i++)
		{
			for(j=0;j<25;j++)
			{
				if(menu_items[min+i][j]!=0x00)
				{
					string[current_pos]=menu_items[min+i][j];
					current_pos++;
				}
				else
				{
					string[current_pos]='\n';
					current_pos++;
					string[current_pos]=' ';
					current_pos++;
					break;
				}
			}
		}
	//}
	screen_write(string,ALIGN_LEFT_TOP/*,SSI0*/);
	lcd_write(COMMAND,0x40+(current_position-min));
	lcd_write(COMMAND,0x80);
	lcd_write(DATA,0xFF);
	//Uncomment next line in case you wish to make the selection bar a bit thicker
	//lcd_write(DATA,0xFF);
	cp=current_position;
}

/*
 * Set buttons to go up and down in the menu
 */
void set_buttons_up_down(void)
{
	//PB0 (up) & PB1 (down)
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000002;   //  activate clock for Port B
	delay = SYSCTL_RCGC2_R;         //    allow time for clock to stabilize
	GPIO_PORTB_DIR_R &= ~0x03;             // make PB0 and PB1 in
	GPIO_PORTB_AFSEL_R &= 0x03;           // disable alt funct on PB0 and PB1
	GPIO_PORTB_DEN_R |= 0x03;             // enable digital I/O on PB0 and PB1
	GPIO_PORTB_AMSEL_R &= ~0x03;		//Disable analog in PB0 and PB1
}

void reset_min_max(void)
{
	min=0;
	max=5;
}
