AVR assembly és AVR C modulok együttes használata AVR C projektben
Összeállította: Sándor Tamás
//AVR C project létrehozása során a main.c AVR C modulba a következő forráskód részletet
//elhelyezni. A 3. oldalon található assembly forráskódot egy kieg.s fájlban helyezzük el, amelyből a .s
//kiterjesztés az assembly forráskódra utal. A további szintaktikai és használati összefoglaló a 4.
//oldalon került összefoglalásra.
// A szükséges header állományok
#include <avr/io.h>
#include <avr/interrupt.h>
//Az assembly modulban található szubrutinok felsorolása. Az extern kulcsszó
//segítségével jelezhetjük azt, hogy a következő négy függvény egy külső modulban fog
//majd előfordulni. A felsorolt szubrutinoknak nulla (void), egy vagy több bemenő
//paramétere is lehet. A LED_out_in_asm(uint8_t) egy egy bájtos előjelnélküli bemenő
//paraméterrel rendelkezik. Maga LED_out_in_asm szubrutin az átadott paramétert az r24
//regiszteren keresztül veszi át. A seven_segment_in_asm(uint8_t, uint8_t) esetében már
//két darab előjelnélküli egy bájtos érték kerül átadásra a seven_segment_in_asm
//szubrutinnak az r24-es és a r22-es regisztereken keresztül.
extern void init_in_asm(void);
extern void LED_out_in_asm(uint8_t);
extern uint8_t GOMB_in_in_asm(void);
extern void seven_segment_in_asm(uint8_t, uint8_t);
void balra(void);
void jobbra(void);
void balra_jobbra(void);
void knight_rider(void);
volatile uint8_t LED=0x01, LED1=0x01, LED2=0x80, GOMB=0, seven_seg[4]={0}, digit=0;
volatile uint8_t irany=0, timer_num=0;
int main(void)
{
 init_in_asm();
while(1)
 {
}

 return 0;
}
ISR(TIMER0_OVF_vect)
{
int8_t GOMB1=0;
if(timer_num == 0x40)
{
GOMB1=GOMB_in_in_asm();
if(GOMB1)
{
GOMB=GOMB1;
if(GOMB==4){LED=0x01;}
if(GOMB==8){LED1=0x01;LED2=0x80;}
}
switch(GOMB)
{
case 1: balra(); break;
case 2: jobbra(); break;
case 4: balra_jobbra(); break;
case 8: knight_rider(); break;
}
}
if(!timer_num)
{
if(seven_seg[0]==9)
{
seven_seg[0]=0;
if(seven_seg[1]==9)
{
seven_seg[1]=0;
if(seven_seg[2]==9)
{
seven_seg[2]=0;
if(seven_seg[3]==9)
{
seven_seg[3]=0;
}
else seven_seg[3]++;
}
else seven_seg[2]++;
}
else seven_seg[1]++;
}
else seven_seg[0]++;
}
timer_num--;
LED_out_in_asm(LED);
seven_segment_in_asm(seven_seg[digit],digit);
if(digit==3) digit=0; else digit++;
}
uint8_t negativ(int8_t val)
{
 return ~val;
}
void balra()
{
if(LED==0x80) LED=1; else LED<<=1;
}
void jobbra()
{
if(LED==0x01) LED=0x80; else LED>>=1;
}
void balra_jobbra()
{
if(LED==0x01) irany=0;
if(LED==0x80) irany=1;
if(irany) LED>>=1; else LED<<=1;
}
void knight_rider()
{
if(LED1==0x01) LED1=0x80; else LED1>>=1;
if(LED2==0x80) LED2=0x01; else LED2<<=1;
LED=LED1|LED2;
}

#define __SFR_OFFSET 0 // Use 0 for the I/O register offset
#include <avr/io.h> // Defines I/O port aliases
.global init_in_asm
.global LED_out_in_asm
.global GOMB_in_in_asm
.global seven_segment_in_asm

.section .text ; Defines a code section
init_in_asm:
ldi r16, 0xff
out DDRA, r16
ldi r16, 0xf0
out DDRB, r16
out DDRD, r16
eor r16, r16
sts DDRG, r16
ldi r16, 4
out TCCR0, r16
ldi r16, 1
out TIMSK, r16
sei
ret
LED_out_in_asm:
 out PORTD, r24
swap r24
 out PORTB, r24
swap r24
 ret

;
GOMB_in_in_asm:
 lds r24, PING
 ret

;
seven_segment_in_asm:
ori r24, 0x80
swap r22
or r24, r22
out PORTA, r24
 call negativ
neg r24
ret
C és az assembly nyelv együttes használata
 Lehetséges C függvény hívása assembly függvényként
 Lehetséges assembly modulból C függvény hívására
AVR Studio assembler projektjében nem lehetséges integrálni a két nyelvet (C és assembly), kizárólag
csak az AVR C projektben.
Az AVR C projektekben a GCC compiler mind a két C és assembly nyelvet lehet használni.
 .s fájlok (megfelel a .asm fájlnak) assembly forráskód,
 .c fájlok C forráskóddal,
 .cpp fájlok C++ forráskóddal.
Néhány különbség van az AVR Studio assembler és GCC tools szintaktikája között, ezek a következők:
 #include <avr/io.h> megfelel a .include "m32def.inc"
 .segment .data megfelel a .dseg
 .segment .text megfelel a .cseg
 .asciz "message" megfelel a .db "message", 0 (a karaktertömb automatikusan 0-val kerül
feltöltésre)
 lo8() megfelel a HIGH()
 hi8() megfelel a LOW()
 az .s fájlkiterjesztés megfelel az .asm fájlkiterjesztésnek
 Nem kell .org direktiva, hanem a fordító automatikusan kezeli a kódszegmens kezdetét,
adatszegmens esetében pedig 0x60 foglalhatunk csak területet, ez ATMEGA128 esetében
0x100-t jelenti.
 Az .s kiterjesztésű assembly forrást tartalmazó fájl a következő GCC direktívákat kell
tartalmaznia:
o #include <avr/io.h> — definiálja a PORTB, SREG, stb.
o #define __SFR_OFFSET 0
o C Compiler a következő regisztereket használja:
 függvények paraméterátadásra a R25:8 (R25-től R8-ig)
 uint8_t — előjelnélküli 8-bites egész
 uint8_t function(uint8_t i, uint8_t j);
 i átadásra kerül az R25:24 regiszterekben, mivel 8 bites így csak a R24-ben
 j átadásra kerül az R23:22 regiszterekben, mivel 8 bites így csak a R22-ben
 8-bites változó esetében az R24-es regiszterben történik a paraméter
visszatérési értékének a megadása
 16-bites változó esetében az R25:R24-es regiszterben történik a paraméter
visszatérési értékének a megadása
 32-bites változó esetében az R25:R22-es regiszterben történik a paraméter
visszatérési értékének a megadása
 64-bites változó esetében az R25:R18-es regiszterben történik a paraméter
visszatérési értékének a megadása
