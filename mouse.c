#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "mouse.h"
 

/***********************************************************
*                      函数原型                             *
************************************************************/

/**********************************************************************
* Function:         MouseInit
* Description:      鼠标设备初始化
* Input:            @_ptMouseHandler         鼠标信息句柄
*                   @_strMousePathName       鼠标驱动路径
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_ERR                   失败
*                   DB_OK                    成功
**********************************************************************/
DB_INT32 MouseInit(T_MouseHandler *_ptMouseHandler, const DB_CHAR *_strMousePathName)
{
    /* 1.入口参数合法性判断 */ 
    if (_ptMouseHandler == DB_NULL || _strMousePathName == DB_NULL)
    {
        printf("MouseInit: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.打开input设备的事件节点 */
    _ptMouseHandler->m_s32Fd = open(_strMousePathName, O_RDONLY);
    if(_ptMouseHandler->m_s32Fd == DB_ERR)
    {
        perror("MouseInit: Open mouse event fail");
        return DB_ERR;
    }

    /* 3.申请鼠标鼠标属性(颜色，形状)空间 */
    if ((_ptMouseHandler->m_ptGarMouse = malloc(sizeof(T_GarMouse))) == DB_NULL)
    {
        perror("MouseInit: _ptMouseHandler->m_ptGarMouse malloc fail");
        return DB_ERR;
    }

    /* 4.设置鼠标参数 */
    /* 初始化鼠标颜色 */
    _ptMouseHandler->m_ptGarMouse->m_u32Color = MOUSE_COLOR;
    /* 首次绘制鼠标不需要还原场景 */
    _ptMouseHandler->m_ptGarMouse->m_bIsRestore = DB_FALSE;
    
    return DB_OK;
}


/**********************************************************************
* Function:         MouseDeInit
* Description:      鼠标设备反初始化
* Input:            @_ptMouseHandler         鼠标信息句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           无
**********************************************************************/
DB_VOID MouseDeInit(T_MouseHandler *_ptMouseHandler)
{
    /* 1.入口参数合法性判断 */ 
    if (_ptMouseHandler == DB_NULL)
    {
        printf("MouseDeInit: Invalid parameter(Null pointer)\n");
        return ;
    }

    /* 2.关闭文件描述符 */
    if (_ptMouseHandler->m_s32Fd == DB_ERR)
    {
        close(_ptMouseHandler->m_s32Fd);
        _ptMouseHandler->m_s32Fd = DB_ERR;
    }

    /* 3.释放鼠标属性(颜色，形状)申请的空间 */
    if (_ptMouseHandler->m_ptGarMouse != DB_NULL)
    {
        free(_ptMouseHandler->m_ptGarMouse);
        _ptMouseHandler->m_ptGarMouse = DB_NULL;
    }
}

/**********************************************************************
* Function:         UpdateMouseState
* Description:      更新鼠标坐标位置及按键状态(绝对)
* Input:            @_ptMouseHandler         鼠标信息句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           无
**********************************************************************/
DB_VOID UpdateMouseState(T_MouseHandler *_ptMouseHandler)
{
    /* 1.入口参数合法性判断 */ 
    if (_ptMouseHandler == DB_NULL)
    {
        printf("UpdateMouseState: Invalid parameter(Null pointer)\n");
        return ;
    }

    struct input_event tMouseEvent;
    /* 2.读取鼠标的更新时间内容 */
    read(_ptMouseHandler->m_s32Fd, &tMouseEvent, sizeof(struct input_event));

    /* 3.判断事件类型 更新鼠标信息 */
    switch(tMouseEvent.type)
    {
        // case EV_SYN:        /* 同步事件 */
        case EV_KEY:           /* 点击事件 */
            if (tMouseEvent.code == M_KEY_LEFT)                        /*左键动作 */
            {   
                _ptMouseHandler->m_u8LeftState = tMouseEvent.value;    /* 记录左键状态 */
            }
            else if (tMouseEvent.code == M_KEY_RIGHT)                  /* 右键动作 */
            {
                _ptMouseHandler->m_u8RightState = tMouseEvent.value;   /* 记录右键状态 */
            }
            else if (tMouseEvent.code == M_KEY_MIDDLE)                 /* 中键动作 */
            {
                _ptMouseHandler->m_u8MiddleState = tMouseEvent.value;  /* 记录中键状态 */
            }
            
            /* 修改更新标志位 */
            _ptMouseHandler->m_u8UpdateKeyState = UPDATE;   /* 该标志位表示按键有更新，不管是按下还是弹起 */
            break;
        case EV_REL:        /* 相对位移 */
            printf("EV_REL\n");
            break;
        case EV_ABS:        /* 绝对坐标 */
            /* 获取上一次坐标 */
            _ptMouseHandler->m_s32OldPosition_y = _ptMouseHandler->m_s32NowPosition_y;
            _ptMouseHandler->m_s32OldPosition_x = _ptMouseHandler->m_s32NowPosition_x;
            
            /* 更新坐标 */
            if(tMouseEvent.code == REL_X)   /* x轴移动 */
            {
                _ptMouseHandler->m_s32NowPosition_x = tMouseEvent.value / MOUSE_ZOOM_X;  /* 将鼠标坐标范围映射到显示范围内 */
            }
            if(tMouseEvent.code == REL_Y)   /* y轴移动 */
            {
                _ptMouseHandler->m_s32NowPosition_y = tMouseEvent.value / MOUSE_ZOOM_Y; /* 将鼠标坐标范围映射到显示范围内 */
            }

            /* 修改更新标志位 */
            _ptMouseHandler->m_u8UpdatePosition = UPDATE;   /* 该标志位表示鼠标坐标有更新 */
            break;
        default:
            break;
    }

    /* 4.打印鼠标位置信息 */
    #if DISABLE
    printf("nx: %d  ny: %d ox: %d  oy: %d\n", _ptMouseHandler->m_s32NowPosition_x, _ptMouseHandler->m_s32NowPosition_y, 
            _ptMouseHandler->m_s32OldPosition_x, _ptMouseHandler->m_s32OldPosition_y);
    #endif
}