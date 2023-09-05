#include <stc15f2k60s2.h>
#define uchar unsigned char
#define uint unsigned int 
void displaym();
sbit en=P2^5;            		//1602  6管脚
sbit rs=P2^7;	 //1602端口	4管脚
sbit rw=P2^6;//lcd1602控制端口 5管脚
sbit num1=P1^0;						
sbit num2=P1^1;								   
sbit num3=P1^2;											   												 
sbit num4=P1^3;								 //开始停止切换

sbit out=P3^4;							 
sbit out1=P3^7;			

uint zhuansu,flag,zkb,flag_1,ziran,flag_ziran;
bit kai=0;

void delay(uint z)//延时1ms函数
{
 uint x,y;
 for(x=0;x<z;x++)
     for(y=0;y<120;y++);
}


void write_com(uchar com)//向1602写一字节（控制指令）
{
  rs=0;
  P0=com;
  delay(5);
  en=0;
  delay(10);
  en=1;
}
void write_data(uchar date)//向1602写一字节（数据）
{
  rs=1;
  P0=date;
  delay(5);
  en=0;
  delay(5);
  en=1;
}
void init()//初始化函数
{ 
  en=0;
  rw=0;
  write_com(0x01);         //lcd初始化
  write_com(0x38);					   //5X7显示
  write_com(0x0c);			 //关闭光标
  TMOD=0x11;               //定时器0方式1（16位定时器）,定时器0方式0（13位定时器）           
  TH0=0xfc;
  TL0=0x18;      //1ms          
  EA=1;                    //开总中断 
  ET0=1;                   //定时器0开中断
  TR0=1; 
  EX1=1;
  IT1=1;                 //定时器启动  
  write_com(0x80);
  write_data('V');
  write_data(':');
  write_com(0x87);		 //第一行显示转速
  write_data('r');
  write_data('p');
  write_data('s');
  write_com(0xc0); 
  write_data('z');
  write_data('h');
  write_data('a');
  write_data('n');
  write_data('k');
  write_data('o');
  write_data('n');
  write_data('g');
  write_data('b');
  write_data('i');	  //在第二行显示zhankongbi:
  write_data(':');
  displaym();
}
void keyscan()  //键盘扫描函数
{ 
  if(num1==0)     
  {
    delay(5);      //消除抖动
	 if(num1==0)
	 {
		 zkb=100;
		 ziran=0;
			displaym();								
     }
  }
  if(num2==0)
  {
    delay(5);
	 if(num2==0)
	 {
		zkb=200;
		 ziran=0;
			displaym();					
	 }
  }
    if(num3==0)
  {
    delay(5);
	 if(num3==0)
	 {
		ziran=1;
		 zkb=50;
		 displaym();
	 }
  }
      if(num4==0)
  {
    delay(5);
	 if(num4==0)
	 {
	 while(num4==0)	 
	  kai=~kai;
	 }
  }
}
void display()
{
  write_com(0x82);
  zhuansu=zhuansu/2;	  //将两秒内的计数乘以30得到转每分

  if(zhuansu/10000!=0)
   write_data(zhuansu/10000+0x30);						  //如果转速的万位不为0	正常显示否则显示空格
   else
   write_data(' ');

  if(zhuansu/1000==0)
  write_data(' ');
  else 
  write_data(zhuansu%10000%1000+0x30);								//如果转速小于1000 千位为空格 否则正常显示
  
  if(zhuansu/100==0)
  write_data(' ');
  else
  write_data(zhuansu%10000%1000/100+0x30)              //如果转速小于100 百位为空格 否则正常显示

    if(zhuansu/10==0)
  write_data(' ');
  else
  write_data(zhuansu%10000%1000%100/10+0x30);				  //如果转速小于10 十位为空格 否则正常显示

  write_data(zhuansu%10000%1000%100%10+0x30);
  write_com(0xd0);			  //如果没有这句，当中断内的显示函数执行完，就会在转速的位置显示占空比数据，导致乱码
}
void displaym()
{
 write_com(0xcb);
   if(zkb/200%10!=0)
  write_data(zkb/200%10+0x30);			   //如果占空比百位不为0则显示百位否则显示空格
  else 
  write_data(' ');
  
  if(zkb/200%10==0&&zkb/20%10==0)
  write_data(' ');
  else
  write_data(zkb/20%10+0x30);				 //如果占空比小于10 十位正常显示  否则显示空格
  
  write_data(zkb/2%10+0x30);				 //显示个位
}
void main()
{
  flag_1=0;
	zkb=100;							 //占空比为100
	zhuansu=0;					   //转速初值0
	flag=0;
  init();              //初始化
	while(1)
	{
	keyscan();				//键盘扫描程序	  
	}
}

void int1()interrupt 2				//外部中断1脉冲技术记录电机的转速 电机转一圈zhuansu加一
{
  zhuansu++;
}

void int3()interrupt 1					 //产生PWM
{ 		
  TH0=0xfc;
  TL0=0x18;//定时1ms
  flag++;	
  if(flag==2000)				   //计时到达2s
  {
	display();					//显示转速
	zhuansu=0;					  //转速置0
	flag=0;
  }
	if(ziran==1)
	{
		flag_ziran++;
		if(flag_ziran==8000&& zkb==50)
		{
			zkb=100;
			displaym();
			flag_ziran=0;
		}
		if(flag_ziran==8000 && zkb==100)
		{
			zkb=150;
			displaym();
			flag_ziran=0;
		}
		if(flag_ziran==8000 && zkb==150)
		{
			zkb=200;
			displaym();
			flag_ziran=0;
		}
		if(flag_ziran==8000 && zkb==200)
		{
			zkb=50;
			displaym();
			flag_ziran=0;
		}
	}
	
  	flag_1++;
	if(flag_1>199)
	flag_1=1;
	if(kai==1)		  //如果kai==1电机启动
	{
		if(flag_1<zkb)				 //小于占空比m输出PWM=0输出电压为1
	   {
	     out=0;
		 out1=1;
	   }
	   else
	   {
	  	 out=1;
	  	 out1=1;
	   }
 	}
	 
	 if(kai==0)			  //kai=0电机停止转动
	 {
	   out=1;
	   out1=1;
	 }
 }

