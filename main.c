#include <avr/io.h>
#include <avr/interrupt.h>
//#define F_CPU 16000000UL
//#include <util/delay.h>			//nem szabad használni, törölve lesz
#include "lcd.h"
extern void init_in_asm(void);
extern void LED_out_in_asm(uint8_t);

#define FIRED 1						//olvashatóság miatt, Számláló start
#define RESET 0
#define ONAIR 2						//olvashatóság miatt, Számláló reset

									//Volatile fordító optimalizálás elkerülése miatt - változhat az értéke
volatile unsigned char timer_500ms = RESET;
volatile unsigned char timer_50ms = RESET;											          // Fired => 1
volatile unsigned char timer_5ms = RESET;
volatile unsigned char timer_6ms = RESET;           // Fired => 1
void ledout(uint8_t str_pos);

unsigned int in,i,sor,x,oszlop,gin,szam;
unsigned int str_pos=0,page=0,cap=0,cur_pos=0; //uint próba
char vaild;

void wait_6ms();
void wait_5ms();
void wait_50ms();
void wait_500ms();
void IsKeyReleased (void);
void Multitap (unsigned int szam, char kybrdChars[10][10],char *stp);
void seg(); // a kijelző alsó értékére ír
void init();
void show7seg();


void init() {
	//DDRG = 0;
    //DDRA = 0xFF;
    //DDRD = 0xF0;
    //DDRB = 0xF0;
	//DDRC=0b01111000;
    // Timer/Counter1 init, CTC mode, f = 1ms
    // f = F_CPU / (2 * presc * (1 + OCRnA))
    // presc = 1 ( 1<<CS10 )*/
    //TCCR1A = 0;
    //TCCR1B = (( 1<<WGM12 ) | ( 1<<CS10 ));
    //TCCR1C = 0;
    //OCR1A = 7999*2; 
    //TIMSK |= ( 1<<OCIE1A );
    
    //sei();
}


//-----------------------------------------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect)												//megszakító vektor beállítása
{
    static unsigned int tick_500ms = 0;		
	static unsigned int tick_50ms = 0;
    static unsigned int tick_5ms = 0;
	static unsigned int tick_6ms = 0;


	if (timer_500ms == RESET) {tick_500ms=0;timer_500ms = ONAIR;}
 
    tick_500ms++;
	tick_50ms++;
    tick_5ms++;
	tick_6ms++;

    if( tick_500ms >= 800 ) //1secre átírtam teszteléshez
    {
        tick_500ms = 0;
        timer_500ms = FIRED;
    }

	if (tick_50ms >= 50 )
	{
		tick_50ms = 0;
        timer_50ms = FIRED;

	}
    
    if( tick_5ms >= 5 )
    {
        tick_5ms = 0;
        timer_5ms = FIRED;
    }
	if( tick_6ms >= 6 )
    {
        tick_6ms = 0;
        timer_6ms = FIRED;
    }
}
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
int main(void)
{
	unsigned int cursor=0x0F;
	unsigned int blink=0x0F;
	char kybrdChars[10][10]= {
  {"\ _0"}, 		//0
  {".,;!?'\"()"}, 	//1
  {"abc"}, 			//2
  {"def"}, 			//3
  {"ghi"}, 			//4
  {"jkl"}, 			//5
  {"mno"}, 			//6
  {"pqrs"}, 		//7
  {"tuv"}, 			//8
  {"wxyz"},		 	//9
};

	

	char t1[159]="0";
	char *stp;
	stp=t1;				
	
	init_in_asm();
	sei();
	//init();
	LCD_Init();

	while (1)	
	{	

				
		x=1;
		sor=1;	
		vaild=0;
		gin=PING;
		switch (gin)
			{
		
			case 1: if(cur_pos > 0) {LCD_SendCommand(0x10);LCD_SendData(' ');LCD_SendCommand(0x10);cur_pos--;str_pos--;}wait_50ms(); break;
			case 2:	 
					 if((cur_pos==32)&&(page<4)) {page++;cur_pos=0;LCD_SendStr(t1);break;}	
					 if(str_pos > ((page*32)+cur_pos)) {LCD_SendCommand(0x14);cur_pos++;wait_50ms();break;}//kurzor jobbra mozgatása,amíg van bevitt karakter hd44780 28.old
					 
					 									
			case 4:	 if(cur_pos>0)   {LCD_SendCommand(0x10);cur_pos--;wait_50ms();break;} //balra mozgás
					 if((cur_pos==0) && (page==0)) {wait_50ms();break;}
					 if((cur_pos==0) && (page>0))	   {page--;LCD_SendStr(t1);break;}


			case 8:	 blink^=0x01;LCD_SendCommand(blink);wait_50ms();break;
			case 16: LCD_SendStr(t1);//cursor^=0x02;LCD_SendCommand(cursor);wait_50ms();break;

			
			default: break;
			}
		
		
		for(i=1;i<=4;i++)
		{	
			
			PORTC=x<<3;
			wait_5ms();
			show7seg();
			in=PINC;
			in&=0b00000111;
			
			if(in==0x06) {oszlop=1; vaild=1;break;}
			if(in==0x05) {oszlop=2; vaild=1;break;}
			if(in==0x03) {oszlop=3; vaild=1;break;}
			x=x<<1;
			sor++;	
	    }
//---------------------------------------------------------------------------------------------------------------------
		if (vaild==1)						// kiíratás
		{
			
			szam=oszlop+3*(sor-1); 
			if (szam==11) {szam=0;}		//4. sor középső gombja a 0, 0. címről kapja a karaktereket a tömbből
			if (szam==12) {switch (cap){case 0: cap=1;break;case 1:cap=0;break;}} //nagybetűre váltás
			IsKeyReleased();
			if (szam>=0 && szam<=9) //ne kerüljön bele a multitap-be a nagybetűre váltás
			{
			Multitap(szam,kybrdChars,stp);
			stp++;
			if((cur_pos>31)&&(page<5))	//következő oldalra lépés
				{
				page++;
				cur_pos=0;
				LCD_SendCommand(LCDCLEAR);	//képernyőt töröl + adress counterbe 0-át ír


				}
			}
			
			//seg();
			ledout(str_pos);
			//wait_500ms();
		

 		}
	if (timer_5ms)
		 	{
	 		timer_5ms = RESET;
	 		show7seg();
	 		}		
	
	}
	

	return 0;
}

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//Függvény deklarációk:

void ledout(uint8_t str_pos)
{
if ((str_pos>=0)&&(str_pos<=20)) {PORTB=0x10;PORTD=0x00;}
if ((str_pos>=21)&&(str_pos<=40)) {PORTB=0x30;PORTD=0x00;}
if ((str_pos>=41)&&(str_pos<=60)) {PORTB=0x70;PORTD=0x00;}
if ((str_pos>=61)&&(str_pos<=80)) {PORTB=0xF0;PORTD=0x00;}
if ((str_pos>=81)&&(str_pos<=100)) {PORTB=0xF0;PORTD=0x10;}
if ((str_pos>=101)&&(str_pos<=120)) {PORTB=0xF0;PORTD=0x30;}
if ((str_pos>=121)&&(str_pos<=140)) {PORTB=0xF0;PORTD=0x70;}
if ((str_pos>=141)&&(str_pos<=160)) {PORTB=0xF0;PORTD=0xF0;}


}

void seg()
	{
	PORTA=0b10100000;
	PORTA+=szam;
	wait_5ms();
		

	}

void show7seg()
{
    static int digit = 0;
    
    if ( digit == 0 ) PORTA = 0b10000000 | ( digit<<4 ) | (str_pos%100)%10;		//str_pos de most teszt
    if ( digit == 1 ) PORTA = 0b10000000 | ( digit<<4 ) | (str_pos%100)/10;
    if ( digit == 2 ) PORTA = 0b10000000 | ( digit<<4 ) | (str_pos%1000)/100;
    if ( digit == 3 ) PORTA = 0b10000000 | ( digit<<4 ) |  str_pos/1000;
    // digit++;
    if ( ++digit == 4 ) digit = 0;
}
void wait_500ms()
{
	while (1)
	{
		if( timer_500ms==FIRED )             // Timer expired.
        {
            timer_500ms = RESET;
            break;
       	}
	}


}


void wait_50ms()
{
	while (1)
	{
		if( timer_50ms )             // Timer expired.
        {
            timer_50ms = RESET;
            break;
       	}
	}


}

void wait_5ms()
{
	while (1)
	{
		if( timer_5ms )             // Timer expired.
        {
            timer_5ms = RESET;
            
           	break;
       	}
	}


}

void wait_6ms()
{
	while (1)
	{
		if( timer_6ms )             // Timer expired.
        {
            timer_6ms = RESET;
            
           	break;
       	}
	}


}
void IsKeyReleased (void)
{
unsigned int chck;
while (1)
{
  chck = ~PINC;
  chck&=0b00000111;
  if (timer_5ms)
		 	{
	 		timer_5ms = RESET;
	 		show7seg();
	 		}		    //ott lesz 0 ahol a gombot lenyomták, többi 1
  if (chck)  {wait_6ms();}    //ha check nem 0, akkor lefut >>csak akkor 0 ha a gomb el van engedve
  else
  break;


}
return;
}


void Multitap (unsigned int szam, char kybrdChars[10][10],char *stp) 
{

unsigned int check, col=0; //lenyomott bill tárolására

LCD_SendData(kybrdChars[szam][col]);
*stp=kybrdChars[szam][col];						//kiküldött karaktert tárolom stringben
str_pos++;
cur_pos++;
timer_500ms=RESET;
  
    while(1) 
    {		if (timer_5ms)
		 	{
	 		timer_5ms = RESET;
	 		show7seg();
	 		}		
			


		
      check=PINC;

      check&=0b00000111;

      if (in==check) //ha a lenyomott gomb, ugyanaz mint korábban
        {col++;
        if (kybrdChars[szam][col]=='\0') {col=0;};
		LCD_SendCommand(0x10);		//kurzor balra shifteltetés 1el
		LCD_SendData(kybrdChars[szam][col]);	
		*stp=kybrdChars[szam][col];					//ha van újranyomás, át kell írni
		IsKeyReleased();
		timer_500ms=RESET;
        }
		else if(check==0x06||check==0x05||check==0x03) {break;}
			
			
	
		

      
      
    if( timer_500ms==FIRED )            //akkor 1 sec timer indul
      {// 
      break;
      
      }
    
    }
  
											//következő karakterre ugrás, csak 1x kell, amint biztos az a karakter lesz
return;
}
  

//--------------------------------------------------------------------------------------------------------------------
// LCD függvények

void LCD_Init(void){
	LCD_D_DDR |= LCD_D_MASK;
	LCD_C_DDR |= (1<<RS)
			  |  (1<<RW)
			  |  (1<<EN);
	//DISP.SETT.
	LCD_D_PORT = BITMODE4;
	LCD_Clock();
	LCD_Clock();
	LCD_Clock();
	LCD_SendCommand(LINES); //próba

	LCD_SendCommand(LCDCLEAR);
	LCD_SendCommand(LCDON);
}

void LCD_Clock(void){
	
	wait_5ms();
	EN_HI();
	wait_5ms();
	EN_LO();
	wait_5ms();
}

void LCD_SendData(unsigned char data){
	RS_HI();			//ADAT!
	if (data>=97 && data<=122) //ha betű, csak akkor nézze hogy kis vagy nagy betű kell
	{
	LCD_D_PORT = (data-(cap*32)) & LCD_D_MASK;	//0xF0
	}
	else
	LCD_D_PORT = data & LCD_D_MASK;
	LCD_Clock();
	LCD_D_PORT = data<<4;
	LCD_Clock();
}
void LCD_SendCommand(unsigned char command){
	RS_LO();			//PARANCS!
	LCD_D_PORT = command & LCD_D_MASK;	// PORTE & 0xF0 pl Cursoff: 0x28 & 0xF0
	LCD_Clock();
	LCD_D_PORT = command<<4;
	LCD_Clock();
}
//alma
/*void LCD_SendStr(char *str){
	while(*str){
		LCD_SendData(*str++);
		//str++;
	}
}*/

void LCD_SendStr(char *str){

  LCD_SendCommand(LCDCLEAR); 
  unsigned int first,last=0;

/*
  if (page==0) {first=0; last=31;}
  	if (page==1) {first=32; last=63;}
		if (page==2) {first=64; last=95;}
			if(page==3) {first=96; last=127;}
				if(page==4) {first=128; last=159;}

*/
				


  switch (page)
  {
        case 0: first=0; last=31;break;
        case 1: first=32; last=63;break;
        case 2: first=64; last=95;break;
        case 3: first=96; last=127;break;
        case 4: first=128; last=159;break;
  }

 
 
 
	while((first<=last)&&(*(str+first))){  
		LCD_SendData(*(str+first));
		first++;
		cur_pos++;
		//str++;
	}
}

