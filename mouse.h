#ifndef MOUSE_H
#define MOUSE_H
#include <linux/input.h>
#include "DB_TypeDef.h"

/***********************************************************
*                       宏定义                              *
************************************************************/
#define MOUSE_SIZE  10              /* 鼠标大小(单位像素点) */ 

#define MOUSE_COLOR 0x009966FF      /* 默认鼠标颜色 */

/* 用于表示鼠标的位置有没有更新 */
#define UPDATE                          1
#define NO_UPDATE                       0

/* 按键的状态 */
#define MY_KEY_DOWN                     1
#define MY_KEY_UP                       0

/* 鼠标缩放比例 */
#define MOUSE_ZOOM_X                    82
#define MOUSE_ZOOM_Y                    109

/* event.code */
#define M_KEY_LEFT                      272           /* 鼠标左键 */
#define M_KEY_RIGHT                     273           /* 鼠标右键 */
#define M_KEY_MIDDLE                    274           /* 鼠标中键 */

/* 鼠标图形信息 */
typedef struct GarMouse
{
    DB_UINT32 m_u32Color;                               /* 鼠标颜色 */
    DB_BOOL m_bIsRestore;                               /* 绘制鼠标是否需要还原上次鼠标位置的场景 */
    DB_UINT32 m_au32OldBuffer[MOUSE_SIZE][MOUSE_SIZE];  /* 鼠标旧缓存，用于移动还原之前的图像 */
    DB_UINT32 m_au32NewBuffer[MOUSE_SIZE][MOUSE_SIZE];  /* 鼠标新缓存，移动时获取新位置的图像信息 */
}T_GarMouse;

/* 鼠标信息 */
typedef struct MouseHandler
{
    DB_INT32 m_s32Fd;                        /* 打开的文件描述符 */
    DB_INT32 m_s32OldPosition_x;             /* 鼠标的x坐标 */ 
    DB_INT32 m_s32OldPosition_y;             /* 鼠标的y坐标 */ 
    DB_INT32 m_s32NowPosition_x;             /* 鼠标的x坐标 */ 
    DB_INT32 m_s32NowPosition_y;             /* 鼠标的y坐标 */

    DB_UINT8 m_u8UpdatePosition;             /* 更新位置标志 */   
    DB_UINT8 m_u8LeftState;                  /* 鼠标左键状态 */ 
    DB_UINT8 m_u8RightState;                 /* 鼠标右键状态 */ 
    DB_UINT8 m_u8MiddleState;                /* 鼠标中键状态 */ 

    DB_UINT8 m_u8UpdateKeyState;             /* 更新按键状态标志 */
    DB_UINT8 m_u8Reserved1;                  /* 保留 */
    DB_UINT8 m_u8Reserved2;                  /* 保留 */
    DB_UINT8 m_u8Reserved3;                  /* 保留 */

    T_GarMouse *m_ptGarMouse;                /* 鼠标的指针(用在有图形界面绘制图标的情况下) */ 
}T_MouseHandler;

/***********************************************************
*                      函数声明                             *
************************************************************/
DB_INT32 MouseInit(T_MouseHandler *_ptMouseHandler, const char *_s8pMousePathName); /* 鼠标初始化函数 */
DB_VOID MouseDeInit(T_MouseHandler *_ptMouseHandler);                               /* 鼠标反初始化函数 */
DB_VOID UpdateMouseState(T_MouseHandler *_ptMouseHandler);                          /* 更新鼠标状态及坐标函数 */


#endif /* MOUSE_H */