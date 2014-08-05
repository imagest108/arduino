;---------------------------------------------------------------------------;
; Software implemented UART via an ISP cable.                               ;
; (C)ChaN, 2005 (http://elm-chan.org/)                                      ;
;---------------------------------------------------------------------------;
; MOSI and MISO are used as inverted signals to connect to RS-232C line
; directly. The MISO pin must be configured as an output before using xmit().
; Following definitions must be changed for each device, clock and bps.
; Pin definitions are for most 20/40 pin devices except for TN26.
; Any interrupt during xmit() or rcvr() is being executed will be defered
; until end of the function. When use xmit() with any interrupt, choose
; higher bit rate as possible to minimize critical time. But rcvr() and any
; interrupt cannot be used simultaneously.
;
;            1MHz  2MHz  4MHz  6MHz  8MHz  10MHz  12MHz  16MHz  20MHz
;   2.4kbps   138     -     -     -     -      -      -      -      -
;   4.8kbps    68   138     -     -     -      -      -      -      -
;   9.6kbps    33    68   138   208     -      -      -      -      -
;  19.2kbps     -    33    68   102   138    173    208      -      -
;  38.4kbps     -     -    33    50    68     85    102    138    172
;  57.6kbps     -     -    21    33    44     56     68     91    114
; 115.2kbps     -     -     -     -    21     27     33     44     56

.nolist
#include <avr/io.h>
.list

#define	BPS	138	/* Bit delay. (see above) */

#define	TXREG	_SFR_IO_ADDR(PORTB)	/* MISO: Port and bit position */
#define	TXBIT	PB6
#define RXREG	_SFR_IO_ADDR(PINB)	/* MOSI: Port and bit position */
#define	RXBIT	PB5

#define USE_OUTPUT	/* Remove this when output functions are not needed */
#define USE_INPUT	/* Remove this when input functions are not needed */
#define USE_STRFUNCS	/* Remove this when string functions are not needed */


#ifdef SPM_PAGESIZE
.macro	_LPMI	reg
	lpm	\reg, Z+
.endm
.macro	_MOVW	dh,dl, sh,sl
	movw	\dl, \sl
.endm
#else
.macro	_LPMI	reg
	lpm
	mov	\reg, r0
	adiw	ZL, 1
.endm
.macro	_MOVW	dh,dl, sh,sl
	mov	\dl, \sl
	mov	\dh, \sh
.endm
#endif



#ifdef	USE_OUTPUT
;---------------------------------------------------------------------------;
; Transmit a byte in serial format of N81
;
;Prototype: void xmit (uint8_t data);
;Size: 16 words

.global xmit
.func xmit
xmit:
	in	r0, _SFR_IO_ADDR(SREG)	;Save flags

	com	r24		;C = start bit
	ldi	r25, 10		;Bit counter
	cli			;Start critical section

1:	ldi	r23, BPS-1	;----- Bit transferring loop 
2:	dec	r23     	;Wait for a bit time
	brne	2b		;/
	brcs	3f		;MISO = bit to be sent
	cbi	TXREG, TXBIT	;
3:	brcc	4f		;
	sbi	TXREG, TXBIT	;/
4:	lsr	r24     	;Get next bit into C
	dec	r25     	;All bits sent?
	brne	1b	     	;  no, coutinue

	out	_SFR_IO_ADDR(SREG), r0	;End of critical section
	ret
.endfunc


#ifdef USE_STRFUNCS
;---------------------------------------------------------------------------
; Transmit an ASCIZ string on the program memory
;
;Prototype: void xmitstr (const prog_char *str);
;Size: 10/7 words

.global xmitstr
.func xmitstr
xmitstr:
	_MOVW	ZH,ZL, r25,r24	;Pointer to ASCIZ string
1:	_LPMI	r24		;Get a character
	tst	r24      	;Exit if end of string
	breq	2f		;/
	rcall	xmit		;Transmit it
	rjmp	1b	     	;Continue
2:	ret
.endfunc



;---------------------------------------------------------------------------
; Numeral string transmission
;
;Prototype: void xmitval (uint16_t value, int8_t base, int8_t digits);
;Size: 51 words
;
; value  base  digits   output
;   100    10       6   "   100"
;   100    10       1   "100"
;  1024    16       4   " 400"
;  1024    16      -4   "0400"
;  0x55     2      -8   "01010101"
; 65535   -10       1   "-1"

.global xmitval
.func xmitval
xmitval:			;r25:r24:value, r22:base, r20:digits
	clr	r19      	;r19:stack level
	ldi	r30, ' '	;r30:sign
	ldi	r31, ' '	;r31:filler
	sbrs	r22, 7		;When base indicates signd format and the value
	rjmp	0f		;is minus, add a '-'.
	neg	r22		;
	sbrs	r25, 7		;
	rjmp	0f		;
	ldi	r30, '-'	;
	com	r24		;
	com	r25		;
	adc	r24, r1		;
	adc	r25, r1		;/
0:	sbrs	r20, 7		;When digits indicates zero filled,
	rjmp	1f		;filler is '0'.
	neg	r20		;
	ldi	r31, '0'	;/
				;----- string conversion loop
1:	ldi	r21, 16		;r23 = r25:r24 \ r22
	clr	r23		;r25:r24 /= r22
2:	lsl	r24		;
	rol	r25		;
	rol	r23		;
	cp	r23, r22	;
	brcs	3f		;
	sub	r23, r22	;
	inc	r24		;
3:	dec	r21		;
	brne	2b		;/
	cpi	r23, 10		;r23 is a numerical digit '0'-'F'
	brcs	4f		;
	subi	r23, -7		;
4:	subi	r23, -'0'	;/
	push	r23		;Stack it
	inc	r19		;/
	cp	r24, r21	;Repeat until r25:r25 gets zero
	cpc	r25, r21	;
	brne	1b		;/

	cpi	r30, '-'	;Stack a minus sign if needed
	brne	5f		;
	push	r30		;
	inc	r19		;/
5:	cp	r19, r20	;Stack filler
	brcc	6f		;
	push	r31		;
	inc	r19		;
	rjmp	5b		;/

6:	pop	r24		;Output stacked digits and exit
	rcall	xmit		;
	dec	r19		;
	brne	6b		;
	ret			;/
.endfunc



;---------------------------------------------------------------------------;
; Formatted string transmission
;
;Prototype: void xmitf (const prog_char *format, ...);
;Size: 70/64 words

.global xmitf
.func xmitf
xmitf:
	in	XL, _SFR_IO_ADDR(SPL)
#ifdef SPH
	in	XH, _SFR_IO_ADDR(SPH)
#else
	clr	XH
#endif
	adiw	XL, 3		;X = pointer to arguments
	ld	ZL, X+		;Z = pointer to format string
	ld	ZH, X+		;/

00:	_LPMI	r24		;Get a format char
	cpi	r24, 0		;End of format string?
	breq	90f		;/
	cpi	r24, '%'	;Is format?
	breq	20f		;/
01:	rcall	xmit		;Put a normal character
	rjmp	00b		;/
90:	ret

20:	ldi	r20, 0		;r20: digits
	clt			;T: filler
21:	_LPMI	r24		;Get flags
	cpi	r24, '%'	;Is '%'?
	breq	01b		;/
	cpi	r24, '0'	;Zero filled?
	brne	23f		;
	set			;/
22:	_LPMI	r24		;Get width
23:	cpi	r24, '9'+1	;
	brcc	24f		;
	subi	r24, '0'	;
	brcs	90b		;
	lsl	r20		;
	mov	r0, r20		;
	lsl	r20		;
	lsl	r20		;
	add	r20, r0		;
	add	r20, r24	;
	rjmp	22b		;/

24:	mov	r23, r24	;r23 = type
	ld	r24, X+		;r25:r24 = value
	ld	r25, X+		;/
	cpi	r23, 'c'	;Is type character?
	breq	01b		;/
	cpi	r23, 's'	;Is type string?
	breq	50f		;/
	cpi	r23, 'X'	;Is type hexdecimal?
	ldi	r22, 16		;
	breq	40f		;/
	cpi	r23, 'u'	;Is type unsigned decimal?
	ldi	r22, 10		;
	breq	40f		;/
	cpi	r23, 'd'	;Is type signed decimal?
	ldi	r22, -10	;
	breq	40f		;/
	cpi	r23, 'b'	;Is type binary?, or abort.
	ldi	r22, 2		;
	brne	90b		;/
40:	brtc	41f		;Output the value
	neg	r20		;
41:	push	ZH		;
	push	ZL		;
	rcall	xmitval		;
41:	pop	ZL		;
	pop	ZH		;
	rjmp	00b		;/
50:	push	ZH		;Output the ROM string
	push	ZL		;
	rcall	xmitstr		;
	rjmp	41b		;/
.endfunc

#endif	/* USE_STRFUNCS */
#endif	/* USE_OUTPUT */



#ifdef	USE_INPUT
;---------------------------------------------------------------------------;
; Receive a byte
;
;Prototype: uint8_t rcvr (void);
;Size: 19 words

.global rcvr
.func rcvr
rcvr:
	in	r0, _SFR_IO_ADDR(SREG)	;Save flags

	ldi	r24, 0x80	;Receiving shift reg
	cli			;Start critical section

1:	sbic	RXREG, RXBIT	;Wait for falling edge on MOSI pin
	rjmp	1b
2:	sbis	RXREG, RXBIT	;Wait for rising edge on MOSI pin
	rjmp	2b
	ldi	r25, BPS/2	;Wait for half bit time
3:	dec	r25
	brne	3b

4:	ldi	r25, BPS	;----- Bit receiving loop
5:	dec	r25     	;Wait for a bit time
	brne	5b		;/
	lsr	r24     	;Next bit
	sbis	RXREG, RXBIT	;Get a bit into r24.7
	ori	r24, 0x80
	brcc	4b	     	;All bits received?  no, continue

	out	_SFR_IO_ADDR(SREG), r0	;End of critical section
	ret
.endfunc


#ifdef USE_STRFUNCS
;---------------------------------------------------------------------------;
; Console input
;
;Prototype: void rcvrstr (char *buffer, uint8_t buffsize);
;Size:  24/23 words

.global rcvrstr
.func rcvrstr
rcvrstr:
	_MOVW	ZH,ZL, r25,r24	;Pointer to input buffer
	ldi	r21, 1		;Character count (+'\0')
0:	rcall	rcvr		;Receive a character
	cpi	r24, '\r'	;Enter?
	breq	9f		;/
	cpi	r24, '\b'	;Backspace?
	breq	2f		;/
	cp	r21, r22	;Buffer full?
	brcc	0b		;/
	cpi	r24, ' '	;Invisible code?
	brcs	0b		;/
	st	Z+, r24		;Store a character
	inc	r21		;count++
1:	rcall	xmit		;Show the character
	rjmp	0b		;Continue
2:	cpi	r21, 1		;Backspace: Buffer empty?
	breq	0b		;/
	dec	r21		;count--
	sbiw	ZL, 1		;/
	rjmp	1b		;Move cursor left
9:	rcall	xmit		;Return cursor.
	st	Z, r1		;Terminate with a '\0' and exit
	ret			;/
.endfunc



;---------------------------------------------------------------------------;
; Pick a value from a string
;
;Prototype: uint8_t pickval (char **string, uint16_t *result, uint8_t base);
;Size:  61/59 words

.global pickval
.func pickval
pickval:
	_MOVW	ZH,ZL, r25,r24	;Z = pointer to pointer to numerical string
	ld	XL, Z+	 	;X = pointer to numerical string
	ld	XH, Z+ 		;/
	clr	r18     	;r19:r18 = input register
	clr	r19     	;/
	clt			;Unsigned or plus value

00:	ld	r24, X  	;Skip pre-spaces
	cpi	r24, '-'	;Is signed minus value?
	brne	01f		;
	set			;
	rjmp	16f		;/
01:	cpi	r24, ' '	;End of string?
	brcs	90f		;/
	brne	11f
	adiw	XL, 1
	rjmp	00b

10:	ld	r24, X  	;Numerical string => Integer conversion loop
	cpi	r24, ' '+1      ;Exit if end of a number
	brcs	91f	    	;/
11:	cpi	r24, 'a'	;Convert a digit to sequencial number
	brcs	12f	    	;
	subi	r24, 0x20       ;
12:	subi	r24, '0'	;
	brcs	90f	    	;
	cpi	r24, 10 	;
	brcs	13f	    	;
	cpi	r24, 17 	;
	brcs	90f	    	;
	subi	r24, 7  	;
13:	cp	r24, r20	;
	brcc	90f	    	;/
	ldi	r25, 17 	;r19:r18 *= r20(base)
	sub	r21, r21	;
14:	brcc	15f	    	;
	add	r21, r20	;
15:	ror	r21     	;
	ror	r19     	;
	ror	r18     	;
	dec	r25     	;
	brne	14b	    	;/
	add	r18, r24	;r19:r18 += r24(digit)
	adc	r19, r1 	;/
16:	adiw	XL, 1	  	;Next digit
	rjmp	10b

90:	clr	r24     	;Exit with error(0)
	rjmp	92f
91:	ldi	r24, 1  	;Exit with successful(1)
	brtc	92f	    	;Negate when minus value
	com	r18     	;
	com	r19     	;
	adc	r18, r1 	;
	adc	r19, r1 	;/
92:	st	-Z, XH	 	;Store the string pointer back
	st	-Z, XL	 	;/
	_MOVW	ZH,ZL, r23,r22	;Store the result
	st	Z+, r18 	;
	st	Z+, r19 	;/
	ret
.endfunc

#endif	/* USE_STRFUNCS */
#endif	/* USE_INPUT */

