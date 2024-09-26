/*
ArshiaMadadi
GPS-NEO-6M-0-001 and Blutooth - HC-05
*/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

volatile char buf[100];
volatile char ind, stringReceived;
char gpgga[] = {'$', 'G', 'P', 'G', 'G', 'A'};
char latitude[12]; // Stored latitude
char longitude[12]; // Stored longitude

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

unsigned char UART_Rxchar()
{
	while ((UCSRA & (1 << RXC)) == 0);
	return (UDR);
}

void UART_Txchar(char ch)
{
	while ((UCSRA & (1 << UDRE)) == 0);
	UDR = ch;
}

void UART_Sendstring(char *str)
{
	unsigned char j = 0;

	while (str[j] != 0)
	{
		UART_Txchar(str[j]);
		j++;
	}
}

void processGPSData()
{
	if (buf[0] == '$' && buf[1] == 'G' && buf[2] == 'P' && buf[3] == 'G' && buf[4] == 'G' && buf[5] == 'A')
	{
		// Extract latitude
		int i, j = 0;
		for (i = 17; buf[i] != ',' && j < 11; i++)
		{
			latitude[j++] = buf[i];
		}
		latitude[j] = '\0'; // Null-terminate the string

		// Extract longitude
		j = 0;
		for (i = 30; buf[i] != ',' && j < 11; i++)
		{
			longitude[j++] = buf[i];
		}
		longitude[j] = '\0'; // Null-terminate the string

		// Send the latitude and longitude over Bluetooth
		UART_Sendstring("Latitude: ");
		UART_Sendstring(latitude);
		UART_Sendstring("\r\n");

		UART_Sendstring("Longitude: ");
		UART_Sendstring(longitude);
		UART_Sendstring("\r\n");
	}
}

int main(void)
{
	serialbegin(); // Initialize UART for GPS
	UCSRB |= (1 << RXEN) | (1 << TXEN); // Enable UART receiver and transmitter for Bluetooth
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); // Set data frame to 8-bit for Bluetooth
	UBRRL = BAUD_PRESCALE;
	UBRRH = (BAUD_PRESCALE >> 8);

	sei(); // Enable global interrupts

	while (1)
	{
		if (stringReceived == 1)
		{
			cli(); // Disable interrupts while processing data
			processGPSData(); // Extract and send GPS data
			ind = 0; // Reset index
			stringReceived = 0; // Reset flag
			sei(); // Re-enable interrupts
		}

		// Bluetooth control (optional, depends on your specific needs)
		int read = UART_Rxchar();
		if (read == '1')
		{
			PORTB |= (1 << PB0);
			UART_Sendstring("LED ON");
		}
		else if (read == '2')
		{
			PORTB &= ~(1 << PB0);
			UART_Sendstring("LED OFF");
		}
		else if (read == 'a')
		{
			PORTB |= (1 << PB1);
			UART_Sendstring("Relay On");
		}
		else if (read == 'b')
		{
			PORTB &= ~(1 << PB1);
			UART_Sendstring("Relay OFF");
		}
	}

	return 0;
}
