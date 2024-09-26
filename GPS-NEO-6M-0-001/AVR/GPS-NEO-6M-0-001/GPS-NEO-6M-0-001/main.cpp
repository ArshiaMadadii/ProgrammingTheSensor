/*
ArshiaMadadi
GPS-NEO-6M-0-001
*/

#define F_CPU 8000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 8000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

volatile char buf[100];
volatile char ind, stringReceived;
char gpgga[] = {'$', 'G', 'P', 'G', 'G', 'A'};
char latitude[12]; //Stored latitude
char logitude[12]; //Stored longitude

void serialbegin()
{
	UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); // Select UCSRC and set data frame to 8-bit
	UBRRH = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
	UBRRL = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE); // Enable UART receiver, transmitter and RX complete interrupt
}

ISR(USART_RXC_vect)
{
	char ch = UDR; // Read received data from UART
	buf[ind] = ch; // Store received character in buffer
	ind++; // Increment index

	if (ind < 7)
	{
		if (buf[ind - 1] != gpgga[ind - 1]) // Check for '$GPGGA' pattern
		ind = 0; // Reset index if pattern mismatched
	}

	if (ind >= 50)
	stringReceived = 1; // Set flag when buffer reaches a certain size
}


void serialwrite(char ch)
{
	while ((UCSRA & (1 << UDRE)) == 0); // Wait for empty transmit buffer
	UDR = ch; // Transmit data
}

void serialprint(char *str)
{
	while (*str)
	{
		serialwrite(*str++); // Transmit string character by character
	}
}

void serialprintln(char *str)
{
	serialprint(str); // Transmit string
	serialwrite(0x0d); // Transmit carriage return (CR)
	serialwrite(0x0a); // Transmit line feed (LF)
}


int main()
{
	serialbegin(); // Initialize UART
	sei(); // Enable global interrupts

	while (1)
	{
		if (stringReceived == 1)
		{
			cli(); // Disable interrupts while processing data
			serialprint("Received String:");
			for (int i = 0; i < ind; i++)
			serialwrite(buf[i]); // Transmit received data
			ind = 0; // Reset index
			stringReceived = 0; // Reset flag
			serialprintln(" "); // Transmit new line
			sei(); // Re-enable interrupts
		}
	}

	return 0;
}

