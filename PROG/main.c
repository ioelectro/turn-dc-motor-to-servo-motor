/*
 By Liyanboy74
 https://github.com/liyanboy74
*/

#include <mega32.h>
#include <delay.h>
#include <stdio.h>

#define dir PORTD.6
#define pwm OCR1AL

#define FB_POT 1
#define IN_POT 0

const float KP=1.0;


// Voltage Reference: AVCC pin
#define ADC_VREF_TYPE ((0<<REFS1) | (1<<REFS0) | (0<<ADLAR))

// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | ADC_VREF_TYPE;
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=(1<<ADSC);
// Wait for the AD conversion to complete
while ((ADCSRA & (1<<ADIF))==0);
ADCSRA|=(1<<ADIF);
return ADCW;
}

int read_pot(int channel)
{
int i=0,adc=0,sum=0,data=0;                     
for(i=0;i<10;i++)
 {
 adc=read_adc(channel);
 sum=sum+adc;
 }           
data=sum/10.0;
return data; 
}

void control_motor(int control)
{
if(control>0)
 {
 if(control>255)control=255;   
 pwm=control;
 dir=0;
 }     
else
 {
 control=control*-1;
 if(control>255)control=255;
 pwm=255-control;
 dir=1; 
 }
}

void main(void)
{
// Declare your local variables here
int input,fb,A,err;

// Input/Output Ports initialization
DDRD=0XFF;
PORTD=0X00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 8000.000 kHz
// Mode: Fast PWM top=0x00FF
// OC1A output: Non-Inverted PWM
// OC1B output: Disconnected
// Timer Period: 0.032 ms
// Output Pulse(s):
// OC1A Period: 0.032 ms Width: 0 us
TCCR1A=(1<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (1<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

// ADC initialization
// ADC Clock frequency: 1000.000 kHz
// ADC Voltage Reference: AVCC pin
// ADC Auto Trigger Source: ADC Stopped
ADMUX=ADC_VREF_TYPE;
ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
SFIOR=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

while (1)
      {
      input = read_pot(IN_POT);
      fb 	= read_pot(FB_POT); 
       
      err=input-fb;    
       
      A=err*KP; 
             
      control_motor(A);
       
      delay_ms(1);  
      }
}
