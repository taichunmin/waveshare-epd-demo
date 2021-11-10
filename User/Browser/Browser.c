#include "Browser.h"

DIR dir;
FILINFO fileinfo;	//File information
FRESULT res;
char filename[30][50];		//	File num,File name lenght
char *last_filename;
int browser_cursor=0,browser_paper=0;	//Browser pages
char* pstr;
char 	type[20];		//File Type
uint8_t file_sel=0;		//Selected files	
uint8_t key_flag=0;
uint8_t	Interface_flag=0;
uint8_t file_num=0;
uint8_t fail_num=0;
uint8_t size_flag=0;
extern uint8_t flag_300ms;
/* Back Flag*/
extern uint8_t doBack_flag;  
extern char KEY_BACK_Value ;//stm32f1xx_it.c
#if _USE_LFN
    char lfn[20];           
#endif
void GREEN_LED_Turn(void)
{
	if(HAL_GPIO_ReadPin(GREEN_LED_GPIO_Port,GREEN_LED_Pin))
		LED_GREEN_0;
	else
		LED_GREEN_1;
}

void Browser_paper_clean_half(void)
{	uint8_t i=0,j=0;
	for(i=0;i<128;i++)
	{
		for(j=0;j<48;j++)
		{
				ssd1306_draw_point(i,j, 0);
		}
	}
}
void Browser_paper_clean_full(void)
{	uint8_t i=0,j=0;
	for(i=0;i<128;i++)
	{
		for(j=0;j<64;j++)
		{
				ssd1306_draw_point(i,j, 0);
		}
	}
}
unsigned char	Browser_save_fileame(unsigned char* path)
{
	uint8_t num=0;
	res=f_opendir(&dir,(const TCHAR*)path);
	
		#if _USE_LFN
						fileinfo.lfname=lfn;
						fileinfo.lfsize = sizeof(lfn);
		#endif	
		
		if (res == FR_OK) 
		{	
			printf("\r\n"); 
			while(1)
			{
				res = f_readdir(&dir, &fileinfo);                   //Read a file in the directory
				if (res != FR_OK || fileinfo.fname[0] == 0) break;  //Exit if there is an error or if it reaches the end
				pstr=strrchr(*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname,'.');				
				if(pstr==NULL)continue;				//Skip without suffix names
				pstr++;
				strcpy(type,pstr);
				
				if(strcasecmp(pstr,"BMP")==0)
				{
					strcpy(filename[num],*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname);
					printf("<%d>:  ", num);//Print the path	
					printf("%s\r\n",  filename[num]);//Print file name	 
					num++;
				}
			} 
		 }
		return num;  	//Return file number
}
char browser_cursor_old = 0;
const char Model_selection[20] = {6,0,4,1,2,3,7,255,255,255,255,255};
//不想显示的直接改255
/***************************************
//数字标识的涵义，下面对应墨水屏程序的区分，和底板的墨水屏区分不一致
//修改上面数组的顺序，就可以修改选择栏的排列顺序
//注意每多加一种屏幕，下面变量ePaper_Quantity需要加1
//对应的字符串数组也需要加对应的字符串
//字符串为二维数组，不可以使用变量来保存二维数组，内存不足，在任意地方开变量数组注意内存大小
0: 2.13
1: 2.9
2: 4.2
3: 7.5
4: 2.7
5: 2.13B
6：1.54B
7：7.5HD
此表对应下面字符串以及驱动选择，如果需要修改请将下面驱动一起改
***************************************/
//每多一个屏幕加1，注意屏幕总数 = ePaper_Quantity + 1 
const int  ePaper_Quantity = 7;
const char Model_string[ePaper_Quantity+3][21] = {
	{"2.13 inch e-paper"},//0
	{"2.9 inch e-paper"},//1
	{"4.2 inch e-paper"},//2
	{"7.5 inch e-paper"},//3
	{"2.7 inch e-paper"},//4
	{"2.13 inch e-paper B"},//5
	{"1.54 inch e-paper B"},//6
	{"7.5 inch e-paper HD"},//7
	{".........."},//多留两个方便调试，防止数据出界，
	{".........."},//
};
int Sort = 1;
void Browser_display(void)
{	

	/********************Read key value******************/		
	key_flag=ReadKey();
	if(Interface_flag==0)
	{	
		/********************Display Function Options*****************/
		Browser_paper_clean_full();
		if(browser_cursor==0)
			ssd1306_display_string(0, 13,"NFC Test Demo", 12, 0);
		else 
			ssd1306_display_string(0, 13,"NFC Test Demo", 12, 1);
		if(browser_cursor==1)
			ssd1306_display_string(0, 25,"Update E-paper Demo", 12, 0);
		else 
			ssd1306_display_string(0, 25,"Update E-paper Demo", 12, 1);
		/********************Display basic interface******/
		ssd1306_display_string(0, 0,"Select Demo", 12, 1);
		ssd1306_draw_line(0,11,128,0);
		ssd1306_draw_bitmap(0, 48, c_chFont1608[95], 8, 16);
		ssd1306_display_string(10, 50,"UP", 12, 1);
		ssd1306_draw_bitmap(28, 48, c_chFont1608[96], 8, 16);
		ssd1306_display_string(38, 50,"DOWN", 12, 1);
		ssd1306_draw_bitmap(63, 52, c_chFont1608[97], 16, 8);
		ssd1306_display_string(77, 50,"BACK", 12, 1);
		ssd1306_draw_bitmap(102, 52, c_chFont1608[98], 16, 8);
		ssd1306_display_string(116, 52,"OK", 12, 1);
		ssd1306_draw_line(0,48,128,0);
		ssd1306_refresh_gram();
				/********************Key handler******/
		if(key_flag!=0)
			{		
				if(key_flag==1)					//Up
				{	browser_cursor--;
					if(browser_cursor<0)
					{		browser_cursor=1;
					}
				}
				else if(key_flag==2)					//Down
				{	browser_cursor++;
					if(browser_cursor>1)
					{
						browser_cursor=0;
					}
				}
				if(key_flag==4)
				{
					if(browser_cursor==0)
					{		
						Browser_paper_clean_full();
						ssd1306_display_string(20,24,"NFC reading test", 12, 1);
						ssd1306_draw_line(0,11,128,0);
						ssd1306_draw_bitmap(18, 52, c_chFont1608[97], 16, 8);
						ssd1306_display_string(32, 50,"BACK", 12, 1);
						ssd1306_draw_bitmap(63, 48, c_chFont1608[95], 8, 16);
						ssd1306_display_string(74, 50,"WakeUP", 12, 1);
						ssd1306_draw_line(0,48,128,0);
						ssd1306_refresh_gram();
						while(1)
						{
							rfalWorker();
							demoCycle();				//Enter NFC Test Demo
							if(doBack_flag==1)
							{
								doBack_flag=0;
								break;
							}
						}
					}
					else if(browser_cursor==1)
						Interface_flag++;							//Next interface
					browser_cursor=0;
				}
		}
	}
	else if(Interface_flag==1)
	{
		/********************Display E-paper Size Options*****************/
		Browser_paper_clean_full();
		for(int k=0;k<ePaper_Quantity;k++){
			if(browser_cursor%3 ==0 ){
				ssd1306_display_string(10, 13,Model_string[Model_selection[browser_cursor]], 12, 0);//
				//查表方式获取字符串
			}else  if(Model_selection[browser_cursor-browser_cursor%3] < ePaper_Quantity) {
				ssd1306_display_string(10, 13,Model_string[Model_selection[browser_cursor-browser_cursor%3]], 12, 1);
			}
			
			if(browser_cursor%3 ==1){
				ssd1306_display_string(10, 25,Model_string[Model_selection[browser_cursor]], 12, 0);
			}else if(Model_selection[browser_cursor-browser_cursor%3+1] < ePaper_Quantity) {
				//在最后一页不显示多余的字符串
				ssd1306_display_string(10, 25,Model_string[Model_selection[browser_cursor-browser_cursor%3+1]], 12, 1);
				//在不同的条件下获取不同的字符串，
				/*  
				位置//0    1   2
				    +0   +1  +2   对应数字%3 + 0 
						+1   +0  -1   对应数字%3 + 1
				    +2   +1  +0   对应数字%3 + 2 
				*/
			}
			
			if(browser_cursor%3 == 2)
				ssd1306_display_string(10, 37,Model_string[Model_selection[browser_cursor]], 12, 0);
			else if(Model_selection[browser_cursor-browser_cursor%3+2] < ePaper_Quantity) 
				ssd1306_display_string(10, 37,Model_string[Model_selection[browser_cursor-browser_cursor%3+2]], 12, 1);
		}

		/********************Display basic interface******/
		ssd1306_display_string(0, 0,"Select E-paper Size", 12, 1);
		ssd1306_draw_line(0,11,128,0);
		ssd1306_draw_bitmap(0, 48, c_chFont1608[95], 8, 16);
		ssd1306_display_string(10, 50,"UP", 12, 1);
		ssd1306_draw_bitmap(28, 48, c_chFont1608[96], 8, 16);
		ssd1306_display_string(38, 50,"DOWN", 12, 1);
		ssd1306_draw_bitmap(63, 52, c_chFont1608[97], 16, 8);
		ssd1306_display_string(77, 50,"BACK", 12, 1);
		ssd1306_draw_bitmap(102, 52, c_chFont1608[98], 16, 8);
		ssd1306_display_string(116, 52,"OK", 12, 1);
		ssd1306_draw_line(0,48,128,0);
		ssd1306_refresh_gram();
		/********************Key handler******/
		if(key_flag!=0)
			{		
				if(key_flag==1)					//Up
				{	
					browser_cursor--;
					if(browser_cursor<0)
					{		
						browser_cursor=ePaper_Quantity-1;
					}
				}
				else if(key_flag==2)					//Down
				{	
					browser_cursor++;
					if(browser_cursor>=ePaper_Quantity)
					{
						browser_cursor=0;
					}
				}
				else if(key_flag==3)					//Down
				{
					browser_cursor=0;
					Interface_flag--;							//Next interface
				}
				if(key_flag==4)
				{
					Interface_flag++;							//Next interface
					size_flag=Model_selection[browser_cursor];			//Selected E-paper Size
					printf("%d :%s \r\n",size_flag,Model_string[size_flag]);
					browser_cursor_old = browser_cursor;
					browser_cursor=0;
					printf("browser_cursor_old = %d\r\n",browser_cursor_old);
					Sort = 1;
				}
				printf("browser_cursor= %d\r\n",browser_cursor);
		}
	}
	else if(Interface_flag==2)
	{	
		/********************Display file*****************/	
		//排序
		//注意在刷新结束后在重新选择图片后会保存当前图片是第几个，重新选择墨水屏之后会自动跳到第几个图片
		//
		char count=0;
		char TIMP[50];
		if(Sort == 1){
			for(char i = 0;i<50;i++){
				if(filename[i][0] == Model_string[size_flag][0]){//2.13
					if(filename[i][2] == Model_string[size_flag][2] && filename[i][3] == Model_string[size_flag][3]){//2.13
						//成立
						strcpy(TIMP,filename[count]);
						strcpy(filename[count],filename[i]);
						strcpy(filename[i],TIMP);
						count++;
					}else if(filename[i][3] == Model_string[size_flag][2]){//2in1
						if(Model_string[size_flag][3] <= '9' && Model_string[size_flag][3] >= '0'){//如果是数字
							if(filename[i][4] == Model_string[size_flag][3] ){
								//成立
								strcpy(TIMP,filename[count]);
								strcpy(filename[count],filename[i]);
								strcpy(filename[i],TIMP);
								count++;
							}
						}else{//如果不是数字
							//成立
							strcpy(TIMP,filename[count]);
							strcpy(filename[count],filename[i]);
							strcpy(filename[i],TIMP);
							count++;
						}
					}
				}
			}
			Sort = 0;
		}
		
		Browser_paper_clean_full();
		file_sel=browser_paper*3+browser_cursor;
		if(browser_cursor==0)
			ssd1306_display_string(10, 13,filename[browser_paper*3], 12, 0);
		else
			ssd1306_display_string(10, 13,filename[browser_paper*3], 12, 1);
		if(browser_cursor==1)
			ssd1306_display_string(10, 25,filename[browser_paper*3+1], 12, 0);
		else
			ssd1306_display_string(10, 25,filename[browser_paper*3+1], 12, 1);
		if(browser_cursor==2)
			ssd1306_display_string(10, 37,filename[browser_paper*3+2], 12, 0);
		else
			ssd1306_display_string(10, 37,filename[browser_paper*3+2], 12, 1);
		/********************Display basic interface******/
		ssd1306_display_string(0, 0,"Select BMP Picture", 12, 1);
		ssd1306_draw_line(0,11,128,0);
		ssd1306_draw_bitmap(0, 48, c_chFont1608[95], 8, 16);
		ssd1306_display_string(10, 50,"UP", 12, 1);
		ssd1306_draw_bitmap(28, 48, c_chFont1608[96], 8, 16);
		ssd1306_display_string(38, 50,"DOWN", 12, 1);
		ssd1306_draw_bitmap(63, 52, c_chFont1608[97], 16, 8);
		ssd1306_display_string(77, 50,"BACK", 12, 1);
		ssd1306_draw_bitmap(102, 52, c_chFont1608[98], 16, 8);
		ssd1306_display_string(116, 52,"OK", 12, 1);
		ssd1306_draw_line(0,48,128,0);
		ssd1306_refresh_gram();
		/********************Key handler******/
		if(key_flag!=0){		
				if(key_flag==1){				//up
					browser_cursor--;
					if(browser_cursor<0){		
						browser_cursor=2;
						browser_paper--;	
						if(browser_paper<0){
							browser_paper=file_num/3;
							browser_cursor=file_num%3-1;
						}
					}
				}else if(key_flag==2){					//down
					browser_cursor++;
					if((browser_paper*3+browser_cursor)>=file_num)			//Last file
					{
						browser_cursor=0;
						browser_paper=0;
					}else if(browser_cursor>2){
						browser_cursor=0;
						if((browser_paper*3-1)<file_num){
							browser_paper++;
						}else{
							browser_paper=0;
						}
					}
				}else if(key_flag==3){				//back
					browser_cursor=browser_cursor_old;
					Interface_flag--;							//Last interface
					printf("browser_cursor = %d\r\n",browser_cursor);
					
				}else if(key_flag==4){
					Interface_flag++;							//Next interface
					Browser_paper_clean_full();
					ssd1306_display_string(10, 8,"Whether to go ahead and make a choice?", 12, 1);
					ssd1306_draw_bitmap(23, 52, c_chFont1608[97], 16, 8);
					ssd1306_display_string(37, 50,"BACK", 12, 1);
					ssd1306_draw_bitmap(72, 52, c_chFont1608[98], 16, 8);
					ssd1306_display_string(86, 52,"OK", 12, 1);
					ssd1306_refresh_gram();
				}
				key_flag=0;
		}
	}else if(Interface_flag==3){						//Confirmation interface
				Browser_paper_clean_full();
				ssd1306_display_string(10, 8,"Whether to go ahead and make a choice?", 12, 1);
				ssd1306_draw_bitmap(23, 52, c_chFont1608[97], 16, 8);
				ssd1306_display_string(37, 50,"BACK", 12, 1);
				ssd1306_draw_bitmap(72, 52, c_chFont1608[98], 16, 8);
				ssd1306_display_string(86, 52,"OK", 12, 1);
				ssd1306_refresh_gram();
				if(key_flag==3){
					Interface_flag--;							//Previous interface
					Browser_paper_clean_full();
					/********************Display basic interface******/
					ssd1306_draw_bitmap(0, 48, c_chFont1608[95], 8, 16);
					ssd1306_display_string(10, 50,"UP", 12, 1);
					ssd1306_draw_bitmap(28, 48, c_chFont1608[96], 8, 16);
					ssd1306_display_string(38, 50,"DOWN", 12, 1);
					ssd1306_draw_bitmap(63, 52, c_chFont1608[97], 16, 8);
					ssd1306_display_string(77, 50,"BACK", 12, 1);
					ssd1306_draw_bitmap(102, 52, c_chFont1608[98], 16, 8);
					ssd1306_display_string(116, 52,"OK", 12, 1);
					ssd1306_draw_line(0,48,128,0);
				}else if(key_flag==4){
					Interface_flag++;							//Next interface
				}
				key_flag=0;
	}
	else if(Interface_flag==4)						//Start drawing interface
	{
		  /*show sd card pic*/
    //1.Initialize the SD card
		while(1){
			if(!SDCard_Init())
			{	LED_RED_1;
				Browser_paper_clean_full();
				ssd1306_display_string(26, 16,"SD Card Error!", 12, 1);
				ssd1306_display_string(0,28,"Please press any key to try again.", 12, 1);
				ssd1306_refresh_gram();
			}
			else 
			{	LED_RED_0;
				Browser_paper_clean_full();
				ssd1306_display_string(5, 16,"Reading BMP picture    data from RAM...", 12, 1);
				ssd1306_refresh_gram();
				break;
			}
			while(!ReadKey());
		}
    //2.Read BMP pictures into RAM
		if(last_filename!=filename[file_sel]){
			printf("file_sel = %s\r\n",filename[file_sel]);
			SDCard_ReadBMP(filename[file_sel]);
			if(KEY_BACK_Value == 1){
				Interface_flag = 2;
				KEY_BACK_Value = 0; 
			}else{
				last_filename=filename[file_sel];
			}
		}else{
			HAL_Delay(200);
		}
		/* Open TIM3 Interrupt */	
		HAL_TIM_Base_Start_IT(&htim3); 
		Interface_flag++;
		Browser_paper_clean_full();
		ssd1306_display_string(5, 16,"Please approach the e-paper or press back     key to exit.", 12, 1);
		ssd1306_refresh_gram();
	}
	else if(Interface_flag==5)
	{	
	//3.Refresh the picture in RAM to e-Paper
		if(flag_300ms==1)
		{
			rfalFieldOff();
			flag_300ms=0;
			LED_GREEN_1;
		}
		else if(flag_300ms==2)
		{
			Start_Dawring();
			flag_300ms=0;
			LED_GREEN_0;
		}
		if(key_flag==3)
		{	
			LED_GREEN_0;
			Interface_flag=2;
		}
	}
}
const int length[ePaper_Quantity+1]= {16,16,100,120,121,106,100,120};
/***********************************************
							Read_RAM
***********************************************/
void Read_RAM(uint32_t i,uint8_t *l)
{
	uint8_t j;

	/******************************
		0: 2.13   16
		1: 2.9    16
		2: 4.2    100
		3: 7.5    120
		4: 2.7    121
		5: 2.13B  106
		6：1.54B  100 
		7：7.5HD  120
	一次发送数据不得大于128-3 
	*******************************/
	for(j=0;j<length[size_flag];j++)
	{
			l[3+j]=SPIRAM_RD_Byte(i*length[size_flag]+j);
	}
}
void Read_RedRAM(uint32_t i,uint8_t *l)
{
	uint8_t j;
	for(j=0;j<length[size_flag];j++)
	{
		if(size_flag  == 6){//1.54B需要翻转红色图片数据，其他屏幕在底层做了处理不需要翻转
			l[3+j]=~SPIRAM_RD_Byte(i*length[size_flag]+j+60000);
		}else {
			l[3+j]=SPIRAM_RD_Byte(i*length[size_flag]+j+60000);
		}
	}
}
void fail_interface(void)
{	
	Browser_paper_clean_full(); 
	ssd1306_display_string(26,8,"Update failed", 12, 1);
	ssd1306_display_string(0,24,"Please press any key to exit and try again", 12, 1);
	ssd1306_refresh_gram();
}

void Show_Progress(int progress)
{
		char Progress[4]={0,0,'%','\0'};
		Progress[0]=progress/10%10+'0';
		Progress[1]=progress/1%10+'0';
		ssd1306_display_string(40,16,"Progress:", 12, 1);
		ssd1306_display_string(58,32,Progress, 12, 1);
		ssd1306_refresh_gram();
}
/*******************************************************************
						1.54B 钥匙扣程序
						1.54B 不共用底板，固件固定
						特殊处理
*******************************************************************/
char Start_Dawring_1in54B(void)
{
	uint8_t step=5;
	uint8_t step_5[128] = {0xcd,0x05,100};
	uint8_t step_4[2] = {0xcd,0x04};
	uint8_t step_6[2] = {0xcd,0x06};
	uint8_t rx[20] = {0};
	uint16_t actrxlen[20],i=0,progress=0;
	
	if(size_flag == 6){
		step_5[2] = 100;
	}
	KEY_BACK_Value = 0;//中断响应
	while(1){
		if(step == 5){
			
			Browser_paper_clean_full(); 
			if(size_flag==6){						//1.54inch B 钥匙扣
					for(i=0;i<50;i++){	
						rx[0]=1;rx[1]=1;
						Read_RAM(i,step_5);
						rfalTransceiveBlockingTxRx(step_5,103,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);  // cd 05
						progress=i*100/100;
						printf("Proress: %d %%\r\n", progress);
						Show_Progress(progress);
						GREEN_LED_Turn();
						if(KEY_BACK_Value == 1){
									step = 15;//传输数据被打断
									break;
						}
					}
			 }
			if(KEY_BACK_Value == 0){
				step = 6;
				rx[0]=1;rx[1]=1;
				printf("1.54_Step6\r\n");	
			}else {
				step = 15;
			}
			 
		}else if(step == 6){
			rfalTransceiveBlockingTxRx(step_4,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);										//cd 04																					
			GREEN_LED_Turn();
			step=7;
			if(rx[0]==0&&rx[1]==0){	
				rx[0]=1;rx[1]=1;
				step=7;
				printf("1.54_Step7\r\n");	
			}else{
				fail_num++;
				if(fail_num>10){
					printf("Update failed, please press any key to exit and try again.\r\n");
					fail_interface();
					LED_RED_1;
					step=14;
					fail_num=0;
					DEV_Delay_ms(200);
				}
			}
		}else if(step == 7){
			Browser_paper_clean_full(); 
			if(size_flag==6){							//1.54inch B 钥匙扣
					for(i=0;i<50;i++){	
						rx[0]=1;rx[1]=1;
						Read_RedRAM(i,step_5);//发送红色图片数据
						rfalTransceiveBlockingTxRx(step_5,103,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);  // cd 05
						progress=i*100/100+50;
						printf("Proress: %d %%\r\n", progress);
						Show_Progress(progress);
						GREEN_LED_Turn();
						if(KEY_BACK_Value == 1){
									step = 15;//传输数据被打断
									break;
						}
					}
			 }
			if(KEY_BACK_Value == 0){
				step = 8;
				rx[0]=1;rx[1]=1;
				printf("1.54_Step8\r\n");	
			}else {
				step = 15;
			}
		}else if(step == 8){
			//发送更新指令
			rfalTransceiveBlockingTxRx(step_6,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);										//cd 06																		
			GREEN_LED_Turn();
			if(rx[0]==0&&rx[1]==0){	
				rx[0]=1;rx[1]=1;
				step=9;
				printf("1.54_Step9\r\n");	
			}else{
				fail_num++;
				if(fail_num>10)
				{
					printf("Update failed, please press any key to exit and try again.\r\n");
					fail_interface();
					LED_RED_1;
					step=14;
					fail_num=0;
					DEV_Delay_ms(200);
				}
			}
		}else if(step == 9 ){
				return 0;
		}
		else if(step == 14){
			return 1;
		}
		else if(step == 15){//被打短
			printf("step 15\r\n");
			return 2;
		}
	}
}


uint8_t Start_Dawring(void)
{	
	ReturnCode        err;
  uint8_t           devIt = 0;
  rfalNfcaSensRes   sensRes;
	uint8_t						step=0,progress=0;
	uint8_t						step0[2]={0xcd,0x0d};				
  uint8_t						step1[3]={0xcd,0x00,10};				//select e-paper type and reset e-paper  						4:2.13inch e-Paper   7:2.9inch e-Paper  10:4.2inch e-Paper  14:7.5inch e-Paper
	uint8_t						step2[2]={0xcd,0x01};						//e-paper normal mode  type：
	uint8_t						step3[2]={0xcd,0x02};						//e-paper config1
	uint8_t						step4[2]={0xcd,0x03};						//e-paper power on	
	uint8_t						step5[2]={0xcd,0x05};						//e-paper config2
	uint8_t						step6[2]={0xcd,0x06};						//EDP load to main
	uint8_t						step7[2]={0xcd,0x07};						//Data preparation
	uint8_t						step8[123]={0xcd,0x08,0x64};		//Data start command			2.13inch(0x10:Send 16 data at a time)    2.9inch(0x10:Send 16 data at a time)     4.2inch(0x64:Send 100 data at a time)  7.5inch(0x78:Send 120 data at a time)
	uint8_t						step9[2]={0xcd,0x18};					//e-paper power on
	uint8_t						step10[2]={0xcd,0x09};					//Refresh e-paper
	uint8_t						step11[2]={0xcd,0x0a};					//wait for ready
	uint8_t						step12[2]={0xcd,0x04};					//e-paper power off command
	uint8_t						step13[124]={0xcd,0x19,121};	
//	uint8_t						step13[2]={0xcd,0x0b};					//Judge whether the power supply is turned off successfully
//	uint8_t						step14[2]={0xcd,0x0c};					//The end of the transmission
	uint8_t						rx[20];
	uint16_t					actrxlen[20],i=0;
	
  rfalNfcaPollerInitialize();   /* Initialize for NFC-A */
  rfalFieldOnAndStartGT();      /* Turns the Field On if not already and start GT timer */
	KEY_BACK_Value = 0;
  err = rfalNfcaPollerTechnologyDetection( RFAL_COMPLIANCE_MODE_NFC, &sensRes );
  if(err == ERR_NONE) 
  {
    rfalNfcaListenDevice nfcaDevList[1];
    uint8_t                   devCnt;

    err = rfalNfcaPollerFullCollisionResolution( RFAL_COMPLIANCE_MODE_NFC, 1, nfcaDevList, &devCnt);

    if ( (err == ERR_NONE) && (devCnt > 0) ) 
    {
      devIt = 0;
       Browser_paper_clean_full(); 
      /* Check if it is Topaz aka T1T */
      if( nfcaDevList[devIt].type == RFAL_NFCA_T1T ) 
      {
        /********************************************/
        /* NFC-A T1T card found                     */
        /* NFCID/UID is contained in: t1tRidRes.uid */
        printf("ISO14443A/Topaz (NFC-A T1T) TAG found. UID: %s\r\n", hex2Str(nfcaDevList[devIt].ridRes.uid, RFAL_T1T_UID_LEN));	
				ssd1306_display_string(10,24,"E-paper detected", 12, 1);
				ssd1306_refresh_gram();
				
      }
      else
      {
        /*********************************************/
        /* NFC-A device found                        */
        /* NFCID/UID is contained in: nfcaDev.nfcId1 */
        printf("ISO14443A/NFC-A card found. UID: %s\r\n", hex2Str(nfcaDevList[0].nfcId1, nfcaDevList[0].nfcId1Len));
				ssd1306_display_string(15, 24,"E-paper detected.", 12, 1);
				ssd1306_refresh_gram();
      }
			HAL_Delay(1000);
			printf("size_flag = %d\r\n",size_flag);
			while(1)
		{			
				if(step==0)
				{		printf("Step0");	
						rfalTransceiveBlockingTxRx(step0,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);		//cd 0d																						
						if(HAL_GPIO_ReadPin(GREEN_LED_GPIO_Port,GREEN_LED_Pin))
							LED_GREEN_0;
						else
							LED_GREEN_1;
						if(rx[0]==0&&rx[1]==0)
						{	rx[0]=1;rx[1]=1;
							step=1;
							printf("Step1\r\n");	
						}
					else
					{
						fail_num++;
						if(fail_num>10)
						{
							printf("Update failed, please press any key to exit and try again.\r\n");
							fail_interface();
							LED_RED_1;
							step=14;
							fail_num=0;
							DEV_Delay_ms(200);
							
						}
					}
				}
				else if(step==1)
				{		Browser_paper_clean_full(); 
						//step1[2] 保存墨水屏型号
						//step8[2] 保存一次发送的字节数
						//step13[2]  保存第二次发送数据的字节数发送的字节数 
						//一般step8发黑色图像，step13发红色图像
						if(size_flag==0)					//2.13inch
						{
							step1[2]=EPD_2IN13V2;
							step8[2]=16;
							step13[2] = 0;
						}
						else if(size_flag==1)			//2.9inch
						{	
							step1[2]=EPD_2IN9;
							step8[2]=16;
							step13[2] = 0;
						}
						else if(size_flag==2)			//4.2inch
						{
							step1[2]=EPD_4IN2;
							step8[2]=100;
							step13[2] = 0;
						}
						else if(size_flag==3)			//7.5inch
						{
							step1[2]=EPD_7IN5V2;
							step8[2]=120;
							step13[2] = 0;
						}else if(size_flag==4)			//2.7inch
						{
							step1[2]=EPD_2IN7;
							step8[2]=121;
							//第一次发送空白数据，发其他数据底层不处理直接全部变成0xff
							step13[2] = 121;
							//发送第二次数据，才是真的图像数据，如果不发前面的0xff,最后面输出的图像不正常的偏黑
						}else if(size_flag==5)			//2.13inch B
						{
							step1[2]=EPD_2IN13BC;   //墨水屏编码 EPD_2IN13BC 为 5 
							step8[2]=106; //一次发送的字节长度
							step13[2] = 106;
						}else if(size_flag==7)		
						{
							step1[2]=EPD_7IN5HD;   
							step8[2]=120; //一次发送的字节长度
							step13[2] = 0;
						}
						
						if(size_flag == 6){
							rfalTransceiveBlockingTxRx(step1,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);		//cd 00   
						}
						else {
							rfalTransceiveBlockingTxRx(step1,3,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);
						}
						ssd1306_display_string(20, 24,"E-paper config", 12, 1);
						ssd1306_refresh_gram();
						if(rx[0]==0&&rx[1]==0)
						{rx[0]=1;rx[1]=1;
							step=2;
							printf("Step2\r\n");	
							fail_num=0;
							DEV_Delay_ms(100);
						}
						else
						{
							fail_num++;
							if(fail_num>10)
							{
								printf("Update failed, please press any key to exit and try again.\r\n");
								fail_interface();
								step=14;
								fail_num=0;
								DEV_Delay_ms(200);
								LED_RED_1;
							}
						}
						DEV_Delay_ms(10);
				}
				else if(step==2)
				{
						rfalTransceiveBlockingTxRx(step2,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);			//cd 01
						if(rx[0]==0&&rx[1]==0)
						{	rx[0]=1;rx[1]=1;
							step=3;
							printf("Step3\r\n");	
							fail_num=0;
							DEV_Delay_ms(100);
						}
						else
						{
							fail_num++;
							if(fail_num>50)
							{
								printf("Update failed, please press any key to exit and try again.\r\n");
								fail_interface();
								step=14;
								fail_num=0;
								DEV_Delay_ms(200);
								LED_RED_1;
							}
						}
						DEV_Delay_ms(100);
				}
				else if(step==3)
				{
						rfalTransceiveBlockingTxRx(step3,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);   //cd 02
						if(rx[0]==0&&rx[1]==0)	
						{
							rx[0]=1;rx[1]=1;
							step=4;
							printf("Step4\r\n");
							fail_num=0;
						}
						else
						{
							fail_num++;
							if(fail_num>10)
							{
								printf("Update failed, please press any key to exit and try again.\r\n");
								fail_interface();
								step=14;
								fail_num=0;
								DEV_Delay_ms(200);
								LED_RED_1;
							}
						}
					DEV_Delay_ms(200);
				}
				else if(step==4)
				{
					rfalTransceiveBlockingTxRx(step4,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);    //cd 03
					if(size_flag ==6){
						/*******************************************************************
																	1.54B 钥匙扣程序
						*******************************************************************/
						printf("Start_Dawring_1in54B\r\n");
						char t= Start_Dawring_1in54B();
							if(t == 0){
								step=11;
								//1.54B 数据传输完毕等待刷新
							}else if(t == 1){
								step=14;
								//1.54B 数据传输错误
							}else if(t ==2){
								step = 15;
							}
						/*******************************************************************
																	1.54B 钥匙扣程序 END
						*******************************************************************/
						}
					if(rx[0]==0 && rx[1]==0){
						if(KEY_BACK_Value == 0){
							fail_num=0;
							rx[0]=1;rx[1]=1;
							step=5;
							printf("Step5\r\n");
						}else {
							step = 15;
						}
					}
					else
					{
						fail_num++;
						if(fail_num>10)
						{
							printf("Update failed, please press any key to exit and try again.\r\n");
							fail_interface();
							step=14;
							fail_num=0;
							DEV_Delay_ms(200);
							LED_RED_1;
						}
					}
				}
				else if(step==5)
				{
						rfalTransceiveBlockingTxRx(step5,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);			//cd 05
							if(rx[0]==0&&rx[1]==0){
								rx[0]=1;rx[1]=1;
								step=6;
								printf("Step6\r\n")	;
								fail_num=0;
								DEV_Delay_ms(100);
							}
							else
							{
								fail_num++;
								if(fail_num>30)
								{
									printf("Update failed, please press any key to exit and try again.\r\n");
									fail_interface();
									step=14;
									fail_num=0;
									DEV_Delay_ms(200);
									LED_RED_1;
								}
							}
							DEV_Delay_ms(10);
				}
				else if(step==6)
				{
						rfalTransceiveBlockingTxRx(step6,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);       //cd 06
					if(rx[0]==0&&rx[1]==0){	
					rx[0]=1;rx[1]=1;
						step=7;
						printf("Step7\r\n");
						fail_num=0;
						DEV_Delay_ms(100);
					}
					else
					{
						fail_num++;
						if(fail_num>10)
						{
							printf("Update failed, please press any key to exit and try again.\r\n");
							fail_interface();
							step=14;
							fail_num=0;
							DEV_Delay_ms(200);
							LED_RED_1;
						}
					}
				}
				else if(step==7)
				{		Browser_paper_clean_full(); 
						rfalTransceiveBlockingTxRx(step7,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);      //cd 07
						ssd1306_display_string(5,16,"Start data transfer", 12, 1);
						ssd1306_refresh_gram();
						if(rx[0]==0&&rx[1]==0){
							rx[0]=1;rx[1]=1;
							step=8;
							printf("Step8\r\n");
							fail_num=0;
						}
						else
						{
							fail_num++;
							if(fail_num>10)
							{
								printf("Update failed, please press any key to exit and try again.\r\n");
								fail_interface();
								step=14;
								fail_num=0;
								DEV_Delay_ms(200);
								LED_RED_1;
							}
						}
				}
				else if(step==8)																																								//cd 08
				{		
					Browser_paper_clean_full(); 
						if(size_flag==0)							//2.13inch
					 {
							for(i=0;i<250;i++)
							{	rx[0]=1;rx[1]=1;
								Read_RAM(i,step8);
								rfalTransceiveBlockingTxRx(step8,19,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);
								progress=i*100/250;
								printf("Proress: %d %%\r\n", progress);
								Show_Progress(progress);
								GREEN_LED_Turn();
								if(KEY_BACK_Value == 1){
									step = 15;
									break;
								}
							}
					 }
					 else if(size_flag==1)
					 {
						 for(i=0;i<296;i++)
							{	rx[0]=1;rx[1]=1;
								Read_RAM(i,step8);
								rfalTransceiveBlockingTxRx(step8,19,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);
								progress=i*100/296;
								printf("Proress: %d %%\r\n", progress);
								Show_Progress(progress);
								GREEN_LED_Turn();
								if(KEY_BACK_Value == 1){
									step = 15;
									break;
								}
							}
					 }
					 else if(size_flag==2)					//4.2inch
						{
							for(i=0;i<150;i++)
							{	rx[0]=1;rx[1]=1;
								Read_RAM(i,step8);
								rfalTransceiveBlockingTxRx(step8,103,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);
								progress=i*100/150;
								printf("Proress: %d %%\r\n", progress);
								Show_Progress(progress);
								GREEN_LED_Turn();
								if(KEY_BACK_Value == 1){
									step = 15;
									break;
								}
							}
						}
						else if(size_flag==3)			//7.5inch
						{
								for(i=0;i<400;i++)
							{	rx[0]=1;rx[1]=1;
								Read_RAM(i,step8);
								rfalTransceiveBlockingTxRx(step8,123,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);
								progress=i*100/400;
								printf("Proress: %d %%\r\n", progress);
								Show_Progress(progress);
								GREEN_LED_Turn();
								HAL_Delay(6);
								if(KEY_BACK_Value == 1){
									step = 15;
									break;
								}
							}
						}else if(size_flag==5)			//2.13inch B
						{
								for(i=0;i<26;i++)
							{	rx[0]=1;rx[1]=1;
								Read_RAM(i,step8);
								rfalTransceiveBlockingTxRx(step8,109,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);
								progress=i*50/26;
								printf("Proress: %d %%\r\n", progress);
								Show_Progress(progress);
								GREEN_LED_Turn();
								if(KEY_BACK_Value == 1){
									step = 15;
									break;
								}
							}
						}else if(size_flag==7)			//7.5HD
						{
							
							for(i=0;i<484;i++)
							{	rx[0]=1;rx[1]=1;
								Read_RAM(i,step8);
								//memset(&step8[3], 0xf0, 120);
								rfalTransceiveBlockingTxRx(step8,123,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);
								progress=i*100/484;
								printf("Proress: %d %%\r\n", progress);
								Show_Progress(progress);
								GREEN_LED_Turn();//
								if(KEY_BACK_Value == 1){
									step = 15;
									break;
								}
							}
							memset(&step8[3], 0xff, 120);
							rfalTransceiveBlockingTxRx(step8,110+3,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);
							
							
						}else if(size_flag==4){			//2.7inch
							for(i=0;i<48;i++){	
								rx[0]=1;rx[1]=1;
								//Read_RAM(i,step8);
								memset(&step8[3], 0xFF, 121);
								rfalTransceiveBlockingTxRx(step8,124,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);
								progress=i*50/48;
								printf("Proress: %d %%\r\n", progress);
								Show_Progress(progress);
								GREEN_LED_Turn();//
								if(KEY_BACK_Value == 1){
									step = 15;
									break;
								}
							}
						}
						if(KEY_BACK_Value == 0){
							rx[0]=1;rx[1]=1;
							step=9;
							printf("Step9\r\n");
						}else {
							step = 15;
						}
				}
				else if(step==9)
				{		
					if(size_flag==0 || size_flag==1 || size_flag==2 || size_flag==3 || size_flag ==7){
						rfalTransceiveBlockingTxRx(step9,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);//cd 18
						//黑白屏发送底板那边也屏蔽了，无效果。   2.7除外
						if(rx[0]!=0||rx[1]!=0)
						{
								fail_num++;
								if(fail_num>10)
								{
									printf("Update failed, please press any key to exit and try again.\r\n");
									fail_interface();
									step=14;
									fail_num=0;
									DEV_Delay_ms(500);
									LED_RED_1;
								}
							}
							else
								fail_num=0;
							
						rx[0]=1;rx[1]=1;
						step=10;
						printf("Step10\r\n");
					}else if(size_flag == 5 ||  size_flag == 4){
						rfalTransceiveBlockingTxRx(step9,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);	//cd 18
						//rx[0]=1;rx[1]=1;
						step=19;
						printf("Step19\r\n");
					
					}
				}
				else if(step==10)
				{
						rfalTransceiveBlockingTxRx(step10,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);//cd 09 刷新指令
						if(rx[0]!=0||rx[1]!=0)
							{
								fail_num++;
								if(fail_num>10)
								{
									printf("Update failed, please press any key to exit and try again.\r\n");
									fail_interface();
									step=14;
									fail_num=0;
									DEV_Delay_ms(500);
									LED_RED_1;
								}
							}
							else
								fail_num=0;
						rx[0]=1;rx[1]=1;
						step=11;
						printf("Step11\r\n");
					DEV_Delay_ms(200);
				}
				else if(step==11)
				{		
						if(size_flag == 5 || size_flag == 6){//黑白红屏幕刷新时间比较长先等待
								DEV_Delay_ms(10000);
						}else if(size_flag == 7){
							DEV_Delay_ms(1000);
						}
						while(1)
						{	rx[0]=1;rx[1]=1;
							if(size_flag == 6){
								rfalTransceiveBlockingTxRx(step8,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);
								//1.54B发送0xcd 0x08
							}else{
								rfalTransceiveBlockingTxRx(step11,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);										//cd 0a
							}
							Browser_paper_clean_full(); 
							if(rx[0]==0xff&&rx[1]==0)
							{
								printf("E-paper Reflash OK\r\n");
								ssd1306_display_string(10,16,"E-paper Reflash OK", 12, 1);
								ssd1306_refresh_gram();
								fail_num=0;
								step=12;
								printf("Step12\r\n");
								DEV_Delay_ms(200);
								break;
							}
							else
							{
								if(fail_num>70)
								{
									printf("Update failed, please press any key to exit and try again.\r\n");
									fail_interface();
									step=14;
									fail_num=0;
									DEV_Delay_ms(500);
									LED_RED_1;
									break;
								}
								else
								{	fail_num++;
									printf("E-paper Reflashing, Waiting\r\n");
									DEV_Delay_ms(100);
								}
							}			
						}
					
				}
				else if(step==12)
				{
						rfalTransceiveBlockingTxRx(step12,2,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);										//cd 04
						rx[0]=1;rx[1]=1;
						step=13;
						printf("Step13\r\n");
						DEV_Delay_ms(200);
				}
				else if(step==13)
				{		
						rx[0]=1;rx[1]=1;
						step=14;
						printf("Step13\r\n");
						printf("E-paper UPdate OK\r\n");
						printf("Press any key to exit\r\n");
						Browser_paper_clean_full(); 
						ssd1306_display_string(0,32,"Press any key to exit", 12, 1);
						ssd1306_display_string(10,16,"E-paper UPdate OK", 12, 1);
						ssd1306_refresh_gram();
						DEV_Delay_ms(200);
				}
				else if(step==14)
				{	
					rfalFieldOff();
					key_flag=ReadKey();
					if(key_flag!=0)
					{
						LED_RED_0;
						LED_GREEN_0;
						step=0;
						Interface_flag=2;
						return 1;
					}
					
				}else if(step==15)
				{	
					printf("step 14\r\n");
					rfalFieldOff();
					Browser_paper_clean_full(); 
					ssd1306_display_string(0,32,"Refresh interrupted", 12, 1);
					ssd1306_display_string(10,16,"  End with", 12, 1);
					ssd1306_refresh_gram();
					DEV_Delay_ms(1000);
						LED_RED_0;
						LED_GREEN_0;
						step=0;
						Interface_flag=2;
						return 1;
				}
				else if(step==19)
				{	
									
				
					if(size_flag==4 ){
						for(i=0;i<48;i++){	
							rx[0]=1;rx[1]=1; 
							Read_RAM(i,step13);
							rfalTransceiveBlockingTxRx(step13,124,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);	//CD 19
							progress=i*50/48+50;
							printf("Proress: %d %%\r\n", progress);
							Show_Progress(progress);
							GREEN_LED_Turn();//绿灯闪烁
						}
					}else if(size_flag==5){
						for(i=0;i<26;i++){	
							rx[0]=1;rx[1]=1;
							Read_RedRAM(i,step13);
							//memset(&step13[3], 0xfE, 106);
							rfalTransceiveBlockingTxRx(step13,109,rx,20,actrxlen,RFAL_TXRX_FLAGS_DEFAULT,2157+2048);
							progress=i*50/26+50;
							printf("Proress: %d %%\r\n", progress);
							Show_Progress(progress);
							GREEN_LED_Turn();//绿灯闪烁
						}
					}
					rx[0]=1;rx[1]=1;
					step=10;
					printf("Step10\r\n");
				}
				
				if(step==14||HAL_GPIO_ReadPin(GREEN_LED_GPIO_Port,GREEN_LED_Pin))
						LED_GREEN_0;
					else
						LED_GREEN_1;
			}
    }
  }
	//printf("END\r\n");
	return 0;
}
