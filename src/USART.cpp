#include <USART.h>

//function to initialize the USART to 8 bit char and 1 stop bit
void USART_init(void)
{
    //set baud rate
    UBRR0 = BAUD_RATE; 
    //set frame format: 8 data, 1 stop bit.
    UCSR0C = ((0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00));
    //enable receiver and transmitter 
    UCSR0B = ((1<<RXEN0) | (1<<TXEN0));
}
//function to send one char 
void USART_send(unsigned char data)
{
	//while transmit buffer is not empty loop
	while(!(UCSR0A & (1<<UDRE0)));
    //when buffer is empty write data to transmit
    UDR0 = data;
}
//function to send a string
void USART_send_string(char* str)
{
	while(*str != '\0')
	{
		USART_send(*str);
		str++;
	}
}
//function to receive a char
unsigned char USART_receive(void)
{
	//wait for data to be received
	while(!(UCSR0A & (1<<RXC0)));
	// get the received data and return it
	return UDR0;
}	

void USART_get_string(char* str)
{
	//to do for the lab
	unsigned char i = 0;
	unsigned char c;
  
	while(1)	// Loop forever
	{
		c = USART_receive();	// Grab our next character
		if(c != '\n')			// Don't output the new line character in serial
		{
			USART_send(c);  // Send to serial
		}
		if(c == '\0' || c == '\r')
		{
			break;	// Break on NULL character
		}
		if (c == '\n')
		{
			continue;	// Skip new line character and loop again
		}
		str[i] = c;	// Write into the supplied buffer
		i++;        // Next character index
	}
	str[i + 1] = 0;	// Make sure string has a null at the end
}