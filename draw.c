#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include "draw.h"


/***********************************************************
*                      函数原型                             *
************************************************************/

/**********************************************************************
* Function:         DrawPoint_32(内联)
* Description:      绘制像素点函数
* Input:            @_ptFbHandle        屏幕信息句柄
*                   @_s32Piexl_x         绘制点x坐标
*                   @_s32Piexl_y         绘制点y坐标
*                   @_u32color           32位颜色值
* Attention:        此内联函数没有做入口参数监测 需要在调用此函数之前做出去参数合法性的检测
* Return:           无
**********************************************************************/
static inline DB_VOID DrawPoint_32(const T_FbHandler *_ptFbHandler, const DB_INT32 _s32Piexl_x, const DB_INT32 _s32Piexl_y, const DB_UINT32 _u32color)
{
    *(DB_UINT32 *)(_ptFbHandler->m_pu8StartAddr + _s32Piexl_x * _ptFbHandler->m_s32PixelSize  + _s32Piexl_y * _ptFbHandler->m_tFix.line_length) = _u32color;
}

/**********************************************************************
* Function:         DrawPoint_16(内联)
* Description:      绘制像素点函数
* Input:            @_ptFbHandle        屏幕信息句柄
*                   @_s32Piexl_x         绘制点x坐标
*                   @_s32Piexl_y         绘制点y坐标
*                   @_u32color           16位颜色值
* Attention:        此内联函数没有做入口参数监测 需要在调用此函数之前做出去参数合法性的检测
*                   为了适配_ptFbHandle中的函数指针，_u32color为32位，但实际只读低16位即可
* Return:           无
**********************************************************************/
static inline DB_VOID DrawPoint_16(const T_FbHandler *_ptFbHandler, const DB_INT32 _s32Piexl_x, const DB_INT32 _s32Piexl_y, const DB_UINT32 _u32color)
{
    *(DB_UINT16 *)(_ptFbHandler->m_pu8StartAddr + _s32Piexl_x * _ptFbHandler->m_s32PixelSize  + _s32Piexl_y * _ptFbHandler->m_tFix.line_length) = (DB_UINT16)_u32color;
}

/**********************************************************************
* Function:         ReadPoint_32(内联)
* Description:      读取像素点颜色
* Input:            @_ptFbHandle        屏幕信息句柄
*                   @_s32Piexl_x         绘制点x坐标
*                   @_s32Piexl_y         绘制点y坐标
* Attention:        此内联函数没有做入口参数监测 需要在调用此函数之前做出去参数合法性的检测
* Return:           无
**********************************************************************/
static inline DB_UINT32 ReadPoint_32(const T_FbHandler *_ptFbHandler, const DB_INT32 _s32Piexl_x, const DB_INT32 _s32Piexl_y)
{
    return *(DB_UINT32 *)(_ptFbHandler->m_pu8StartAddr + _s32Piexl_x * _ptFbHandler->m_s32PixelSize  + _s32Piexl_y * _ptFbHandler->m_tFix.line_length);
}

/**********************************************************************
* Function:         ReadPoint_16(内联)
* Description:      读取像素点颜色
* Input:            @_ptFbHandle        屏幕信息句柄
*                   @_s32Piexl_x         绘制点x坐标
*                   @_s32Piexl_y         绘制点y坐标
* Attention:        此内联函数没有做入口参数监测 需要在调用此函数之前做出去参数合法性的检测
*                   为了适配_ptFbHandle中的函数指针，返回值为32位，但实际读到的是16位，在使用中，对获取的值进行强转为16位 舍弃高16位即可
* Return:           无
**********************************************************************/
static inline DB_UINT32 ReadPoint_16(const T_FbHandler *_ptFbHandler, const DB_INT32 _s32Piexl_x, const DB_INT32 _s32Piexl_y)
{
    DB_UINT16 u16Color = *(DB_UINT16 *)(_ptFbHandler->m_pu8StartAddr + _s32Piexl_x * _ptFbHandler->m_s32PixelSize  + _s32Piexl_y * _ptFbHandler->m_tFix.line_length);
    return (DB_UINT32)u16Color;
}

/**********************************************************************
* Function:         RGB888ToRGB565
* Description:      RGB888颜色格式转RGB565
* Input:            @_u32Color888        RGB888颜色值
* Return:           RGB565颜色值
***********************************************************************/
DB_UINT16 RGB888ToRGB565(const DB_UINT32 _u32Color888)
{
    DB_UINT16 u16Color565 = 0;

    /* 1.获取RGB单色，并截取高位 */
    DB_UINT8 u8Red   = (_u32Color888 & RGB888_RED)   >> 19;
    DB_UINT8 u8Green = (_u32Color888 & RGB888_GREEN) >> 10;
    DB_UINT8 u8Blue  = (_u32Color888 & RGB888_BLUE)  >> 3;

    /* 2.连接 */
    u16Color565 = (u8Red << 11) + (u8Green << 5) + (u8Blue << 0);

    /* 3.返回 */
    return u16Color565;
}

/**********************************************************************
* Function:         RGB565ToRGB888
* Description:      RGB565颜色格式转RGB888
* Input:            @_u16Color565        RGB565颜色值
* Return:           RGB888颜色值
***********************************************************************/
DB_UINT32 RGB565ToRGB888(const DB_UINT16 _u16Color565)
{
    DB_UINT32 u32Color888 = 0;

    /* 1.获取RGB单色，并填充低位 */
    DB_UINT8 u8Red   = (_u16Color565 & RGB565_RED)    >> 8;
    DB_UINT8 u8Green = (_u16Color565 & RGB565_GREEN)  >> 3;
    DB_UINT8 u8Blue  = (_u16Color565 & RGB565_BLUE)   << 3;

    /* 2.连接 */
    u32Color888 = (u8Red << 16) + (u8Green << 8) + (u8Blue << 0);

    /* 3.返回 */
    return u32Color888;
}

/**********************************************************************
* Function:         IsOnDrawingBoard
* Description:      判断图形坐标是否在画板范围内
* Input:            @_u32JudgeType       判断类型，判断圆心还是判断起始坐标
*                   @_ptGraHandler       图形属性句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 IsOnDrawingBoard(const DB_UINT32 _u32JudgeType, const T_Graphical *_ptGraHandler)
{
    /* 1.入口参数判断 */
    if (_ptGraHandler == DB_NULL)
    {
        printf("IsOnDrawingBoard: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.判断需要判断的坐标类型，看是判断圆心还是判断起始坐标 */
    if (_u32JudgeType == CENTER_POSITION)
    {
        /* 绘图坐标有效性判断 看是否在画板范围内 */
        if (_ptGraHandler->m_u32Center_x >= _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardBegin_x ||
            _ptGraHandler->m_u32Center_x <= _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardEnd_x ||
            _ptGraHandler->m_u32Center_y >= _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardBegin_y ||
            _ptGraHandler->m_u32Center_y <= _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardEnd_y)
        {
            return DB_OK;
        }
        else
            return DB_ERR;
    }
    if (_u32JudgeType == BEGIN_POSITION)
    {
        /* 绘图坐标有效性判断 看是否在画板范围内 */
        if (_ptGraHandler->m_u32Begin_x >= _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardBegin_x ||
            _ptGraHandler->m_u32Begin_x <= _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardEnd_x ||
            _ptGraHandler->m_u32Begin_y >= _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardBegin_y ||
            _ptGraHandler->m_u32Begin_y <= _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardEnd_y)
        {
            return DB_OK;
        }
        else
            return DB_ERR;
    }

    return DB_ERR;
}

/**********************************************************************
* Function:         IsOnFb
* Description:      判断图形坐标是否在fb范围内
* Input:            @_u32JudgeType       判断类型，判断圆心还是判断起始坐标
*                   @_ptGraHandler       图形属性句柄
*                   @_ptFbHandler        屏幕信息句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 IsOnFb(const DB_UINT32 _u32JudgeType, const T_Graphical *_ptGraHandler, const T_FbHandler *_ptFbHandler)
{
    /* 1.入口参数判断 */
    if (_ptGraHandler == DB_NULL)
    {
        printf("IsOnDrawingBoard: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.判断需要判断的坐标类型，看是判断圆心还是判断起始坐标 */
    if (_u32JudgeType == CENTER_POSITION)
    {
        /* 绘图坐标有效性判断 看是否在fb范围内 */
        if (_ptGraHandler->m_u32Center_x >= 0 || _ptGraHandler->m_u32Center_x < _ptFbHandler->m_s32Size_x ||
            _ptGraHandler->m_u32Center_y >= 0 || _ptGraHandler->m_u32Center_y < _ptFbHandler->m_s32Size_y)
        {
            return DB_OK;
        }
        else
            return DB_ERR;
    }
    if (_u32JudgeType == BEGIN_POSITION)
    {
        /* 绘图坐标有效性判断 看是否在fb范围内 */
        if (_ptGraHandler->m_u32Begin_x >= 0 || _ptGraHandler->m_u32Begin_x < _ptFbHandler->m_s32Size_x ||
            _ptGraHandler->m_u32Begin_y >= 0 || _ptGraHandler->m_u32Begin_y < _ptFbHandler->m_s32Size_y)
        {
            return DB_OK;
        }
        else
            return DB_ERR;
    }

    return DB_ERR;
}

/**********************************************************************
* Function:         DrawFullCircle
* Description:      绘制圆点函数
* Input:            @_ptFbHandler        屏幕信息句柄
*                   @_ptGraHandler       图形属性句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 DrawFullCircle(const T_FbHandler *_ptFbHandler, const T_Graphical *_ptGraHandler)
{
    /* 1.入口参数合法性判断 */
    if (_ptFbHandler == DB_NULL || _ptGraHandler == DB_NULL)
    {
        printf("DrawFullCircle: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    /* 绘图限制变量 */
    DB_INT32 s32LimitBegin_x;
    DB_INT32 s32LimitBegin_y;
    DB_INT32 s32LimitEnd_x;
    DB_INT32 s32LimitEnd_y;
    /* 遍历变量 */
    DB_INT32 s32Min_x;          /* x坐标最小值 */
    DB_INT32 s32Min_y;          /* y坐标最小值 */
    DB_INT32 s32Pixel_x = 0;    /* x坐标遍历变量 */
    DB_INT32 s32Piexl_y = 0;    /* y坐标遍历变量 */

    /* 3.判断绘制在fb还是绘制在窗口的画板上 */
    if (_ptGraHandler->m_ptDarwingBoard != DB_NULL) /* 图形绘制在画板上 */
    {
        /* a.判断圆心坐标是否在画板范围中 */
        if (IsOnDrawingBoard(CENTER_POSITION, _ptGraHandler) == DB_ERR)
        {
            /* 如果不在做错误返回 */
            printf("DrawFullCircle: Invalid Circle position\n");
            return DB_ERR;
        }

        /* b.给限制变量赋值 */
        s32LimitBegin_x = _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardBegin_x;
        s32LimitBegin_y = _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardBegin_y;
        s32LimitEnd_x = _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardEnd_x;
        s32LimitEnd_y = _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardEnd_y;

        /* c.遍历变量赋初值 */
        s32Min_x = ((_ptGraHandler->m_u32Center_x - s32LimitBegin_x) > _ptGraHandler->m_u32Radius) ?
                    (_ptGraHandler->m_u32Center_x - _ptGraHandler->m_u32Radius) : s32LimitBegin_x;     /* x坐标最小值 */
        s32Min_y = ((_ptGraHandler->m_u32Center_y - s32LimitBegin_y) > _ptGraHandler->m_u32Radius) ?
                    (_ptGraHandler->m_u32Center_y - _ptGraHandler->m_u32Radius) : s32LimitBegin_y;     /* y坐标最小值 */
    }
    else/* 图形直接绘制在fb上 */
    {
        /* a.判断圆心坐标是否在fb范围中 */
        if (IsOnFb(CENTER_POSITION, _ptGraHandler, _ptFbHandler) == DB_ERR)
        {
            /* 如果不在做错误返回 */
            printf("DrawFullCircle: Invalid Circle position\n");
            return DB_ERR;
        }

        /* b.给限制变量赋值 */
        s32LimitBegin_x = 0;
        s32LimitBegin_y = 0;
        s32LimitEnd_x = _ptFbHandler->m_s32Size_x - 1;
        s32LimitEnd_y = _ptFbHandler->m_s32Size_x - 1;

        /* c.遍历变量赋初值 */
        s32Min_x = (_ptGraHandler->m_u32Center_x > _ptGraHandler->m_u32Radius) ?
                   (_ptGraHandler->m_u32Center_x - _ptGraHandler->m_u32Radius) : 0;        /* x坐标最小值 */
        s32Min_y = (_ptGraHandler->m_u32Center_y > _ptGraHandler->m_u32Radius) ?
                   (_ptGraHandler->m_u32Center_y - _ptGraHandler->m_u32Radius) : 0;        /* y坐标最小值 */
    }

    /* 4.绘图 */
    /* 从圆的最左边x坐标开始遍历,直到圆的最右边 */
    for (s32Pixel_x = s32Min_x; (s32Pixel_x <= _ptGraHandler->m_u32Center_x + _ptGraHandler->m_u32Radius) && (s32Pixel_x <= s32LimitEnd_x); s32Pixel_x++)
    {
        /* 从圆的最下面y坐标开始遍历，直到圆的最上面 */
        for (s32Piexl_y = s32Min_y; (s32Piexl_y <= _ptGraHandler->m_u32Center_y + _ptGraHandler->m_u32Radius) && (s32Piexl_y <= s32LimitEnd_y); s32Piexl_y++)
        {
            /* 获取满足圆条件的点 */
            if ((s32Pixel_x - _ptGraHandler->m_u32Center_x) * (s32Pixel_x - _ptGraHandler->m_u32Center_x) +
                (s32Piexl_y - _ptGraHandler->m_u32Center_y) * (s32Piexl_y - _ptGraHandler->m_u32Center_y) <= _ptGraHandler->m_u32Radius * _ptGraHandler->m_u32Radius)
            {
                /* 获取满足画板显示的点 */
                _ptFbHandler->m_pDrawPixel(_ptFbHandler, s32Pixel_x, s32Piexl_y, _ptGraHandler->m_u32Color_32);/* 画点函数 */
            }
        }
    }

    return DB_OK;
}

/**********************************************************************
* Function:         DrawLine
* Description:      DDA算法画线函数
* Input:            @_ptFbHandler        屏幕信息句柄
*                   @_ptGraHandler       图形属性句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 DrawLine(const T_FbHandler *_ptFbHandler, const T_Graphical *_ptGraHandler)
{
    /* 1.入口参数合法性判断 */
    if (_ptFbHandler == DB_NULL || _ptGraHandler == DB_NULL)
    {
        printf("DrawLine: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    /* 绘图限制变量 */
    DB_INT32 s32LimitBegin_x;
    DB_INT32 s32LimitBegin_y;
    DB_INT32 s32LimitEnd_x;
    DB_INT32 s32LimitEnd_y;
    /* 创建局部图形属性变量 给画圆点用 */
    T_Graphical tGraValTemp = *_ptGraHandler;
    /* 设置图形线条宽度  调用画圆点函数来实现,故该处的宽度应除以2的到画圆点的半径 */
    tGraValTemp.m_u32Radius = _ptGraHandler->m_u8Width / 2;
    /* 遍历变量 */
    DB_INT32 s32i     = 0;      /* 遍历变量 */
    DB_FLOAT flength  = 0;      /* 步进总长度 */
    DB_FLOAT fAdd_x   = 0;      /* x方向步进增加量 */
    DB_FLOAT fAdd_y   = 0;      /* y方向步进增加量 */
    DB_FLOAT fPixel_x = 0;      /* 像素点x坐标值 */
    DB_FLOAT fPixel_y = 0;      /* 像素点y坐标值 */

    /* 3.判断绘制在fb还是绘制在窗口的画板上 */
    if (_ptGraHandler->m_ptDarwingBoard != DB_NULL) /* 图形绘制在画板上 */
    {
        /* a.判断起始坐标是否在画板范围中 */
        if (IsOnDrawingBoard(BEGIN_POSITION, _ptGraHandler) == DB_ERR)
        {
            /* 如果不在做错误返回 */
            printf("DrawLine: Invalid Begin position\n");
            return DB_ERR;
        }

        /* b.给局部变量赋值 */
        s32LimitBegin_x = _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardBegin_x;
        s32LimitBegin_y = _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardBegin_y;
        s32LimitEnd_x = _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardEnd_x;
        s32LimitEnd_y = _ptGraHandler->m_ptDarwingBoard->m_u32DrawingBoardEnd_y;
    }
    else/* 图形直接绘制在fb上 */
    {
        /* a.判断圆心坐标是否在fb范围中 */
        if (IsOnFb(CENTER_POSITION, _ptGraHandler, _ptFbHandler) == DB_ERR)
        {
            /* 如果不在做错误返回 */
            printf("DrawLine: Invalid Begin position\n");
            return DB_ERR;
        }

        /* b.给限制变量赋值 */
        s32LimitBegin_x = 0;
        s32LimitBegin_y = 0;
        s32LimitEnd_x = _ptFbHandler->m_s32Size_x - 1;
        s32LimitEnd_y = _ptFbHandler->m_s32Size_x - 1;
    }

    /* 4.根据x和y方向总增量选择一个增量大的作为步进总长度(斜率会小)*/
    if (abs(_ptGraHandler->m_u32End_x - _ptGraHandler->m_u32Begin_x) >= abs(_ptGraHandler->m_u32End_y - _ptGraHandler->m_u32Begin_y))
        flength = abs(_ptGraHandler->m_u32End_x - _ptGraHandler->m_u32Begin_x);
    else
        flength = abs(_ptGraHandler->m_u32End_y - _ptGraHandler->m_u32Begin_y);

    /* 5.获取x,y方向上的步进增量  此处的强转int是为了让fAdd_x运算后变得有符号 */
    fAdd_x = ((DB_INT32)_ptGraHandler->m_u32End_x - (DB_INT32)_ptGraHandler->m_u32Begin_x) / flength;
    fAdd_y = ((DB_INT32)_ptGraHandler->m_u32End_y - (DB_INT32)_ptGraHandler->m_u32Begin_y) / flength;

    /* 6.设置线条起始坐标值 */
    fPixel_x = _ptGraHandler->m_u32Begin_x;
    fPixel_y = _ptGraHandler->m_u32Begin_y;

    /* 7.画点 */
    for (s32i = 0; s32i < flength; s32i++)
    {
        /* 填充像素坐标 */
        tGraValTemp.m_u32Center_x = (DB_INT32)(fPixel_x + 0.5);
        tGraValTemp.m_u32Center_y = (DB_INT32)(fPixel_y + 0.5);
        /*  判断参数的合法性(是否超出可显示范围内) */
        if (tGraValTemp.m_u32Center_x < s32LimitBegin_x || tGraValTemp.m_u32Center_x > s32LimitEnd_x ||
            tGraValTemp.m_u32Center_y < s32LimitBegin_y || tGraValTemp.m_u32Center_y > s32LimitEnd_y)
            break;// 像素坐标开始不合法,退出循环
        DrawFullCircle(_ptFbHandler, &tGraValTemp);
        /* 坐标调整 */
        fPixel_x = fPixel_x + fAdd_x;
        fPixel_y = fPixel_y + fAdd_y;
    }

    return DB_OK;
}
/**********************************************************************
* Function:         DrawFullRect
* Description:      矩形填充
* Input:            @_ptFbHandler           屏幕信息句柄
*                   @_u32Begin_x            起始x坐标
*                   @_u32Begin_y            起始y坐标
*                   @_u32End_x              终止x坐标
*                   @_u32End_y              终止y坐标
*                   @_u32Color              填充颜色
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
***********************************************************************/
DB_INT32 DrawFullRect(const T_FbHandler *_ptFbHandler, const DB_UINT32 _u32Begin_x, const DB_UINT32 _u32Begin_y,
                      const DB_UINT32 _u32End_x, const DB_UINT32 _u32End_y, const DB_UINT32 _u32Color)
{
    /* 1.入口参数判断 */
    if (_ptFbHandler == DB_NULL)
    {
        printf("DrawFullRect: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义遍历变量 */
    DB_INT32 s32PiexlMax_x = _u32Begin_x > _u32End_x ? _u32Begin_x : _u32End_x;
    DB_INT32 s32PiexlMax_y = _u32Begin_y > _u32End_y ? _u32Begin_y : _u32End_y;
    DB_INT32 s32PiexlMin_x = _u32Begin_x > _u32End_x ? _u32End_x : _u32Begin_x;
    DB_INT32 s32PiexlMin_y = _u32Begin_y > _u32End_y ? _u32End_y : _u32Begin_y;
    DB_UINT32 u32i;
    DB_UINT32 u32j;

    /* 3.填充 */
    for (u32i = s32PiexlMin_x; u32i <= s32PiexlMax_x; u32i++)
    {
        for (u32j = s32PiexlMin_y; u32j <= s32PiexlMax_y; u32j++)
        {
            /* 判断时候在fb可绘制范围内 */
            if (u32i < _ptFbHandler->m_s32Size_x && u32j < _ptFbHandler->m_s32Size_y)
                _ptFbHandler->m_pDrawPixel(_ptFbHandler, u32i, u32j, _u32Color);
        }
    }

    return DB_OK;
}

/**********************************************************************
* Function:         DrawBmp
* Description:      绘制bmp图片
* Input:            @_ptFbHandler        屏幕信息句柄
*                   @_ptGarNode          图形属性句柄
*                   @_u8PartType         图形节点所属的部件类型
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
***********************************************************************/
DB_INT32 DrawBmp(const T_FbHandler *_ptFbHandler, const DB_VOID *_ptGarNode, const DB_UINT8 _u8PartType)
{
    /* 1.入口参数判断 */
    if (_ptFbHandler == DB_NULL || _ptGarNode == DB_NULL)
    {
        printf("DrawBmp: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    DB_INT32 s32i           =  0;       /* 循环遍历变量 */
    DB_INT32 s32j           =  0;       /* 循环遍历变量 */
    DB_UINT8 *u8pBmpBuffer  = DB_NULL;  /* bmp文件缓存 */
    DB_UINT8 *u8pDstAddr    = DB_NULL;  /* fb目标缓存地址 */
    DB_UINT8 u8BitCount;                /* 图形每个像素的位数 */
    // DB_INT32 s32LimitBegin_x;           /* 坐标限制变量 */
    // DB_INT32 s32LimitBegin_y;
    // DB_INT32 s32LimitEnd_x;
    // DB_INT32 s32LimitEnd_y;
    DB_UINT32 u32Height;                /* 图形的高度 */
    DB_UINT32 u32Width;                 /* 图形的宽度 */
    DB_INT32 s32BmpBegin_x;             /* bmp图形起始坐标 */
    DB_INT32 s32BmpBegin_y;

    /* 3.判断传入的绘图图形属于哪个部件,并图形变量进行赋值 */
    switch (_u8PartType)
    {
    case PART_TITLE:
        /* 设置限制坐标 */
        // s32LimitBegin_x = ((T_TitleItem*)_ptGarNode)->m_ptTitle->m_u32TitleBegin_x;
        // s32LimitBegin_y = ((T_TitleItem*)_ptGarNode)->m_ptTitle->m_u32TitleBegin_y;
        // s32LimitEnd_x = ((T_TitleItem*)_ptGarNode)->m_ptTitle->m_u32TitleEnd_x;
        // s32LimitEnd_y = ((T_TitleItem*)_ptGarNode)->m_ptTitle->m_u32TitleEnd_y;
        /* 设置图形每个像素的位数以及图像的缓存位置 */
        if (((T_TitleItem*)_ptGarNode)->m_u8IsClicked == CLICKED)
        {
            u8BitCount = ((T_TitleItem*)_ptGarNode)->m_u8BitCount_Clicked;
            u8pBmpBuffer = ((T_TitleItem*)_ptGarNode)->m_u8pResources_Clicked;
        }
        else if (((T_TitleItem*)_ptGarNode)->m_u8IsSelcet == SELECT)
        {
            u8BitCount = ((T_TitleItem*)_ptGarNode)->m_u8BitCount_Select;
            u8pBmpBuffer = ((T_TitleItem*)_ptGarNode)->m_u8pResources_Select;
        }
        else
        {
            u8BitCount = ((T_TitleItem*)_ptGarNode)->m_u8BitCount_Default;
            u8pBmpBuffer = ((T_TitleItem*)_ptGarNode)->m_u8pResources_Default;
        }
        /* 设置图形宽度 */
        u32Width = abs(((T_TitleItem*)_ptGarNode)->m_u32Begin_x - ((T_TitleItem*)_ptGarNode)->m_u32End_x) + 1;
        /* 设置图形高度 */
        u32Height = abs(((T_TitleItem*)_ptGarNode)->m_u32Begin_y - ((T_TitleItem*)_ptGarNode)->m_u32End_y) + 1;
        /* 设置bmp图形起始坐标 */
        s32BmpBegin_x = ((T_TitleItem*)_ptGarNode)->m_u32Begin_x;
        s32BmpBegin_y = ((T_TitleItem*)_ptGarNode)->m_u32Begin_y;

        break;
    case PART_MENU:
        /* 设置限制坐标 */
        // s32LimitBegin_x = ((T_TitleItem*)_ptGarNode)->m_ptTitle->m_u32TitleBegin_x;
        // s32LimitBegin_y = ((T_TitleItem*)_ptGarNode)->m_ptTitle->m_u32TitleBegin_y;
        // s32LimitEnd_x = ((T_TitleItem*)_ptGarNode)->m_ptTitle->m_u32TitleEnd_x;
        // s32LimitEnd_y = ((T_TitleItem*)_ptGarNode)->m_ptTitle->m_u32TitleEnd_y;
        /* 设置图形每个像素的位数以及图像的缓存位置 */
        if (((T_MenuItem*)_ptGarNode)->m_u8IsClicked == CLICKED)
        {
            u8BitCount = ((T_MenuItem*)_ptGarNode)->m_u8BitCount_Clicked;
            u8pBmpBuffer = ((T_MenuItem*)_ptGarNode)->m_u8pResources_Clicked;
        }
        else if (((T_MenuItem*)_ptGarNode)->m_u8IsSelcet == SELECT)
        {
            u8BitCount = ((T_MenuItem*)_ptGarNode)->m_u8BitCount_Select;
            u8pBmpBuffer = ((T_MenuItem*)_ptGarNode)->m_u8pResources_Select;
        }
        else
        {
            u8BitCount = ((T_MenuItem*)_ptGarNode)->m_u8BitCount_Default;
            u8pBmpBuffer = ((T_MenuItem*)_ptGarNode)->m_u8pResources_Default;
        }
        /* 设置图形宽度 */
        u32Width = abs(((T_MenuItem*)_ptGarNode)->m_u32Begin_x - ((T_MenuItem*)_ptGarNode)->m_u32End_x) + 1;
        /* 设置图形高度 */
        u32Height = abs(((T_MenuItem*)_ptGarNode)->m_u32Begin_y - ((T_MenuItem*)_ptGarNode)->m_u32End_y) + 1;
        /* 设置bmp图形起始坐标 */
        s32BmpBegin_x = ((T_MenuItem*)_ptGarNode)->m_u32Begin_x;
        s32BmpBegin_y = ((T_MenuItem*)_ptGarNode)->m_u32Begin_y;
        break;
    case PART_NONE:
        /* 设置fb显示限制 */
        break;
    default:
        printf("DrawBmp: parameter error:_u8PartType\n");
        return DB_ERR;
        break;
    }
    /* 4.判断缓冲区是否为空 */
    if (u8pBmpBuffer == DB_NULL)
        printf("DrawBmp: Resources error (NULL)\n");

    /* 5.根据Bmp存储格式显示图片 */
    switch(u8BitCount)
    {
    case RGB16:
        printf("RGB16 Not implemented\n");
        break;

    case RGB24:
        printf("RGB24 Not implemented\n");
        break;

    case RGB32:
        /* 设置初始fb放置bmp文件的地址 */
        u8pDstAddr = _ptFbHandler->m_pu8StartAddr + s32BmpBegin_x * _ptFbHandler->m_s32PixelSize  + s32BmpBegin_y * _ptFbHandler->m_tFix.line_length;
        for (s32i = 0, s32j = u32Height - 1; s32j != 0; s32i++, s32j--)
        {
            /* 向fb中拷贝颜色数据，这里的4指的是每个像素点所占字节大小 */
            memcpy(u8pDstAddr + _ptFbHandler->m_tFix.line_length * s32j, u8pBmpBuffer + u32Width * 4 * s32i, u32Width * 4);
        }
        break;
    default:
        break;
    }

    return DB_OK;
}

/**********************************************************************
* Function:         FBInfoInit
* Description:      打开fb驱动文件,进行帧缓存内存映射,填充_ptFbHandler(g_ptFbInfo)结构体
* Input:            @_ptFbHandler           屏幕信息句柄
*                   @_strFbPathName         fb驱动文件路径名
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_NULL                 失败
                    屏幕信息句柄的指针        成功
***********************************************************************/
T_FbHandler* FbInfoInit(T_FbHandler *_ptFbHandler, const char *_strFbPathName)
{
    /* 1.入口参数合法性判断 */
    if (_ptFbHandler == DB_NULL || _strFbPathName == DB_NULL)
    {
        printf("FbInfoInit: Invalid parameter(Null pointer)\n");
        return DB_NULL;
    }

    /* 2.打开fb的驱动 */
    _ptFbHandler->m_s32Fd = open(_strFbPathName, O_RDWR);
    if (_ptFbHandler->m_s32Fd == DB_ERR)
    {
        perror("FbInfoInit: Cannot open framebuffer device");
        return DB_NULL;
    }

    /* 3.获取fb固定信息 */
    if (ioctl(_ptFbHandler->m_s32Fd, FBIOGET_FSCREENINFO, &_ptFbHandler->m_tFix) == DB_ERR)
    {
        close(_ptFbHandler->m_s32Fd);     /* 关闭文件描述符 */
        perror("FbInfoInit: Error reading fixed information");
        return DB_NULL;
    }

    /* 4.获取fb属性信息 */
    if (ioctl(_ptFbHandler->m_s32Fd, FBIOGET_VSCREENINFO, &_ptFbHandler->m_tVar) == DB_ERR)
    {
        close(_ptFbHandler->m_s32Fd);     /* 关闭文件描述符 */
        perror("FbInfoInit: Error reading variable information");
        return DB_NULL;
    }

    /* 5.填充fb操作方法 */
    /* 通过fix中的两个成员可以计算得到屏幕的实际分辨率  此处的8是每个字节的位数 */
    _ptFbHandler->m_s32PixelSize = _ptFbHandler->m_tVar.bits_per_pixel / 8;
    /* 根据实际的每个像素点所占的字节数选择画点函数 */
    switch(_ptFbHandler->m_tVar.bits_per_pixel)
    {
    case RGB16:
        _ptFbHandler->m_pDrawPixel = DrawPoint_16;
        _ptFbHandler->m_pReadPixel = ReadPoint_16;
        break;
    case RGB24:

    case RGB32:
        _ptFbHandler->m_pDrawPixel = DrawPoint_32;
        _ptFbHandler->m_pReadPixel = ReadPoint_32;
        break;
    }

    /* 6.获取实际可操作的窗口大小 */
    _ptFbHandler->m_s32Size_y = _ptFbHandler->m_tVar.yres;
    _ptFbHandler->m_s32Size_x = _ptFbHandler->m_tVar.xres;

    /* 7.进行framebuffer的操作缓冲区内存映射 */
    _ptFbHandler->m_pu8StartAddr = (DB_UINT8 *)mmap(NULL, _ptFbHandler->m_tFix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, _ptFbHandler->m_s32Fd, 0);
    if (_ptFbHandler->m_pu8StartAddr == MAP_FAILED)
    {
        close(_ptFbHandler->m_s32Fd);     /* 关闭文件描述符 */
        perror ("FbInfoInit: Failed to map framebuffer device to memory");
        return DB_NULL;
    }

    /* 8.返回 */
    return _ptFbHandler;
}

/**********************************************************************
* Function:         FbInfoDeInit
* Description:      FbInfoDeInit反向初始化，关闭文件描述符，指针指向NULL，取消内存映射
* Input:            @_ptFbHandler   屏幕信息句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           无
**********************************************************************/
DB_VOID FbInfoDeInit(T_FbHandler *_ptFbHandler)
{
    /* 1.入口参数合法性判断 */
    if (_ptFbHandler == DB_NULL)
    {
        printf("FbInfoDeInit: Invalid parameter(Null pointer)\n");
        return;
    }

    /* 2.关闭文件描述符 */
    if (_ptFbHandler->m_s32Fd != DB_ERR)
    {
        close(_ptFbHandler->m_s32Fd);
        _ptFbHandler->m_s32Fd = DB_ERR;
    }

    /* 3.解除内存映射 */
    if(_ptFbHandler->m_pu8StartAddr != DB_NULL)
    {
        munmap (_ptFbHandler->m_pu8StartAddr, _ptFbHandler->m_tFix.smem_len);
        _ptFbHandler->m_pu8StartAddr = DB_NULL;
    }

    /* 4.清除回调函数 */
    _ptFbHandler->m_pDrawPixel = DB_NULL;
    _ptFbHandler->m_pReadPixel = DB_NULL;
}

/**********************************************************************
* Function:         ClearScreen
* Description:      清除屏幕显示的内容
* Input:            @_ptFbHandler   屏幕信息句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           无
**********************************************************************/
DB_VOID ClearScreen(const T_FbHandler * _ptFbHandler)
{
    /* 1.入口参数合法性判断 */
    if (_ptFbHandler == DB_NULL)
    {
        printf("ClearScreen: Invalid parameter(Null pointer)\n");
        return;
    }

    /* 2.清空fb */
    memset(_ptFbHandler->m_pu8StartAddr, 0, _ptFbHandler->m_tFix.smem_len); /* 清屏为黑色，效率高 */
}