/*
ArshiaMadadi
Blutooth - HC-05
*/

#define F_CPU 8000000UL
#include <avr/io.h>

#define USART_BAUDRATE 9600
#define BAUD (((F_CPU / (USART_BAUDRATE*16UL)))-1)

unsigned char UART_Rxchar(){
	while((UCSRA &(1<<RXC))  == 0);
	return(UDR);
}

void UART_Txchar(char ch){
	while((UCSRA & (1<<UDRE)) == 0);
	UDR = ch;
}

void UART_Sendstring(char *str){
	unsigned char j =0;
	
	while(str[j] != 0){
		UART_Txchar(str[j]);
		j++;
		
	}
}

int main(void)
{
	UCSRB |= (1<<RXEN) | (1<<TXEN);
	UCSRC |= (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1);
	UBRRL = BAUD;
	UBRRH = (BAUD>>8);
	
	DDRB = 0xFF;
	while (1)
	{
		int read = UART_Rxchar();
		if (read == '1'){
			PORTB |= (1<<PB0);
			UART_Sendstring("LED ON");
		}
		else if (read == '2'){
			PORTB &= ~(1<<PB0);
			UART_Sendstring("LED OFF");
		}
		else if (read == 'a'){
			PORTB |= (1<<PB1);
			UART_Sendstring("Relay On");
		}
		else if (read == 'b'){
			PORTB &= ~(1<<PB1);
			UART_Sendstring("Relay OFF");
		}
	}
}

