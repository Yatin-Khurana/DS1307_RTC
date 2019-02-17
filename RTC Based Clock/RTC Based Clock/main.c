/*
 * RTC Based Clock.c
 *
 * Created: 29-10-2018 03:29:27 AM
 * Author : Yatin Khurana
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "Custom_IO.h"
#include <avr/interrupt.h>
#include "TWI.h"
long int OVF;
int time[12] = {1,2,3,4,5,6,7,8,9,0,1,2};
int temp = 24;
int sev_seg_pin[6] = {0,1,2,3,4,5};// Respective Channels on Port DDRB
int mode = 7;
int MODE_SW = PC2;
int UP = PC3;
int DOWN = PC4;
int OK_SW = PC5;
int counter = 0;
//--------------DS1307 Functions----------------
void update_time(){
	int s,m,h,dd,mm,yy;
	s = TWI_recv_from_location(0xD0,0);
	m = TWI_recv_from_location(0xD0,1);
	h = TWI_recv_from_location(0xD0,2);
	dd = TWI_recv_from_location(0xD0,4);
	mm = TWI_recv_from_location(0xD0,5);
	yy = TWI_recv_from_location(0xD0,6);
	// Sec value
	time[5]=s&0B00001111;
	time[4]=s>>4;
	// Min
	time[3]=m&0B00001111;
	time[2]=m>>4;
	// Hour
	time[1]=h&0B00001111;
	time[0]=h>>4;				// BCD Conversion
	// Date
	time[6]=dd&0B00001111;
	time[7]=dd>>4;
	// Month
	time[8]=mm&0B00001111;
	time[9]=mm>>4;
	// Year
	time[10]=yy&0B00001111;
	time[11]=yy>>4;				// BCD Conversion
}
void settime(){
	//Time
	TWI_start();
	TWI_tx(0xD0);
	TWI_tx(0x00);
	TWI_tx(((time[5])|(time[4]<<4)));
	TWI_tx(((time[3])|(time[2]<<4)));
	TWI_tx(((time[1])|(time[0]<<4)));
	TWI_stop();
	//Dates
	TWI_start();
	TWI_tx(0xD0);
	TWI_tx(0x04);
	TWI_tx(((time[7])|(time[6]<<4)));
	TWI_tx(((time[9])|(time[8]<<4)));
	TWI_tx(((time[11])|(time[10]<<4)));
	TWI_stop();
}
void correct_values(){
	// An Algorithm to Correct Values of Data When Setting Time.
	//---------------- Positives ----------------------
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
	// Dates
	
	//Ineffective for year value
	if((time[8]==1)&(time[9]>=3)){
		time[8]=0;
		time[9]=0;
	}
	if(time[9]>=10)
	{
		time[8]++;
		time[9]=0;
	}
	if(time[8]>=1)
	{
		time[11]++;
		time[8]=0;
		time[9]=0;
	}
	if(time[7]>=10)
	{
		time[6]++;
		time[7]=1;
	}
	if(((time[6]>=3)&&(time[7])>=1)) // Odd Months
	{
		if((time[9]==1)||(time[9]==3)||(time[9]==5)||(time[9]==9)){
			time[6] = 0;
			time[7]=1;
		}
	}
	if(((time[6]>=3)&&(time[7])>=0)) //Even Months
	{
		if(~((time[9]==1)||(time[9]==3)||(time[9]==5)||(time[9]==9)||(time[9]==2))){
			time[6] = 0;
			time[7]=1;
		}
	}
	if(((time[6]>=2)&&(time[7])>=8)) // The Pretty February !
	{
		if((time[9]==2)&&(time[8]!=1)){
			time[6] = 0;
			time[7]=1;
		}
	}
	//---------------- Negatives ----------------------
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
	// Dates
	if(time[11]<0)
	{
		time[10]--;
		time[11]=9;
	}
	if(time[10]<0)
	{
		time[10]=9;
		time[11]=9;
	}
	if((time[8]<=0)&(time[9]<=0)){
		time[8]=1;
		time[9]=2;
	}
	if(time[9]<=0)
	{
		if(time[8]==1){
		time[8]=0;
		time[9]=9;
		}
		else if(time[8]==0){
			time[9]--;
		}
	}
	if(time[7]<0)
	{
		time[6]--;
		time[7]=9;
		time[9]--;
	}
	if(((time[6]<=0)&&(time[7])<=0)) // Odd Months
	{
		if((time[9]==1)||(time[9]==3)||(time[9]==5)||(time[9]==9)){
			time[6] = 3;
			time[7]=0;
			time[9]--;
		}
	}
	if(((time[6]<=0)&&(time[7])<=0)) //Even Months
	{
		if(~((time[9]==1)||(time[9]==5)||(time[9]==9)||(time[9]==2))){
			time[6] = 3;
			time[7]=1;
			time[9]--;
		}
	}
	if(((time[6]<=0)&&(time[7])==0)) // The Pretty February !
	{
		if((time[9]==3)&&(time[8]!=1)){
			time[6] = 2;
			time[7]=8;
			time[9]--;
		}
	}
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
   TCCR1A = 0;
   TCCR1B = 2;
   TCNT1 = 0;
   TIFR = 0;
   TIMSK = 4;
   OCR1A = 0;
   OCR1B = 0;
   ICR1 = 0;
   sei();
    while (1) 
    {
			if((PINC&(1<<MODE_SW))==0)
			{
				_delay_ms(500);
				mode++;
				if((mode==6)|(mode>=8))
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
				else if(mode == 3)
				{
					time[7]++;
				}
				else if(mode == 4)
				{
					time[9]++;
				}
				else if(mode == 5)
				{
					time[11]++;
				}
				else{}
				correct_values();
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
				else if(mode == 3)
				{
					time[7]--;
				}
				else if(mode == 4)
				{
					time[9]--;
				}
				else if(mode == 5)
				{
					time[11]--;
				}
				else{}
				correct_values();
			}	
			else if((PINC&(1<<OK_SW))==(0))
			{
				if(mode!=7){
					settime();
				}
				mode = 7;
			}
			if(mode==7){
				update_time();
			}
			
			for(int r = 0;r<=5;r++)
			{
				print_sev_segment(time[r],sev_seg_pin[r]);
				_delay_us(600);
			}
			//-----Dates-----
			print_sev_segment_custom_port(time[6],PB6,'B');
			_delay_us(600);
			print_sev_segment_custom_port(time[7],PB5,'B');
			_delay_us(600);
			print_sev_segment_custom_port(time[8],PC7,'C');
			_delay_us(600);
			print_sev_segment_custom_port(time[9],PC6,'C');
			_delay_us(600);
			print_sev_segment_custom_port(time[10],PD7,'D');
			_delay_us(600);							   
			print_sev_segment_custom_port(time[11],PD6,'D');
			_delay_us(600);							   
			print_sev_segment_custom_port((temp>>4),PD5,'D');
			_delay_us(600);							   
			print_sev_segment_custom_port((temp&0x0F),PD4,'D');
			_delay_us(600);
	}
}
ISR(TIMER0_OVF_vect){
	// A Code Goes Here which blinks the Parameter that has to be set.
}