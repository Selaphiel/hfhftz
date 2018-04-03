#define __SFR_OFFSET 0 // Use 0 for the I/O register offset
#include <avr/io.h> // Defines I/O port aliases
.global init_in_asm
.global LED_out_in_asm
.global GOMB_in_in_asm
.global seven_segment_in_asm


ANSL= 0            ;Eredmény alsó Byte     
ANSH= 1            ;Eredmény felső Byte
        ;Maradék R2
A = 16           ;Szorzandó   
B = 18           ;Szorzó
C=20           ;Bitek száma

lowled=21

.section .text ; Defines a code section



init_in_asm:
ldi r16, 0xFF
out DDRA, r16   ;DONE
ldi r16, 0xF0  
out DDRB, r16   ;DONE
out DDRD, r16   ;DONE
eor r16, r16
sts DDRG, r16   ;DONE
ldi r16, 0x78   ;DONE 0b01111000;
out DDRC, r16   ; megnézni hogy lcd-t assemblyben így kell e engedélyezni.

ldi r16, 0
out TCCR1A,r16       ;Timer/counter 1 inicializálás
; 			       ;Clear Timer on Compare or CTC mode beállítása f=1ms
;             	 	 ;f=F_cpu/(2*prescale * (1+OCRnA))
;        			;prescale = 1 (1<<cs10) 
;
;
ldi r16,0x09
out TCCR1B,r16
ldi r16, 0
sts TCCR1C,r16

ldi r16, 0x7E
ldi r17, 0x3E
out OCR1AH,r17 ;OCR1AH
out OCR1AL,r16 ;OCR1AL

ldi r16, OCIE1A
ori r16,0x10
out TIMSK, r16
ret
;--------------------------------------------------------------------------------------------------------------------------
LED_out_in_asm:
;r24-re kapott értéket osztja 160al eredmény *8 adja meg a világítandó ledek számát
	   
	   	mov A,r24         ;osztandó érték betöltése
        LDI B,20           ;osztó betöltése
DIV88:
        LDI C,9           ;Load bit counter
        SUB R2,R2       ;Clear Remainder and Carry
        MOV ANSL,A         ;Copy Dividend to Answer
LOOP:   ROL ANSL           ;Shift the answer to the left
        DEC C             ;Decrement Counter
         BREQ DONE        ;Exit if eight bits done
        ROL R2           ;Shift the remainder to the left
        SUB R2,B         ;Try to Subtract divisor from remainder
         BRCC SKIP        ;If the result was negative then
        ADD R2,B         ;reverse the subtraction to try again
        CLC               ;Clear Carry Flag so zero shifted into A 
         RJMP LOOP        ;Loop Back
SKIP:   SEC               ;Set Carry Flag to be shifted into A
         RJMP LOOP
DONE:

	 	
 
                 ;Eredmény ANSL és ANSH-ban (lefoglalva, ha 256 karakternél több kellene)
 LEDL:;kimenetre nem elég a számot kirakni, shiftelni kell?
 out PORTD, r23
swap r24
 out PORTB, r21
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
 ;call negativ
neg r24
ret

ledout_in_asm:
r24 ;karakterek száma
;ahányszor csökkenthető 20al, annyi led világítson
kivon:
	ldi r16,20 ;20asával 12,5%
	sub r24,r16
	BRLO DONE ;HA KISEBB
	breq LED  ;HA 0
	inc amount;világitandó ledek száma+1
	RJMP kivon;ahány karakter világítson
		;amig nem osztható 20al addig ne világitson led
	
	
LED:
	világító ledek száma >> portb
LOWINIT:
	ldi r21,0b11110000 ;LEDmask
	ldi r22,0b11100000 ;első szám
	
Low:	mov r30, r22 ;első szám eltárolása
	eor r30, r21 ; xor kapcsolat mask és első szám között:1111^1110=0001
	out PORTB, r30
	dec amount
	breq done
	lsl r22	     ;1110 ból 1100 lesz ha 
	breq HIGHINIT	     ;ha 0000 felső bitekre megy
	rjmp low:
HIGHINIT:
	ldi r22,0b11100000;
High: 	mov r30, r22 ;első szám eltárolása
	eor r30, r21 ; xor kapcsolat mask és első szám között:1111^1110=0001
	out PORTD, r30
	out PORTB,r21
	dec amount
	breq done
	lsl r22	     ;1110 ból 1100 lesz ha
	breq DONE;ha 0000 felső bitekre megy
	rjmp High
DONE:
	RET


	
	
