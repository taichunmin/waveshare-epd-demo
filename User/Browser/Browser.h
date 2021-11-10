
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _Browser_H
#define _Browser_H


/* Includes ------------------------------------------------------------------*/
#include "SSD1306.h"
#include "string.h"
#include "fatfs.h"
#include "Fonts.h"
#include "Key.h"
#include "GUI_Paint.h"
#include "SD_Card_APP.h"
#include "stdbool.h"
#include "rfal_rf.h"
#include "rfal_nfca.h"
#include "tim.h"
#include "demo.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#define	EPD_1IN54B			0
#define	EPD_1IN54C			1
#define	EPD_1IN54V2			2
#define	EPD_1IN54BCV2		3
#define	EPD_2IN13V2			4
#define	EPD_2IN13BC			5
#define	EPD_2IN13D			6
#define	EPD_2IN9				7
#define EPD_2IN9BC			8
#define EPD_2IN9D				9
#define EPD_4IN2				10
#define EPD_4IN2BC			11
#define EPD_7IN5				12
#define EPD_7IN5BC			13
#define EPD_7IN5V2			14
#define EPD_7IN5BCV2		15
#define EPD_2IN7				16
#define EPD_7IN5HD		  17

uint8_t	Browser_save_fileame(uint8_t * path);
void Browser_display(void);
uint8_t Start_Dawring(void);
void GREEN_LED_Turn(void);
#endif
