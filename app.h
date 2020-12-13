#ifndef APP_H
#define APP_H

#include <pthread.h>
#include "mouse.h"
#include "draw.h"
#include "ui.h"

/***********************************************************
*                       宏定义                              *
************************************************************/
/* 用在判断鼠标图形是否画点 */
#define POINT                       0x1         /* 画点 */
#define NOPOINT                     0x0         /* 不画点 */

/* 输入相关ENTER */
#define SET_COLOR                   'c'
#define SET_WIDTH                   'w'
#define MAX_WIDTH                   50
#define ENTER                       0x0a        /* ENTER键 */
#define START                       1           /* 开始记录 */
#define NO_START                    0           /* 未开始记录 */


/* 图形默认属性 */
#define DEFAULT_COLOR               0xff00aaff
#define DEFAULT_WIDTH               5
#define DEFAULT_TYPE                CLEAN

/* 窗口尺寸 */
#define WINDOW_SIZE_X               800
#define WINDOW_SIZE_Y               600

/* 图片路径 */
#define BMP_YUAN_PATH_DEFAULT       "pic/yuan.bmp"
#define BMP_YUAN_PATH_SELECT        "pic/yuan_select.bmp"
#define BMP_LINE_PATH_DEFAULT       "pic/xiantiao.bmp"
#define BMP_LINE_PATH_SELECT        "pic/xiantiao_select.bmp"
#define BMP_RECTANGE_PATH_DEFAULT   "pic/juxing.bmp"
#define BMP_RECTANGE_PATH_SELECT    "pic/juxing_select.bmp"
#define BMP_CLEAN_PATH_DEFAULT      "pic/qingchu.bmp"
#define BMP_CLEAN_PATH_SELECT       "pic/qingchu.bmp"
#define BMP_CLOSE_PATH_DEFAULT      "pic/close.bmp"
#define BMP_CLOSE_PATH_SELECT       "pic/close_select.bmp"


/* 配置XML文件路径 */
#define MENU_XML_PATH               "xml/menu.xml"

/***********************************************************
*                      函数声明                             *
************************************************************/
DB_INT32 DrawMouse(const T_FbHandler *_ptFbHandler, T_MouseHandler *_ptMouseHander);    /* 绘制鼠标 */
DB_INT32 CleanMouse(const T_FbHandler *_ptFbHandler, T_MouseHandler *_ptMouseHander, const T_Window *_ptWindow);   /* 清除鼠标 */
DB_INT32 UpdateWindow(const T_FbHandler *_ptFbHandler, T_Window *_ptWindow,
                      T_MouseHandler *_ptMouseHander);                                  /* 更新UI界面内容 */
DB_INT32 SetWindow(const T_FbHandler *_ptFbHandler, T_Window *_ptWindow);               /* 初始化设置窗口部件成员 */
DB_INT32 RefreshWindow(const T_FbHandler *_ptFbHandler, const T_Window *_ptWindow);     /* 重绘窗口 */
DB_INT32 DarwWindow(const T_FbHandler *_ptFbHandler, T_Window *_ptWindow);              /* 根据更新绘制UI界面 */  
DB_INT32 GetTerminalInput(T_Window *_ptWindow);                                         /* 获取终端输入 */
DB_INT32 CloseWindow(T_Window *_ptWindow);                                              /* 关闭窗口 */

#endif /* APP_H */