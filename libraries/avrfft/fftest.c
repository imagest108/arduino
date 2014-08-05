/*------------------------------------------------*/
/* FFTEST : A test program for FFT module         */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "suart.h"		/* Defs for using Software UART module (Debugging via AVRSP-COM) */
#include "ffft.h"		/* Defs for using Fixed-point FFT module */

#define	SYSCLK		16000000



/*------------------------------------------------*/
/* Global variables                               */

char pool[16];	/* Console input buffer */

int16_t capture[FFT_N];			/* Wave captureing buffer */
complex_t bfly_buff[FFT_N];		/* FFT buffer */
uint16_t spektrum[FFT_N/2];		/* Spectrum output buffer */



/*------------------------------------------------*/
/* Capture waveform                               */

void capture_wave (int16_t *buffer, uint16_t count)
{
	ADMUX = _BV(REFS0)|_BV(ADLAR)|_BV(MUX2)|_BV(MUX1)|_BV(MUX0);	// channel

	do {
		ADCSRA = _BV(ADEN)|_BV(ADSC)|_BV(ADFR)|_BV(ADIF)|_BV(ADPS2)|_BV(ADPS1);
		while(bit_is_clear(ADCSRA, ADIF));
		*buffer++ = ADC - 32768;
	} while(--count);

	ADCSRA = 0;
}


/* This is an alternative function of capture_wave() and can omit captureing buffer.

void capture_wave_inplace (complex_t *buffer, uint16_t count)
{
	const prog_int16_t *window = tbl_window;
	int16_t v;

	ADMUX = _BV(REFS0)|_BV(ADLAR)|_BV(MUX2)|_BV(MUX1)|_BV(MUX0);	// channel

	do {
		ADCSRA = _BV(ADEN)|_BV(ADSC)|_BV(ADFR)|_BV(ADIF)|_BV(ADPS2)|_BV(ADPS1);
		while(bit_is_clear(ADCSRA, ADIF));
		v = fmuls_f(ADC - 32768, pgm_read_word_near(window));
		buffer->r = v;
		buffer->i = v;
		buffer++; window++;
	} while(--count);

	ADCSRA = 0;
}
*/

/*------------------------------------------------*/
/* Online Monitor via an ISP cable                */

int main (void)
{
	char *cp;
	uint16_t m, n, s;
	uint16_t t1,t2,t3;


	DDRE = 0b00000010;	/* PE1:<conout>, PE0:<conin> in N81 38.4kbps */
	TCCR1B = 3;	/* clk/64 */

	xmitstr(PSTR("\r\nFFT sample program\r\n"));

	for(;;) {
		xmitstr(PSTR("\r\n>"));			/* Prompt */
		rcvrstr(pool, sizeof(pool));	/* Console input */
		cp = pool;

		switch (*cp++) {	/* Pick a header char (command) */
			case '\0' :		/* Blank line */
				break;

			case 'w' :		/* w: show waveform */
				capture_wave(capture, FFT_N);
				for (n = 0; n < FFT_N; n++) {
					s = capture[n];
					xmitf(PSTR("\r\n%4u:%6d "), n, s);
					s = (s + 32768) / 1024;
					for (m = 0; m < s; m++) xmit(' ');
					xmit('*');
				}
				break;

			case 's' :		/* s: show spectrum */
				capture_wave(capture, FFT_N);
				TCNT1 = 0;	/* performance counter */
				fft_input(capture, bfly_buff);
				t1 = TCNT1; TCNT1 = 0;
				fft_execute(bfly_buff);
				t2 = TCNT1; TCNT1 = 0;
				fft_output(bfly_buff, spektrum);
				t3 = TCNT1;
				for (n = 0; n < FFT_N / 2; n++) {
					s = spektrum[n];
					xmitf(PSTR("\r\n%4u:%5u "), n, s);
					s /= 512;
					for (m = 0; m < s; m++) xmit('*');
				}
				xmitf(PSTR("\r\ninput=%u, execute=%u, output=%u (x64clk)"), t1,t2,t3);
				break;

			default :		/* Unknown command */
				xmitstr(PSTR("\n???"));
		}
	}
}
