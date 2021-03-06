/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Professional
Automatic Program Generator
� Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 4/25/2019
Author  : NeVaDa
Company : 
Comments: 


Chip type               : ATmega8
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*****************************************************/

#include <mega8.h>
#include <stdio.h>
#include <delay.h>

#ifndef RXB8
#define RXB8 1
#endif

#ifndef TXB8
#define TXB8 0
#endif

#ifndef UPE
#define UPE 2
#endif

#ifndef DOR
#define DOR 3
#endif

#ifndef FE
#define FE 4
#endif

#ifndef UDRE
#define UDRE 5
#endif

#ifndef RXC
#define RXC 7
#endif

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)


#define        DHT_DATA_IN    PINC.2  
#define        DHT_DATA_OUT    PORTC.2            
#define        DHT_DDR_DATA    DDRC.2 
#define DDROUT        1 
#define DDRIN        0 
#define DHT_ER      0 
#define DHT_OK      1 
#define DHT_Temp    0 
#define DHT_RH      1 
#define ADC_VREF_TYPE 0xE0 

typedef  signed          char int8_t; 
typedef  signed            int int16_t; 
typedef  signed long      int int32_t; 

/*    Kieu So Nguyen Khong Dau */ 
typedef  unsigned        char uint8_t; 
typedef  unsigned            int  uint16_t; 
typedef  unsigned long    int  uint32_t; 
/*    Kieu So Thuc */ 
typedef  float            float32_t; 
uint8_t nhiet=0;
uint8_t am=0; 

uint8_t buffer[5]={0,0,0,0,0}; 

uint8_t select = 0;
//------------------------------------------------------// 

// USART Receiver buffer
#define RX_BUFFER_SIZE 16
char rx_buffer[RX_BUFFER_SIZE];

#if RX_BUFFER_SIZE <= 256
unsigned char rx_wr_index,rx_rd_index,rx_counter;
#else
unsigned int rx_wr_index,rx_rd_index,rx_counter;
#endif

// This flag is set on USART Receiver buffer overflow
bit rx_buffer_overflow;

// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{
char status,data;
status=UCSRA;
data=UDR;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer[rx_wr_index++]=data;  
   if(data == 'z'){
       PORTC.0 = 0; 
     }
     if(data == 'l'){
        PORTC.0 = 1;
     }
#if RX_BUFFER_SIZE == 256
   // special case for receiver buffer size=256
   if (++rx_counter == 0)
      {
#else
   if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
   if (++rx_counter == RX_BUFFER_SIZE)
      {
      rx_counter=0;
#endif
      rx_buffer_overflow=1;
      }
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
while (rx_counter==0);
data=rx_buffer[rx_rd_index++];
#if RX_BUFFER_SIZE != 256
if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
#endif
#asm("cli")
--rx_counter;
#asm("sei")
return data;
}
#pragma used-
#endif

// USART Transmitter buffer
#define TX_BUFFER_SIZE 16
char tx_buffer[TX_BUFFER_SIZE];

#if TX_BUFFER_SIZE <= 256
unsigned char tx_wr_index,tx_rd_index,tx_counter;
#else
unsigned int tx_wr_index,tx_rd_index,tx_counter;
#endif


// USART Transmitter interrupt service routine
interrupt [USART_TXC] void usart_tx_isr(void)
{
if (tx_counter)
   {
   --tx_counter;
   UDR=tx_buffer[tx_rd_index++];
#if TX_BUFFER_SIZE != 256
   if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
#endif
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
while (tx_counter == TX_BUFFER_SIZE);
#asm("cli")
if (tx_counter || ((UCSRA & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer[tx_wr_index++]=c;
#if TX_BUFFER_SIZE != 256
   if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0;
#endif
   ++tx_counter;
   }
else
   UDR=c;
#asm("sei")
}
#pragma used-
#endif

// Standard Input/Output functions
#include <stdio.h>
//_-----------------------------------------------------//  Lay du lieu tu DHT11 
uint8_t DHT_GetTemHumi () 
{ 
    uint8_t ii,i,checksum; 
    DHT_DDR_DATA=DDROUT;  // set la cong ra 
    DHT_DATA_OUT=1; 
    delay_ms(50); 
    DHT_DATA_OUT=0; 
    delay_ms(25); // it nhat 18ms 
    DHT_DATA_OUT=1;
    delay_us(10);  
    DHT_DDR_DATA=DDRIN; 
    delay_us(60); 
    if(DHT_DATA_IN)return DHT_ER ; 
    else while(!(DHT_DATA_IN));    //Doi DaTa len 1 
    delay_us(60); 
    if(!DHT_DATA_IN)return DHT_ER; 
    else while((DHT_DATA_IN));    //Doi Data ve 0 
    //Bat dau doc du lieu 
    for(i=0;i<5;i++) 
    { 
        for(ii=0;ii<8;ii++) 
        {    
        while((!DHT_DATA_IN));//Doi Data len 1 
        delay_us(30); 
        if(DHT_DATA_IN) 
            { 
            buffer[i]|=(1<<(7-ii)); 
            while((DHT_DATA_IN));//Doi Data xuong 0 
            } 
        } 
    } 
    //Tinh toan check sum 
    checksum=buffer[0]+buffer[1]+buffer[2]+buffer[3]; 
    //Kiem tra check sum
    DHT_DDR_DATA=DDROUT;
    DHT_DATA_OUT=0;
    delay_ms(10); 
    if((checksum)!=buffer[4])return DHT_ER; 
    //Lay du lieu    
    return(buffer[2]); 
    /*
      if (select==DHT_Temp) 
      {    //Return the value has been choosen 
            return(buffer[2]); 
      } 
      if(select==DHT_RH) 
      { 
            return(buffer[0]+buffer[2]); 
      } 
    return DHT_OK;  */
    
} 

//--------------------------------------------------------------// 
// Declare your global variables here
//char c;
void main(void)
{
// Declare your local variables here
// Input/Output Ports initialization
// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
// all port C is output
PORTC=0x00;
DDRC=0x11;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
TCCR0=0x00;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600

UCSRA=0x00;
/* Enable receiver and transmitter */
UCSRB=0xD8;
/* Set frame format: 8data, 1stop bit */
UCSRC=0x86;
// baud rate 9600 => UBRR = 0x0033
UBRRH=0x00;
UBRRL=0x33;

    
    
// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// ADC initialization
// ADC disabled
ADCSRA=0x00;

// SPI initialization
// SPI disabled
SPCR=0x00;

// TWI initialization
// TWI disabled
TWCR=0x00;

// Global enable interrupts
#asm("sei")

    puts("AT\r\n");  
    delay_ms(100);

    puts("AT+CWMODE=1\r\n"); 
    delay_ms(100);

    puts("AT+CWJAP=\"ttt\",\"12345678\"\r\n");
    delay_ms(1000);
    //c = 'a';
while (1)
      {
      // Place your code here   
      /*
       for(i='a';i<='z';i++){
            putchar(i);
            
       }
      putchar('\n');
       delay_ms(500);  
       */
      // c = getchar();
      // putchar(c); 
      //delay_ms(200);
       
      
      delay_ms(200);
      //printf("nhiet do la %d\n",nhiet);
      puts("AT+CIPSTART=\"TCP\",\"192.168.43.22\",8888\r\n");
      delay_ms(100);    
      puts("AT+CIPSEND=11\r\n");    
      delay_ms(200);     
      //select = 1;
      //am= DHT_GetDoam();
      //delay_ms(2000);   
      //select = DHT_Temp;
      //nhiet= DHT_GetTemHumi(); 
      //am = buffer[0];
      DHT_GetTemHumi();
      printf("n+%d;%d\n\r",buffer[0],buffer[2]);
      /* 
      if(c == 'n'){ 
      nhiet= DHT_GetTemHumi(DHT_Temp);    
      printf("n+%d\n\r",nhiet);
      c = 'a';              
      }
      else if(c == 'a'){
      //puts("1\r\n"); 
      nhiet= DHT_GetTemHumi(DHT_Temp);    
      printf("a+%d\n\r",nhiet); 
      c = 'n';
      }   
      */
    //c = getchar();
    // putchar(c);
      delay_ms(500);
        
      }
}
