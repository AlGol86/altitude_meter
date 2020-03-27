#include "iostm8s103f3.h"
#include "bmp_180.h"
#include "oled.h"

Calibration_bmp180 Calib_data;

int main()
{
  T_P t_p;
	long int h;
	long int p0;
  char pos;
  CLK_CKDIVR=0x18;//(reset value=0x18) 2 MHz
	init_bmp180();
	get_bmp180_T_P_average(&t_p ); 
  init_ssd1306_2(oled_32_128_adr);
  oled_Clear_Screen();
  oled_blink1(0, 127, 64,1);
  oled_Clear_Screen();
  oled_print_nomber(0,t_p.P/10,10,1,GIGA);
  p0=t_p.P;
  oled_Clear_Screen();
	while (1)
        {
          get_bmp180_T_P_average(&t_p ); 
          h=altitude (t_p.P,p0, t_p.T);
          pos=30;
          if(h<0) {oled_print_giga_char('-',0,0);h=-h;}
          else oled_print_giga_char(' ',0,0);
          pos=oled_print_nomber(0,h/10,pos,0,GIGA);
          pos=oled_print_giga_char('.',pos,0);
          pos=oled_print_nomber(1,h%10,pos,0,GIGA);
          oled_print_giga_char(' ',pos,0);
          oled_print_nomber(0,t_p.P/100,10,4,GIGA);
        }
}


