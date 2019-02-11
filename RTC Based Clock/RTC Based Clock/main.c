/*
 * RTC Based Clock.c
 *
 * Created: 29-10-2018 03:29:27 AM
 * Author : Yatin Khurana
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <Custom_IO.h>
#include <avr/interrupt.h>
long int OVF;
 int time[6] = {1,2,3,4,5,6};
 int sev_seg_pin[6] = {0,1,2,3,4,5};// Respective Channels on Port DDRB
 int mode = 4;
 int MODE_SW = PC2;
 int UP = PC3;
 int DOWN = PC4;
 int OK_SW = PC5;
//-------------------------------I2C Functions -----------------------------------//
void TWI_start()
{
	TWCR=0b10100100;
	while((TWCR&0b10000000)==0b00000000);
}

void TWI_stop()
{
	TWCR=0b10010100;
}

void TWI_tx(char x)
{
	TWDR=x;
	TWCR=0b10000100;
	while((TWCR&0b10000000)==0b00000000);
}
char TWI_rx()
{
	TWCR=0b10000100;
	while((TWCR&0b10000000)==0b00000000);
	return (TWDR);
}
int TWI_recv_from_location(int location){
	TWI_start();
	TWI_tx(0XD0);
	TWI_tx(location);// location
	TWI_start();
	TWI_tx(0XD1);
	int s=TWI_rx();
	TWI_stop();
	return s;
}
//------------------------------------------------------------------------------------//
void update_time(){
	 int s,m,h;
	TWI_start();
	TWI_tx(0XD0);
	TWI_tx(0x00);// sec location
	TWI_start();
	TWI_tx(0XD1);
	s=TWI_rx();
	TWI_stop();

	TWI_start();
	TWI_tx(0XD0);
	TWI_tx(0x01);// Min location
	TWI_start();
	TWI_tx(0XD1);
	m=TWI_rx();
	TWI_stop();

	TWI_start();
	TWI_tx(0XD0);
	TWI_tx(0x02);// HH location
	TWI_start();
	TWI_tx(0XD1);
	h=TWI_rx();
	TWI_stop();
	
	//sec value
	time[5]=s&0B00001111;
	time[4]=s>>4;
	// min
	time[3]=m&0B00001111;
	time[2]=m>>4;
	//hour
	time[1]=h&0B00001111;
	time[0]=h>>4;				// BCD Conversion
}
void settime(){
	TWI_start();
	TWI_tx(0xD0);
	TWI_tx(0x00);
	TWI_tx(((time[5])|(time[4]<<4)));
	TWI_tx(((time[3])|(time[2]<<4)));
	TWI_tx(((time[1])|(time[0]<<4)));
	TWI_stop();
}
int main(void)
{
	//TWI Registers
   TWBR=8;
   TWCR=0b11000100;
   TWSR=0;
   DDRC = 0;
   PORTC |= 0b00000011;
   //TImer Registers
   TCNT0 = 0;
   TCCR0 = 1;
   TIFR = 0;
   TIMSK = 1;
    while (1) 
    {
			DDRD = 1<< mode;
			PORTD = 1<<mode;
			if((PINC&(1<<MODE_SW))==0)
			{
				_delay_ms(500);
				mode++;
				if((mode==3)|(mode>=5))
				{
					mode = 0;
				}
			}
			
			
			else if((PINC&(1<<UP))==0)
			{
				_delay_ms(500);
				if(mode == 0)
				{
					time[5]++;
				}
				else if(mode == 1)
				{
					time[3]++;
				}
				else if(mode == 2)
				{
					time[1]++;
				}
				else{}
					if(time[5]>=10)
					{
						time[4]++;
						time[5]=0;
					}
					if(time[4]>=6)
					{
						time[3]++;
						time[4]=0;
						time[5]=0;
					}
					if(time[3]>=10)
					{
						time[2]++;
						time[3]=0;
					}
					if(time[2]>=6)
					{
						time[1]++;
						time[3]=0;
						time[2]=0;
					}
					if(time[1]>=10)
					{
						time[0]++;
						time[1]=0;
					}
					if(((time[0]==2)&&(time[1])==5))
					{
						time[0] = 0;
						time[1]=0;
						
					}
			}
			else if((PINC&(1<<DOWN))==0)
			{
				_delay_ms(500);
				if(mode == 0)
				{
					time[5]--;
				}
				else if(mode == 1)
				{
					time[3]--;
				}
				else if(mode == 2)
				{
					time[1]--;
				}
				else{}
					if(time[5]>=10)
					{
						time[4]++;
						time[5]=0;
					}
					if(time[4]>=6)
					{
						time[3]++;
						time[4]=0;
						time[5]=0;
					}
					if(time[3]>=10)
					{
						time[2]++;
						time[3]=0;
					}
					if(time[2]>=6)
					{
						time[1]++;
						time[3]=0;
						time[2]=0;
					}
					if(time[1]>=10)
					{
						time[0]++;
						time[1]=0;
					}
					if(((time[0]==2)&&(time[1])==5))
					{
						time[0] = 0;
						time[1]=0;
						
					}
					
					//---------------------------
					if(time[5]<0)
					{
						time[4]--;
						time[5]=9;
					}
					if(time[4]<0)
					{
						time[3]--;
						time[4]=5;
						time[5]=9;
					}
					if(time[3]<0)
					{
						time[2]--;
						time[3]=9;
					}
					if(time[2]<0)
					{
						time[1]--;
						time[3]=9;
						time[2]=5;
					}
					if(time[1]<0)
					{
						time[0]--;
						time[1]=9;
					}
					if(time[0]<0)
					{
						time[0] = 0;
						time[1]=0;
						
					}
			}	
		else if((PINC&(1<<OK_SW))==(0))
		{
			if(mode!=4){
				settime();
				DDRD|=1<<3;
				PORTD|=1<<3;
			}
			mode = 4;
			
		}
		
			if(mode==4){
				update_time();
				}
			for(int r = 0;r<=5;r++)
			{
				print_sev_segment(time[r],sev_seg_pin[r]);
				_delay_us(600);
			}
	}
}