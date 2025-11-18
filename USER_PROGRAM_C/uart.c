#include "uart.h"

void uart0_init(void)
{
	_brgh0 = 1;	  // highspeed baudrate
	_brg0 = 0x33; // baudrate config:9600  Baud rate=fH/[16×(N+1)]

	// setdata format
	_bno0 = 0;	 // 8bit data
	_pren0 = 0;	 // 0: Parity function is disabled
	_stops0 = 0; // 1 stop bit

	/*remap pin TX0 = PD7
	TX0PS1~TX0PS0: TX0 pin remapping function selection
	00: TX0 on PA3
	01: TX0 on PA3
	10: TX0 on PC3
	11: TX0 on PD7
	*/
	_tx0ps1 = 1;
	_tx0ps0 = 1;

	// enable uart0_init
	_uarten0 = 1;
	_txen0 = 1;
	_rxen0 = 0;
}

void uart0_send_byte(unsigned char data)
{
	while (!_txif0)
		;							  // waitting transmit free
	volatile unsigned char s = _u0sr; // theo sequence
	(void)s;
	_txr_rxr0 = data; // write data to uart transmit
	while (!_tidle0)
		; // waitting transmit OK
}

void uart0_send_string(const char *s)
{
	while (*s)
		uart0_send_byte((unsigned char)*s++);
}

void uart0_send_number(uint32_t x)
{
	char buf[6];
	int i = 0;
	if (x == 0)
	{
		uart0_send_byte('0');
		return;
	}
	while (x > 0)
	{
		buf[i++] = (char)((x % 10) + '0');
		x /= 10;
	}
	while (i--)
		uart0_send_byte(buf[i]);
}