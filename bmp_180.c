#include "iostm8s103f3.h"
#include "i2c.h"
#include "bmp_180.h"


void del(unsigned  int del)//1=6us
{ while(del!=0){del--;}
}

void init_bmp180(void)
{
extern Calibration_bmp180 Calib_data;
char i;
int* pointer_calib;
pointer_calib= (int*)&Calib_data;
for(i=cAC1_m;i<=cMD_l;i+=2) 
{
 if(start_iic (BMP_addr, WRITE))
 {asm("nop");}
 send_byte (i);
 stop_iic ();

start_iic (BMP_addr, READ);
*pointer_calib = receive_byte (1);
*pointer_calib<<=8;
*pointer_calib += receive_byte (0);
pointer_calib++;
stop_iic ();

}
}

long int get_bmp180_ADC(char cmd, unsigned  int delay)
 {
 long int adc;
 start_iic (BMP_addr, WRITE);
 send_byte (0xf4);
 send_byte (cmd);
 stop_iic ();
 del(delay);
 start_iic (BMP_addr, WRITE);
 send_byte (0xf6);
 stop_iic ();
 start_iic (BMP_addr, READ);
 adc = receive_byte (1);
 adc<<=8;
 adc+= receive_byte (1);
 adc<<=8;
 adc+= receive_byte (0);
 stop_iic ();
 return (adc); 
 } 
	 
void get_bmp180_T_P ( T_P* tp ) 
	{ 
 extern Calibration_bmp180 Calib_data;
 long int x1,x2,x3,b3,b5,b6;
 unsigned long int b4,b7;
 long int UT=0,UP=0;

 UT=(get_bmp180_ADC(0x2e,2000))>>8;
 UP=(get_bmp180_ADC(0xf4,15000))>>5;

 x1=UT-Calib_data.AC6;
 x1*=Calib_data.AC5;
 x1/=32768;
 x2=(long int)Calib_data.MC*2048;
 x2/=(x1+Calib_data.MD);
 
 b5=x1+x2;
 tp->T=(b5+8)>>4;
 
 b6=b5-4000;
 x1=(Calib_data.B2*(b6*b6/4096))/2048;
 x2=(Calib_data.AC2*b6)/2048;
 x3=x1+x2;
 b3=((((long int)Calib_data.AC1*4+x3)<<3)+2)/4;
 x1=(Calib_data.AC3*b6)/8192;
 x2=(Calib_data.B1*((b6*b6)/4096))/65536;
 x3=((x1+x2)+2)/4;
 b4=(Calib_data.AC4*(long int)(x3+32768))/32768;
 b7=((long int)UP-b3)*(50000>>3);
 if(b7<0x80000000) tp->P=(b7*2)/b4;
 else tp->P=(b7/b4)*2;
 x1=((tp->P)/256)*((tp->P)/256);
 x1=(x1*3038)/65536;
 x2=((-7357)*(tp->P))/65536;
 tp->P=(tp->P)+(x1+x2+3791)/16;
 /////////////////////////////////
 tp->P*=75;
 tp->P/=100;
 }
 
void get_bmp180_T_P_average ( T_P* tp )
{
 char i;
 T_P t_p;
 long int sum_P=0,sum_T=0;
 for(i=0;i<40;i++)
  {
  get_bmp180_T_P(&t_p );
  sum_P+=t_p.P;
	sum_T+=t_p.T;
  }
  t_p.P=sum_P/40;
	t_p.T=sum_T/40;
	tp->P=t_p.P;
	tp->T=t_p.T;
 }

long int altitude (long int p,long int p0, int t)
{
	signed long int a,b,c,d;
	a=((p0-p)*(p0-p)/10*77)/10;
	b=(p0-p)*114820;
	c=1000+(t/10-25)*3;
	d=(((a+b)/1000)*c)/100000;
	return (d);
}
