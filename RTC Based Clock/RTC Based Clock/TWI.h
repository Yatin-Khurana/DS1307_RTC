// The TWI Library
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
int TWI_recv_from_location(int devid,int location){
	TWI_start();
	TWI_tx((devid|0));
	TWI_tx(location);// location
	TWI_start();
	TWI_tx((devid|1));
	int s=TWI_rx();
	TWI_stop();
	return s;
}