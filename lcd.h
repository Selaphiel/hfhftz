#ifndef LCD_H	//TOBBSZORI INCLUDE KIZARVA!
#define LCD_H
#include<avr/io.h>


#define LCD_D_PORT	PORTE			//Felső 4 láb
#define LCD_D_DDR	DDRE
#define LCD_D_MASK	0xF0			//Felső 4..
//Control pins:
#define LCD_C_PORT	PORTF
#define LCD_C_DDR	DDRF
#define RS			PF1
#define RW			PF2
#define EN			PF3
#define LCD_DELAY	2				//msec.
//LCD CONST.:
#define BITMODE4		0x20
#define CURSOFF			0x28
#define LCDCLEAR		0x01
#define LCDON			0x0F			//F -> display + cursor +blink enable
//=========================MACROS
#define RS_LO()		LCD_C_PORT &=~(1<<RS)
#define RS_HI()		LCD_C_PORT |= (1<<RS)

#define RW_LO()		LCD_C_PORT &=~(1<<RW)
#define RW_HI()		LCD_C_PORT |= (1<<RW)

#define EN_LO()		LCD_C_PORT &=~(1<<EN)
#define EN_HI()		LCD_C_PORT |= (1<<EN)
//=========================GLOB.VAR.

//=========================PREDEC
void Init(void);
void LCD_Init(void);
void LCD_Clock(void);
void LCD_SendData(unsigned char data);
void LCD_SendCommand(unsigned char command);
void LCD_SendStr(char *str);

#endif


Dear Dr. Mosavi

I have a question about the language of the thesis. Do you think I can submit the thesis in english? 
The institution requires the thesis to be written in hungarian.
Have you had a problem with this before with another student?

Best Regards,
Csaba

  
  
  
  
  
  
  
  
  
  
