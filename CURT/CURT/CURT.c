/*
 *  CURT.c
 *  Name: Omar Salah ELDIN
 *  Created on: Sep 17, 2021
 */
/*
 * Hash includes
 */
#define F_CPU 8000000UL//Work  Task on 8MHZ
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
/*
 * Function prototype
 */
uint16_t  ADC_read(uint8_t channel);
void ADC_Init(void);
void  PWM_Timer0_Init(unsigned char set_duty_cycle);
/*
 * Main function
 */
int main()
{
	uint8_t bool;//boll for the while loop in buzzer
	uint16_t tempC,duty;//Variable for temperature and duty cylce for PWM
    SREG|=(1<<7);//Enable interrupt
	DDRC=0X3F;//Make first 7 bits in PORTc OUTPUT
	PORTC=0X00;//clear PORTC
	DDRA&=~(1<<PA1);//Input from push button
	ADC_Init();
	/*
	 * while loop to check on the temperature read from ADC function then make the requirements
	 */
	while(1)
	{
		tempC=ADC_read(0);//portA 0
		if(tempC<20)
		{
			/*
			 * if the temperature less than 20 then stop the DC motor
			 */
			PORTC=0x01;
			PWM_Timer0_Init(0);//Calling PWM function and stop motor
			/*
			 * Make pin (PC3) logical {1} and clear pin (PC4)
			 */
			PORTC |= ((1<<PC3));
			PORTC &=( ~(1<<PC4));
		}
		else if((tempC>=20)&&(tempC<40))
		{
			/*
			 *if the temperature less than 20 then stop the DC motor
			 */
			PORTC=0x02;
			duty=tempC*6;//to make the motor increase gradually if the temperature increase
			PWM_Timer0_Init(duty);
			/*
			 * Make pin (PC3) logical {1} and clear pin (PC4)
			 */
			PORTC |= (1<<PC3);
			PORTC &= ~(1<<PC4);
		}
		else
		{
			PORTC=0x04;
			PWM_Timer0_Init(255);//Calling PWM function with max speed of motor
			/*
			* Make pin (PC3) logical {1} and clear pin (PC4)
			*/
			PORTC |= (1<<PC3);
			PORTC &= ~(1<<PC4);
			if(tempC>40)
			{
				/*
				 * If condition to check if the temperature more than 40 will activate the buzzer then check the button and temperature
				 * satisfy the conditions
				 */
				bool=1;//Make the while loop true unti the conditions done
				while(bool){
				PORTC|=(1<<PC5);
				_delay_ms(100);
				tempC=ADC_read(0);//portA 0
				if((PINA&(1<<1))&&(tempC<40))
				{
					bool=0;
				}
				else{}
				}
			}
		}
	}
}
/*--------------------------------------------------------------------
 * Function declaration
 *--------------------------------------------------------------------*/

uint16_t  ADC_read(uint8_t channel)
{
	ADMUX = (1 << REFS0)|(1<<ADLAR)|(1<<REFS1)|(channel<<MUX0);   // VREF=2.56V, 8-bit, channel #0 is on PA0
		_delay_us(10);              // allow multiplexer to settle
		ADCSRA |= (1<<ADSC);        // Start Conversion
		while (!(ADCSRA & (1<<ADIF))); // wait for completion
		return ADCH;                // 8-bit result because we use ADLAR
}
//--------------------------------------------------------------------//
void ADC_Init(void)
{
	 DDRA&=~(1<<PA0);//Input define first bit in port A
	/*
	 * Enable ADC and prescaler 64
	 */
	ADCSRA = (1<<ADEN)|(1<< ADPS1)|(1<<ADPS2);
}
//--------------------------------------------------------------------//
void  PWM_Timer0_Init(unsigned char set_duty_cycle)
{
	TCNT0=0;//SET INTIAL value of timer
	OCR0  = set_duty_cycle; // Set Compare Value
	DDRB  = DDRB | (1<<PB3); //set PB3/OC0 as output pin --> pin where the PWM signal is generated from MC.
	/* Configure timer control register
		 * 1. Fast PWM mode FOC0=0
		 * 2. Fast PWM Mode WGM01=1 & WGM00=1
		 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
		 * 4. clock = F_CPU/1 CS00=1
		 */
		TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS00);
}
