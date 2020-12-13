#ifndef DRAW_H
#define DRAW_H

#include <string.h>
#include <linux/fb.h>
#include "ui.h"

/***********************************************************
*                       宏定义                             *
************************************************************/
/* 判断是否在范围内的判断类型 */
#define CENTER_POSITION     0
#define BEGIN_POSITION      1


#define RGB16               16
#define RGB24               24
#define RGB32               32

#define RGB888_RED          0x00ff0000
#define RGB888_GREEN        0x0000ff00
#define RGB888_BLUE         0x000000ff
 
#define RGB565_RED          0xf800
#define RGB565_GREEN        0x07e0
#define RGB565_BLUE         0x001f

/***********************************************************
*                       宏函数                             *
************************************************************/
/* 合成颜色值 */
#define Color_ARGB8888(a, r, g, b) (DB_UINT32)((a) << 24 | (r) << 16 | (g) << 8 | (b))
#define Color_ARGB1555(a, r, g, b) (DB_UINT32)((a) << 24 | (r) << 16 | (g) << 8 | (b))
#define Color_RGB888(r, g, b)      (DB_UINT32)((r) << 16 | (g) << 8  | (b))

/***********************************************************
*                     自定义类型                            *
************************************************************/
/* fb操作句柄 */
typedef struct FbHandler
{
	struct fb_var_screeninfo m_tVar;         /* 结构体定义在/usr/include/linux/fb.h头文件里 */
	struct fb_fix_screeninfo m_tFix;
    DB_INT32 m_s32Fd;                        /* 打开的文件描述符 */
	DB_INT32 m_s32Size_x;                    /* 屏幕的实际宽高 */
    DB_INT32 m_s32Size_y;             
	DB_INT32 m_s32PixelSize;                 /* 每个像素占多少个字节 */
	DB_UINT8 *m_pu8StartAddr;                /* 显存映射到当前进程空间的地址 */
    DB_VOID (*m_pDrawPixel)(const struct FbHandler *, const DB_INT32, const DB_INT32, const DB_UINT32); /* 画点函数 */ 
    DB_UINT32 (*m_pReadPixel)(const struct FbHandler *, const DB_INT32, const DB_INT32);                /* 读点函数 */ 
}T_FbHandler;

/***********************************************************
*                      函数声明                             *
************************************************************/
DB_UINT16 RGB888ToRGB565(const DB_UINT32 _u32Color888);                                          /* RGB888颜色格式转RGB565 */
DB_UINT32 RGB565ToRGB888(const DB_UINT16 _u16Color565);                                          /* RGB565颜色格式转RGB888 */
DB_INT32 DrawLine(const T_FbHandler *_ptFbHandler, const T_Graphical *_ptGraHandler);            /* 绘制线条 */
DB_INT32 DrawFullRect(const T_FbHandler *_ptFbHandler, const DB_UINT32 _u32Begin_x,
                      const DB_UINT32 _u32Begin_y, const DB_UINT32 _u32End_x,
                      const DB_UINT32 _u32End_y, const DB_UINT32 _u32Color);                     /* 矩形填充 */
DB_INT32 DrawBmp(const T_FbHandler *_ptFbHandler, const DB_VOID *_ptPartHandler, const DB_UINT8 _u8PartType);/* 绘制BMP图片 */                                            /* 绘制bmp图片 */
T_FbHandler* FbInfoInit(T_FbHandler *_ptFbHandler, const char *_s8pFbPathName);                  /* fb初始化函数 */
DB_VOID FbInfoDeInit(T_FbHandler *_ptFbHandler);                                                 /* fb反向初始化 */
DB_VOID ClearScreen(const T_FbHandler *_ptFbHandler);                                            /* 清空屏幕 */



#endif /* DRAW_H */