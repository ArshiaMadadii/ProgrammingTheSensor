/*
ArshiaMadadi
SIM800
*/

#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 8000000UL  // Microcontroller frequency, here 8 MHz

// USART functions for sending and receiving characters
void USART_Init(unsigned int baud);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);
void USART_Transmit_String(const char *str);
void USART_Check_Response(void);

// Function to send SMS
void sendSMS(const char *receiverNumber, const char *message);

int main(void) {
	// USART configuration
	USART_Init(9600);  // Baud rate of 9600 bits per second

	while (1) {
		sendSMS("989361130730", "HELLO, test");  // Call function to send SMS
		_delay_ms(60000);  // Delay for one minute
	}
}

void USART_Init(unsigned int baud) {
	unsigned int ubrr = F_CPU / 16 / baud - 1;
	UBRRH = (unsigned char)(ubrr >> 8);  // Set high byte of the baud rate
	UBRRL = (unsigned char)ubrr;  // Set low byte of the baud rate
	UCSRB = (1 << RXEN) | (1 << TXEN);  // Enable receiver and transmitter
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);  // Set frame format: 8 data bits, 1 stop bit
}

void USART_Transmit(unsigned char data) {
	while (!(UCSRA & (1 << UDRE)));  // Wait until the transmit buffer is empty
	UDR = data;  // Transmit data
}

unsigned char USART_Receive(void) {
	while (!(UCSRA & (1 << RXC)));  // Wait for data to be received
	return UDR;  // Return received data from buffer
}

void USART_Transmit_String(const char *str) {
	while (*str != '\0') {
		USART_Transmit(*str);  // Transmit each character in the string
		str++;
	}
}

void USART_Check_Response(void) {
	unsigned char response;
	while (1) {
		response = USART_Receive();
		if (response == '\r' || response == '\n') {
			break;
		}
	}
}

void sendSMS(const char *receiverNumber, const char *message) {
	// Send AT command to set text mode for SMS
	const char cmd_AT_CMGF[] = "AT+CMGF=1\r";
	USART_Transmit_String(cmd_AT_CMGF);
	_delay_ms(1000);  // Wait for the command to be processed
	USART_Check_Response();  // Check response from the module

	// Send AT command to set the recipient's phone number
	const char cmd_AT_CMGS[] = "AT+CMGS=\"";
	USART_Transmit_String(cmd_AT_CMGS);
	USART_Transmit_String(receiverNumber);
	USART_Transmit('"');
	USART_Transmit('\r');
	_delay_ms(1000);  // Wait for the command to be processed
	USART_Check_Response();  // Check response from the module

	// Send the SMS message text
	USART_Transmit_String(message);

	// Send Ctrl+Z to finalize and send the SMS
	USART_Transmit(26);  // ASCII code for Ctrl+Z
	_delay_ms(1000);  // Wait for the message to be sent
	USART_Check_Response();  // Check response from the module
}
