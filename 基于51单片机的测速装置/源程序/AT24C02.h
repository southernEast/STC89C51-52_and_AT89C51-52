//EEPROM
sbit WP =P1^5;
sbit sda=P1^7;
sbit scl=P1^6;


void delay24()
{ ;; }
void start()  //开始信号
{	
	sda=1;
	delay24();
	scl=1;
	delay24();
	sda=0;
	delay24();
}

void stop()   //停止
{
	sda=0;
	delay24();
	scl=1;
	delay24();
	sda=1;
	delay24();
}

void respons()  //应答
{
	uchar i;
	scl=1;
	delay24();
	while((sda==1)&&(i<250))i++;
	scl=0;
	delay24();
}

void initeeprom()
{
	WP=0;
	sda=1;
	delay24();
	scl=1;
	delay24();
}

void write_byte(uchar date)
{
	uchar i,temp;
	temp=date;


	for(i=0;i<8;i++)
	{
		temp=temp<<1;
		scl=0;
	    delay24();
		sda=CY;
		delay24();
		scl=1;
		delay24();
	//	scl=0;
     //   delay();
	}
	scl=0;
	delay24();
	sda=1;
	delay24();
}

uchar read_byte()
{
	uchar i,k;
	scl=0;
	delay24();
	sda=1;
	delay24();
	for(i=0;i<8;i++)
	{
		scl=1;
		delay24();	
		k=(k<<1)|sda;
		scl=0;
		delay24();	
	}

	return k;
}



void write_add(uchar address,uchar date)
{
	WP=0;
	start();
	write_byte(0xa0);
	respons();
	write_byte(address);
	respons();
	write_byte(date);
	respons();
	stop();
}

uchar read_add(uchar address)
{
	uchar date;
	WP=0;
	start();
	write_byte(0xa0);
	respons();
	write_byte(address);
	respons();
	start();
	write_byte(0xa1);
	respons();
	date=read_byte();
	stop();
	return date;
}

