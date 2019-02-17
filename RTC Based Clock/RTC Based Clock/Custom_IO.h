#include <util/delay.h>
	void ex_command(int comm){
		DDRD = 255;
		DDRC = 255;
		PORTD = comm;
		PORTC = 0b00000100;
		_delay_ms(1);
		PORTC = 0;
		_delay_ms(1);
	}

	void clr_lcd(){
		DDRD = 255;
		DDRC = 255;
		PORTD = 0x01;
		PORTC = 4;
		_delay_ms(1);
		PORTC = 0;
		_delay_ms(1);
	}

	void printlcd(char c){
		DDRD = 255;
		DDRC = 255;

		PORTD = c;
		PORTC = 0b00000001; // Select Data Register
		_delay_ms(1);
		PORTC = 0b00000101;
		_delay_ms(1);
		PORTC = 0b00000001;
		_delay_ms(1);
	}
	void printlcd_str(char kj[]){
		DDRD = 255;
		DDRC = 255;
		for(int g=0;kj[g]!='\0';g++){
		printlcd(kj[g]);
		}
		}
	void print_lcd_number(int num){
		if(num <10){
			printlcd(num+48);
		}
		else if ((num <99)&(num >=10)){
			int na = num/10;
			int nb = num%10;
			printlcd(na+48);
			printlcd(nb+48);
		}
		else if ((num <999)&(num >=100)){
			int na = num/100;
			int nb = (num%100)/10;
			int nc = (num%100)%10;
			printlcd(na+48);
			printlcd(nb+48);
			printlcd(nc+48);
		}
		else if ((num <9999)&(num >=100)){
			int na = num/1000;
			int nb = (num%1000)/100;
			int nc = (((num%1000)%100)/10);
			int nd = (((num%1000)%100)%10);
			printlcd(na+48);
			printlcd(nb+48);
			printlcd(nc+48);
			printlcd(nd+48);
		}
	}
	void init_lcd(){
		DDRD = 255;
		DDRC = 255;
		ex_command(0x38);
		ex_command(0x0C);
		ex_command(0x01);
	}
	void print_sev_segment(int s,int ch){
		DDRA = 255;
		DDRB = (1<<ch);
		PORTB = (1<<ch);
		
		if(s == 0){
			PORTA = ~0b00111111; // 0
		}
		else if(s == 1){
			PORTA = ~0b00000110; // 1
		}
		else if(s == 2){
			PORTA = ~0b01011011; // 2
		}
		else if(s == 3){
			PORTA = ~0b01001111; // 3
		}
		else if(s == 4){
			PORTA = ~0b01100110; // 4
		}
		else if(s == 5){
			PORTA = ~0b01101101;  // 5
		}
		else if(s == 6){
			PORTA = ~0b01111101;  // 6
		}
		else if(s == 7){
			PORTA = ~0b00000111;  // 7
		}
		else if(s == 8){
			PORTA = ~0b01111111;  // 8
		}
		else if(s == 9){
			PORTA = ~0b01101111; // 9
		}
	}
void print_sev_segment_custom_port(int s,int ch,char port){
	DDRA = 255;
	if(port=='A'){
		DDRA = (1<<ch);
		PORTA = (1<<ch);
	}
	else if(port=='B'){
		DDRB = (1<<ch);
		PORTB = (1<<ch);
	}
	else if(port=='C'){
		DDRC = (1<<ch);
		PORTC = (1<<ch);
	}
	else if(port=='D'){
		DDRD = (1<<ch);
		PORTD = (1<<ch);
	}
	if(s == 0){
		PORTA = ~0b00111111; // 0
	}
	else if(s == 1){
		PORTA = ~0b00000110; // 1
	}
	else if(s == 2){
		PORTA = ~0b01011011; // 2
	}
	else if(s == 3){
		PORTA = ~0b01001111; // 3
	}
	else if(s == 4){
		PORTA = ~0b01100110; // 4
	}
	else if(s == 5){
		PORTA = ~0b01101101;  // 5
	}
	else if(s == 6){
		PORTA = ~0b01111101;  // 6
	}
	else if(s == 7){
		PORTA = ~0b00000111;  // 7
	}
	else if(s == 8){
		PORTA = ~0b01111111;  // 8
	}
	else if(s == 9){
		PORTA = ~0b01101111; // 9
	}
}