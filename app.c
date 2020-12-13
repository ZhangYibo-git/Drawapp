#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <mxml.h>
#include "app.h"


/***********************************************************
*                      引用变量                             *
************************************************************/
extern T_MouseHandler g_tMouseHandler;     /* 全局鼠标操作句柄 */
extern T_FbHandler g_tFbInfo;              /* 全局fb信息 */
extern T_Window g_tWindow;                 /* 全局的窗口变量 */

extern DB_BOOL g_bMainIsClose;             /* 判断是否关闭主线程 */
/***********************************************************
*                      全局变量                             *
************************************************************/
/* 鼠标的图案 */
static const DB_UINT8 g_u8aMousePic[10][10] =
{
    {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0},
    {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0},
    {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0},
    {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0},
    {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0},
    {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0},
    {0x1, 0x1, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0},
    {0x1, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1},
    {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x0},
    {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0},
};


/***********************************************************
*                      函数原型                             *
************************************************************/

/**********************************************************************
* Function:         DrawMouse
* Description:      绘制鼠标(如果坐标进行了更新才会进行鼠标的绘制)
* Input:            @_ptFbHandler         fb信息句柄
*                   @_ptMouseHander       鼠标信息句柄
* Attention:        该函数执行的条件是鼠标位置的信息更新 也就是鼠标信息句柄中m_u8UpdatePosition为UPDATE
*                   并在完成处理后将m_u8UpdatePosition置为NO_UPDATE
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
*                   DB_UNMET                失败：为满足进入条件
**********************************************************************/
DB_INT32 DrawMouse(const T_FbHandler *_ptFbHandler, T_MouseHandler *_ptMouseHander)
{
    /* 1.入口参数合法性判断 */
    if (_ptFbHandler == DB_NULL || _ptMouseHander == DB_NULL)
    {
        printf("DrawMouse: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.检查坐标位置是否更新 */
    if (_ptMouseHander->m_u8UpdatePosition != UPDATE)
        return DB_UNMET;

    /* 3.定义变量 */
    DB_INT32 s32Pixel_x    = 0;             /* 遍历坐标x */
    DB_INT32 s32Pixel_y    = 0;             /* 遍历坐标y */

    /* 4.判断是否还原上次鼠标位置场景 */
    if (_ptMouseHander->m_ptGarMouse->m_bIsRestore)
    {
        /* 遍历x坐标 */
        for (s32Pixel_x = _ptMouseHander->m_s32OldPosition_x; s32Pixel_x < _ptMouseHander->m_s32OldPosition_x + MOUSE_SIZE; s32Pixel_x++)
        {
            /* 遍历y坐标 */
            for (s32Pixel_y = _ptMouseHander->m_s32OldPosition_y; s32Pixel_y < _ptMouseHander->m_s32OldPosition_y + MOUSE_SIZE; s32Pixel_y++)
            {
                /* 判断x,y坐标的合法性 */
                if(s32Pixel_x < _ptFbHandler->m_s32Size_x && s32Pixel_y < _ptFbHandler->m_s32Size_y)
                    _ptFbHandler->m_pDrawPixel(_ptFbHandler, s32Pixel_x, s32Pixel_y,
                                               _ptMouseHander->m_ptGarMouse->m_au32OldBuffer[s32Pixel_x - _ptMouseHander->m_s32OldPosition_x][s32Pixel_y - _ptMouseHander->m_s32OldPosition_y]);
            }
        }
    }

    /* 5.获取鼠标新位置的场景 */
    /* 遍历x坐标 */
    for (s32Pixel_x = _ptMouseHander->m_s32NowPosition_x; s32Pixel_x < _ptMouseHander->m_s32NowPosition_x + MOUSE_SIZE; s32Pixel_x++)
    {
        /* 遍历y坐标 */
        for (s32Pixel_y = _ptMouseHander->m_s32NowPosition_y; s32Pixel_y < _ptMouseHander->m_s32NowPosition_y + MOUSE_SIZE; s32Pixel_y++)
        {
            /* 判断x,y坐标的合法性 */
            if(s32Pixel_x < _ptFbHandler->m_s32Size_x && s32Pixel_y < _ptFbHandler->m_s32Size_y)
                _ptMouseHander->m_ptGarMouse->m_au32NewBuffer[s32Pixel_x - _ptMouseHander->m_s32NowPosition_x][s32Pixel_y - _ptMouseHander->m_s32NowPosition_y] =
                    _ptFbHandler->m_pReadPixel(_ptFbHandler, s32Pixel_x, s32Pixel_y);   /* 获取像素点 */
        }
    }

    /* 6.在新位置绘制鼠标 */
    /* 遍历x坐标 */
    for (s32Pixel_x = _ptMouseHander->m_s32NowPosition_x; s32Pixel_x < _ptMouseHander->m_s32NowPosition_x + MOUSE_SIZE; s32Pixel_x++)
    {
        /* 遍历y坐标 */
        for (s32Pixel_y = _ptMouseHander->m_s32NowPosition_y; s32Pixel_y < _ptMouseHander->m_s32NowPosition_y + MOUSE_SIZE; s32Pixel_y++)
        {
            /* 判断是否绘制鼠标点 */
            if (g_u8aMousePic[s32Pixel_x - _ptMouseHander->m_s32NowPosition_x][s32Pixel_y - _ptMouseHander->m_s32NowPosition_y] == POINT)
            {
                /* 判断x,y坐标的合法性 */
                if(s32Pixel_x < _ptFbHandler->m_s32Size_x && s32Pixel_y < _ptFbHandler->m_s32Size_y)
                    _ptFbHandler->m_pDrawPixel(_ptFbHandler, s32Pixel_x, s32Pixel_y, _ptMouseHander->m_ptGarMouse->m_u32Color);
            }
        }
    }

    /* 7.将新场景拷贝到旧场景, 用于下次还原*/
    memcpy(_ptMouseHander->m_ptGarMouse->m_au32OldBuffer, _ptMouseHander->m_ptGarMouse->m_au32NewBuffer, sizeof(_ptMouseHander->m_ptGarMouse->m_au32NewBuffer));

    /* 8.设置变量 */
    /* 之后使用鼠标时 都做还原场景操作 */
    if (_ptMouseHander->m_ptGarMouse->m_bIsRestore == DB_FALSE)
        _ptMouseHander->m_ptGarMouse->m_bIsRestore = DB_TRUE;
    /* 读取完坐标之后，更新坐标状态为未更新，说明已经对鼠标的更新动作进行读取 */
    _ptMouseHander->m_u8UpdatePosition = NO_UPDATE;

    return DB_OK;
}

/**********************************************************************
* Function:         CleanMouse
* Description:      清除鼠标
* Input:            @_ptFbHandler         fb信息句柄
*                   @_ptMouseHander       鼠标信息句柄
* Attention:        该函数会清除鼠标的显示，并将还原标志位置为NO_REFRESH，
*                   表示下一次不进行鼠标场景的还原
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
*                   DB_UNMET                失败：为满足进入条件
**********************************************************************/
DB_INT32 CleanMouse(const T_FbHandler *_ptFbHandler, T_MouseHandler *_ptMouseHander, const T_Window *_ptWindow)
{
    /* 1.入口参数合法性判断 */
    if (_ptFbHandler == DB_NULL || _ptMouseHander == DB_NULL)
    {
        printf("CleanMouse: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.触发条件 */
    if (_ptWindow->m_u8IsRefresh == NO_UPDATE && _ptWindow->m_tTitle.m_u8IsRefresh == NO_UPDATE &&
        _ptWindow->m_DrawingBoard.m_u8IsRefresh == NO_UPDATE  && _ptWindow->m_tMenu.m_u8IsRefresh == NO_UPDATE)
        return DB_UNMET;

    /* 3.定义变量 */
    DB_INT32 s32Pixel_x    = 0;             /* 遍历坐标x */
    DB_INT32 s32Pixel_y    = 0;             /* 遍历坐标y */

    /* 4.还原上次鼠标位置场景 */
    /* 遍历x坐标 */
    for (s32Pixel_x = _ptMouseHander->m_s32NowPosition_x; s32Pixel_x < _ptMouseHander->m_s32NowPosition_x + MOUSE_SIZE; s32Pixel_x++)
    {
        /* 遍历y坐标 */
        for (s32Pixel_y = _ptMouseHander->m_s32NowPosition_y; s32Pixel_y < _ptMouseHander->m_s32NowPosition_y + MOUSE_SIZE; s32Pixel_y++)
        {
            /* 判断x,y坐标的合法性 */
            if(s32Pixel_x < _ptFbHandler->m_s32Size_x && s32Pixel_y < _ptFbHandler->m_s32Size_y)
                _ptFbHandler->m_pDrawPixel(_ptFbHandler, s32Pixel_x, s32Pixel_y,
                                            _ptMouseHander->m_ptGarMouse->m_au32OldBuffer[s32Pixel_x - _ptMouseHander->m_s32NowPosition_x][s32Pixel_y - _ptMouseHander->m_s32NowPosition_y]);
        }
    }

    /* 5.设置变量 */
    _ptMouseHander->m_ptGarMouse->m_bIsRestore = NO_REFRESH;

    return DB_OK;
}

/**********************************************************************
* Function:         IsOnRange
* Description:      判读鼠标是否在范围内的函数
* Input:            @_ptPartType            部件类型
* Attention:        判断当前鼠标的坐标是否在传入参数的范围内，传入的参数只
*                   能是菜单、画板、标题
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           UpdateWindow函数
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 IsOnRange(DB_VOID * _ptPartType)
{
    /* 1.定义局部变量 */
    DB_INT32 s32LimitBegin_x;                       /* 绘图限制变量 */
    DB_INT32 s32LimitBegin_y;
    DB_INT32 s32LimitEnd_x;
    DB_INT32 s32LimitEnd_y;

    /*  2.变量赋值 */
    if (((T_Title *)_ptPartType)->m_u32PartType == PART_TITLE)      /* 标题范围 */
    {
        s32LimitBegin_x = ((T_Title *)_ptPartType)->m_u32TitleBegin_x;
        s32LimitBegin_y = ((T_Title *)_ptPartType)->m_u32TitleBegin_y;
        s32LimitEnd_x = ((T_Title *)_ptPartType)->m_u32TitleEnd_x;
        s32LimitEnd_y = ((T_Title *)_ptPartType)->m_u32TitleEnd_y;
    }
    else if (((T_Menu *)_ptPartType)->m_u32PartType == PART_MENU)   /* 菜单范围 */
    {
        s32LimitBegin_x = ((T_Menu *)_ptPartType)->m_u32MenuBegin_x;
        s32LimitBegin_y = ((T_Menu *)_ptPartType)->m_u32MenuBegin_y;
        s32LimitEnd_x = ((T_Menu *)_ptPartType)->m_u32MenuEnd_x;
        s32LimitEnd_y = ((T_Menu *)_ptPartType)->m_u32MenuEnd_y;
    }
    else if(((T_DarwingBoard *)_ptPartType)->m_u32PartType == PART_DRAWING_BOARD)   /* 画板范围 */
    {
        s32LimitBegin_x = ((T_DarwingBoard *)_ptPartType)->m_u32DrawingBoardBegin_x;
        s32LimitBegin_y = ((T_DarwingBoard *)_ptPartType)->m_u32DrawingBoardBegin_y;
        s32LimitEnd_x = ((T_DarwingBoard *)_ptPartType)->m_u32DrawingBoardEnd_x;
        s32LimitEnd_y = ((T_DarwingBoard *)_ptPartType)->m_u32DrawingBoardEnd_y;
    }
    else
    {
        printf("Not On Window\n");
        return DB_ERR;
    }
    
    /* 3.判断是否在标题范围内 */
    if (g_tMouseHandler.m_s32NowPosition_x >= s32LimitBegin_x && g_tMouseHandler.m_s32NowPosition_x <= s32LimitEnd_x &&
        g_tMouseHandler.m_s32NowPosition_y >= s32LimitBegin_y && g_tMouseHandler.m_s32NowPosition_y <= s32LimitEnd_y)
    {
        return DB_OK;
    }

    return DB_ERR;
}

/**********************************************************************
* Function:         GarRecordBegin
* Description:      记录图形起始属性
* Input:            @_ptGarVal              图形信息句柄
*                   @_ptWindow              窗口句柄
*                   @_ptFbHandler           fb信息句柄
*                   @_ptMouseHander         鼠标操作句柄
* Attention:        该函数记录图形的起始位置 记录图形的类型、颜色、宽度信息
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           UpdateWindow函数
* Return:           DB_ERR                  失败
*                   DB_OK                   成功
**********************************************************************/
static DB_INT32 GarRecordBegin(T_Graphical *_ptGarVal, T_Window *_ptWindow, const T_FbHandler *_ptFbHandler, T_MouseHandler *_ptMouseHander)
{
    /* 1.入口参数判断 */
    if (_ptGarVal == DB_NULL || _ptWindow == DB_NULL || _ptFbHandler == DB_NULL || _ptMouseHander == DB_NULL)
    {
        printf("GarRecordBegin: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义返回值变量 */
    DB_INT32 s32Ret = DB_OK;

    /* 3.记录全局图形信息 */
    _ptGarVal->m_u8Type = _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u8Type;
    _ptGarVal->m_u32Color_32 = _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u32Color_32;
    _ptGarVal->m_u8Width = _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u8Width;

    /* 4.记录起始坐标 */
    switch (_ptGarVal->m_u8Type)
    {
    case RING:
        /* 圆环需要指定圆心 */
        _ptGarVal->m_u32Center_x = _ptMouseHander->m_s32NowPosition_x;
        _ptGarVal->m_u32Center_y = _ptMouseHander->m_s32NowPosition_y;
        /* 当坐标记录到极限值时为了满足绘制图形的输入参数的条件，把边界值做 -1 操作 */
        if (_ptGarVal->m_u32Center_x == _ptFbHandler->m_s32Size_x)
                _ptGarVal->m_u32Center_x -= 1;
        if (_ptGarVal->m_u32Center_y == _ptFbHandler->m_s32Size_y)
            _ptGarVal->m_u32Center_y -= 1;
        break;
    case RECTANGE:

    case LINE:
        /* 其他图形都是指定开始坐标 */
        _ptGarVal->m_u32Begin_x = _ptMouseHander->m_s32NowPosition_x;
        _ptGarVal->m_u32Begin_y = _ptMouseHander->m_s32NowPosition_y;
        /* 当坐标记录到极限值时为了满足绘制图形的输入参数的条件，把边界值做 -1 操作 */
        if (_ptGarVal->m_u32Begin_x == _ptFbHandler->m_s32Size_x)
                _ptGarVal->m_u32Begin_x -= 1;
        if (_ptGarVal->m_u32Begin_y == _ptFbHandler->m_s32Size_y)
            _ptGarVal->m_u32Begin_y -= 1;
        break;
    default:
        s32Ret = DB_ERR;
        break;
    }

    /* 5.返回 */
    return s32Ret;
}

/**********************************************************************
* Function:         GarRecordEnd
* Description:      完成图形属性的记录
* Input:            @_ptGarVal              图形信息句柄
*                   @_ptWindow              窗口句柄
*                   @_ptFbHandler           fb信息句柄
*                   @_ptMouseHander         鼠标操作句柄
* Attention:        该函数记录图形的结束位置 并添加到画板绘图链表中
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           UpdateWindow函数
* Return:           DB_ERR                  失败
*                   DB_OK                   成功
**********************************************************************/
static DB_INT32 GarRecordEnd(T_Graphical *_ptGarVal, T_Window *_ptWindow, const T_FbHandler *_ptFbHandler, T_MouseHandler *_ptMouseHander)
{
    /* 1.入口参数判断 */
    if (_ptGarVal == DB_NULL || _ptWindow == DB_NULL || _ptFbHandler == DB_NULL || _ptMouseHander == DB_NULL)
    {
        printf("GarRecordEnd: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义返回值变量 */
    DB_INT32 s32Ret = DB_OK;

    /* 3.记录结束坐标 */
    switch (_ptGarVal->m_u8Type)
    {
    case RING:
        /* 设置圆环半径 */
        _ptGarVal->m_u32Radius =
        sqrt((_ptMouseHander->m_s32NowPosition_x - _ptGarVal->m_u32Center_x) * (_ptMouseHander->m_s32NowPosition_x - _ptGarVal->m_u32Center_x) +
            (_ptMouseHander->m_s32NowPosition_y - _ptGarVal->m_u32Center_y) * (_ptMouseHander->m_s32NowPosition_y - _ptGarVal->m_u32Center_y));    /* 求圆环半径 */
        break;
    case RECTANGE:

    case LINE:
        /* 其他图形都是指定结束坐标 */
        _ptGarVal->m_u32End_x = _ptMouseHander->m_s32NowPosition_x;
        _ptGarVal->m_u32End_y = _ptMouseHander->m_s32NowPosition_y;
        /* 当坐标记录到极限值时为了满足绘制图形的输入参数的条件，把边界值做 -1 操作 */
        if (_ptGarVal->m_u32End_x == _ptFbHandler->m_s32Size_x)
            _ptGarVal->m_u32End_x -= 1;
        if (_ptGarVal->m_u32End_y == _ptFbHandler->m_s32Size_y)
            _ptGarVal->m_u32End_y -= 1;
        break;

    default:
        s32Ret = DB_ERR;
        break;
    }

    /* 4.向画板添加节点 */
    if (AddGarItem(_ptGarVal, &_ptWindow->m_DrawingBoard) == DB_ERR)
    {
        printf("GarRecordEnd: AddBackgroundGar fail\n");
        s32Ret = DB_ERR;
    }

    #if DISABLE
    /* 打印图形起始位置，和结束位置 */
    printf(" %d %d %d %d\n", _ptGarVal->m_u32Begin_x, _ptGarVal->m_u32Begin_y, _ptGarVal->m_u32End_x, _ptGarVal->m_u32End_y);
    #endif

    /* 5.返回 */
    return s32Ret;
}

/**********************************************************************
* Function:         UpdateTitle_Select
* Description:      根据鼠标的位置来设置是否选中标题选项
* Input:            @_ptWindow              窗口句柄
*                   @_ptMouseHander         鼠标句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           UpdateWindow函数
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 UpdateTitle_Select(T_Window *_ptWindow, T_MouseHandler *_ptMouseHander)
{
    /* 1.入口参数判断 */
    if (_ptWindow == DB_NULL || _ptMouseHander == DB_NULL)
    {
        printf("UpdateTitle_Select: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    static DB_UINT8 s_LastState = NO_SELECT;/* 静态变量用于记录上一次的状态 */
    T_TitleItemNode *ptTitleItemNode;       /* 临时菜单节点变量 */
    DB_UINT8 u8_ClickMenuNum;               /* 点击时鼠标所在的按钮序号 */
    DB_UINT8 u8i;                           /* 遍历变量 */

    /* 3.执行函数条件判断(要想不跳出该函数，那么必须) */
    if(_ptMouseHander->m_u8UpdatePosition == UPDATE)
        return DB_UNMET;

    /* 4.选定条件判断 */
    if (IsOnRange(&_ptWindow->m_tTitle) == DB_OK)   /* 在标题范围内 */
    {
        /* 获取点击的按钮的序列号 */
        u8_ClickMenuNum = (_ptWindow->m_tTitle.m_u32TitleEnd_x - _ptMouseHander->m_s32NowPosition_x) / TITLE_BAR_HEIGHT;

        /* 获取标题选项头节点  */
        ptTitleItemNode = &_ptWindow->m_tTitle.m_tTitleItemHead;

        /* 遍历到链表末尾 */
        while (ptTitleItemNode->m_ptNext != DB_NULL)
        {
            /* 移动的下一个节点 */
            ptTitleItemNode = ptTitleItemNode->m_ptNext;
        }

        /* 从链表尾向前遍历，确定点击功能 */
        for (u8i = 0; u8i < _ptWindow->m_tTitle.m_u8TitleItemNum; u8i++)
        {
            if (u8i == u8_ClickMenuNum)/* 是否是选中的按钮 */
            {
                ptTitleItemNode->m_tTitleItem.m_u8IsSelcet = SELECT;           /* 设置为选中 */
                if (s_LastState == NO_SELECT)
                {
                    ptTitleItemNode->m_tTitleItem.m_u8IsRefresh = REFRESH;     /* 设置为需要刷新 */
                    s_LastState = SELECT;
                    _ptWindow->m_tTitle.m_u8IsRefresh = UPDATE;                /* 置位标题栏更新标志 */
                }
            }
            else
            {
                ptTitleItemNode->m_tTitleItem.m_u8IsSelcet = NO_SELECT;        /* 设置为选中 */
                if (s_LastState == SELECT)
                {
                    ptTitleItemNode->m_tTitleItem.m_u8IsRefresh = REFRESH;     /* 设置为需要刷新 */
                    s_LastState = NO_SELECT;
                    _ptWindow->m_tTitle.m_u8IsRefresh = UPDATE;                /* 置位标题栏更新标志 */
                }
            }
            ptTitleItemNode = ptTitleItemNode->m_ptPrior;                      /* 移动到上一个节点 */
        }
    }
    else        /* 不在标题范围内 */
    {
        /* 获取标题选项头节点  */
        ptTitleItemNode = &_ptWindow->m_tTitle.m_tTitleItemHead;
        /*  */
        while (ptTitleItemNode->m_ptNext != DB_NULL)
        {
            ptTitleItemNode = ptTitleItemNode->m_ptNext;                    /* 移动到下一个节点 */
            ptTitleItemNode->m_tTitleItem.m_u8IsSelcet = NO_SELECT;         /* 设置为选中 */
            if (s_LastState == SELECT)
            {
                ptTitleItemNode->m_tTitleItem.m_u8IsRefresh = REFRESH;      /* 设置为需要刷新 */
                s_LastState = NO_SELECT;
                _ptWindow->m_tTitle.m_u8IsRefresh = UPDATE;                 /* 置位标题栏更新标志 */
            }
        }
    }

    return DB_OK;
}

/**********************************************************************
* Function:         UpdateTitle_Clicked
* Description:      根据鼠标的点击效果来更新标题的变化
* Input:            @_ptWindow              窗口句柄
*                   @_ptMouseHander         鼠标句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           UpdateWindow函数
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 UpdateTitle_Clicked(T_Window *_ptWindow, T_MouseHandler *_ptMouseHander)
{
    /* 1.入口参数合法性判断 */
    if (_ptWindow == DB_NULL || _ptMouseHander == DB_NULL)
    {
        printf("UpdateTitle_Clicked: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    T_TitleItemNode *ptTitleItemNode;   /* 临时菜单节点变量 */
    DB_UINT8 u8_ClickMenuNum;           /* 点击时鼠标所在的按钮序号 */
    DB_UINT8 u8i;                       /* 遍历变量 */

    /* 3.获取点击的按钮的序列号 */
    u8_ClickMenuNum = (_ptWindow->m_tTitle.m_u32TitleEnd_x - _ptMouseHander->m_s32NowPosition_x) / TITLE_BAR_HEIGHT;

    /* 4.获取标题选项头节点  */
    ptTitleItemNode = &_ptWindow->m_tTitle.m_tTitleItemHead;

    /* 5.遍历到链表末尾 */
    while (ptTitleItemNode->m_ptNext != DB_NULL)
    {
        /* 移动的下一个节点 */
        ptTitleItemNode = ptTitleItemNode->m_ptNext;
    }

    /* 6.从链表尾向前遍历，确定点击功能 */
    for (u8i = 0; u8i < _ptWindow->m_tTitle.m_u8TitleItemNum; u8i++)
    {
        if (u8i == u8_ClickMenuNum)
        {
            ptTitleItemNode->m_tTitleItem.m_u8IsClicked = CLICKED;      /* 设置为点击 */
            ptTitleItemNode->m_tTitleItem.m_u8IsRefresh = REFRESH;      /* 设置为需要刷新 */
        }
        /* 移动到上一个节点 */
        ptTitleItemNode = ptTitleItemNode->m_ptPrior;
    }

    return DB_OK;
}

/**********************************************************************
* Function:         UpdateMenu_Clicked
* Description:      根据鼠标的点击效果来更新菜单的变化
* Input:            @_ptWindow              窗口句柄
*                   @_ptMouseHander         鼠标句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           UpdateWindow函数
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 UpdateMenu_Clicked(T_Window *_ptWindow, T_MouseHandler *_ptMouseHander)
{
    /* 1.入口参数合法性判断 */
    if (_ptWindow == DB_NULL || _ptMouseHander == DB_NULL)
    {
        printf("UpdateMenu_Clicked: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    T_MenuItemNode *ptMenuItemNode; /* 临时菜单节点变量 */
    DB_UINT8 u8_ClickMenuNum;       /* 点击时鼠标所在的按钮序号 */
    DB_UINT8 u8i;                   /* 遍历变量 */
    
    /* 3.获取点击时按钮的序列号 */
    u8_ClickMenuNum = _ptMouseHander->m_s32NowPosition_x / MENU_ITEN_HEIGHT;
    
    /* 4.获取菜单选项头节点 */
    ptMenuItemNode = &_ptWindow->m_tMenu.m_tMenuItemHead;

    /* 5.遍历到链表末尾，同时修改上一个选中的变量为未选中 */
    while (ptMenuItemNode->m_ptNext != DB_NULL)
    {
        /* 移动的下一个节点 */
        ptMenuItemNode = ptMenuItemNode->m_ptNext;

        if (ptMenuItemNode->m_tMenuItem.m_u8IsClicked == CLICKED)
        {
            ptMenuItemNode->m_tMenuItem.m_u8IsClicked = NO_CLICKED;   /* 设置为未选中 */
            ptMenuItemNode->m_tMenuItem.m_u8IsRefresh = REFRESH;    /* 设置为需要刷新 */
        }
    }

    /* 6.从链表尾向前遍历，修改点击的图标为选中状态 */
    for (u8i = 0; u8i < _ptWindow->m_tMenu.m_u8MenuItemNum; u8i++)
    {
        if (u8i == u8_ClickMenuNum)
        {
            ptMenuItemNode->m_tMenuItem.m_u8IsClicked = CLICKED;      /* 设置为选中 */
            ptMenuItemNode->m_tMenuItem.m_u8IsRefresh = REFRESH;     /* 设置为需要刷新 */
        }
        /* 移动到上一个节点 */
        ptMenuItemNode = ptMenuItemNode->m_ptPrior;
    }

    return DB_OK;
}

/**********************************************************************
* Function:         UpdateWindow
* Description:      更新界面部件变化信息，并置为相应的更新标志位
* Input:            @_ptFbHandler           fb信息句柄
*                   @_ptWindow              窗口句柄
*                   @_ptMouseHander         鼠标信息句柄
* Attention:        该函数调用的条件是鼠标按键的信息更新 也就是鼠标信息句柄中m_u8UpdateKeyState为UPDATE
*                   并在完成处理后将m_u8UpdateKeyState置为NO_UPDATE
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
*                   DB_UNMET                失败：为满足进入条件
**********************************************************************/
DB_INT32 UpdateWindow(const T_FbHandler *_ptFbHandler, T_Window *_ptWindow, T_MouseHandler *_ptMouseHander)
{
    /* 1.入口参数合法性判断 */
    if (_ptFbHandler == DB_NULL || _ptWindow == DB_NULL || _ptMouseHander == DB_NULL)
    {
        printf("UpdateWindow: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }
    /* 2.判断鼠标选中的位置 */
    UpdateTitle_Select(_ptWindow, _ptMouseHander);  

    /* 3.检查鼠标按键是否更新 */
    if (_ptMouseHander->m_u8UpdateKeyState != UPDATE )//&& _ptMouseHander->m_u8UpdatePosition != UPDATE)
        return DB_UNMET;

    /* 4.变量定义 */
    static DB_UINT8 s_u8KeyState = NO_UPDATE;       /* 该变量标识按键是否按下 */
    static T_Graphical tGraVal;                     /* 图形属性结构体，赋值用 */

    /* 5.判断按下按键时所在位置 */
    if (_ptMouseHander->m_u8LeftState == MY_KEY_DOWN)   /* 左键按下 */
    {
        /* a.判断鼠标是否在标题栏范围内 */
        if (IsOnRange(&_ptWindow->m_tTitle) == DB_OK)
        {
            /* 更新标题 */
            UpdateTitle_Clicked(_ptWindow, _ptMouseHander);
            /* 置位标题栏更新标志 */
            _ptWindow->m_tTitle.m_u8IsRefresh = UPDATE;
            /* 退出程序 */
            goto RETURN;
        }

        /* b.判断鼠标是否在菜单栏范围内 */
        if (IsOnRange(&_ptWindow->m_tMenu) == DB_OK)
        {
            /* 更新菜单选项 */
            UpdateMenu_Clicked(_ptWindow, _ptMouseHander);
            /* 置位菜单更新标志 */
            _ptWindow->m_tMenu.m_u8IsRefresh = UPDATE;
            /* 退出程序 */
            goto RETURN;
        }

        /* c.判断鼠标是否在画板范围内 */
        if (IsOnRange(&_ptWindow->m_DrawingBoard) == DB_OK)
        {
            /* 记录图形起始信息：图形起始坐标、图形类型、图形颜色、图形线宽 */
            GarRecordBegin(&tGraVal, _ptWindow, _ptFbHandler, _ptMouseHander);
            /* 标记为图形正在更新 */
            s_u8KeyState = UPDATE;
            /* 退出程序 */
            goto RETURN;
        }
    }

    /* 6.如果上次点击在画板范围内，做左键弹起处理 */
    if (_ptMouseHander->m_u8LeftState == MY_KEY_UP && s_u8KeyState == UPDATE)   /* 左键弹起 */
    {
        /* 记录图形的结束坐标，并添加到画板的绘图链表中 */
        GarRecordEnd(&tGraVal, _ptWindow, _ptFbHandler, _ptMouseHander);
        /* 标记为图形更新完成 */
        s_u8KeyState = NO_UPDATE;
        /* 置位画板更新标志 */
        _ptWindow->m_DrawingBoard.m_u8IsRefresh = UPDATE;
    }

RETURN:
    /* 读取完按键之后，更新按键状态为未更新，说明已经对按键的更新动作进行读取 */
    _ptMouseHander->m_u8UpdateKeyState = NO_UPDATE;

    return DB_OK;
}

/**********************************************************************
* Function:         RefreshTitle
* Description:      刷新标题(根据标题链表中的选项 重新绘制标题bmp图案)
* Input:            @_ptFbHandler           fb信息句柄
*                   @_ptWindow              窗口句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           RefreshWindow函数
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 RefreshTitle(const T_FbHandler *_ptFbHandler, const T_Window *_ptWindow)
{
    /* 1.入口参数判断 */
    if (_ptFbHandler == DB_NULL || _ptWindow == DB_NULL)
    {
        printf("RefreshTitle: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    T_TitleItemNode *ptTitleItemNode = _ptWindow->m_tTitle.m_tTitleItemHead.m_ptNext;   /* 获取标题功能项链表头后的第一个节点 */

    /* 3.绘制背景色 */
    DrawFullRect(_ptFbHandler, _ptWindow->m_tTitle.m_u32TitleBegin_x, _ptWindow->m_tTitle.m_u32TitleBegin_y,
                 _ptWindow->m_tTitle.m_u32TitleEnd_x, _ptWindow->m_tTitle.m_u32TitleEnd_y, _ptWindow->m_tTitle.m_u32BackgroundColor);

    /* 4.绘制功能选项(BMP) */
    while (ptTitleItemNode != DB_NULL)
    {
        if (ptTitleItemNode->m_tTitleItem.m_u8Type == BMP)
            DrawBmp(_ptFbHandler, &ptTitleItemNode->m_tTitleItem, PART_TITLE);
        /* 移动到下一个节点 */
        ptTitleItemNode = ptTitleItemNode->m_ptNext;
    }

    return DB_OK;
}

/**********************************************************************
* Function:         RefreshMenu
* Description:      刷新菜单(根据菜单链表中的选项 重新绘制菜单bmp图案)
* Input:            @_ptFbHandler           fb信息句柄
*                   @_ptWindow              窗口句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           RefreshWindow函数
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 RefreshMenu(const T_FbHandler *_ptFbHandler, const T_Window *_ptWindow)
{
    /* 1.入口参数判断 */
    if (_ptFbHandler == DB_NULL || _ptWindow == DB_NULL)
    {
        printf("RefreshMenu: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    T_MenuItemNode *ptMenuItemNode = _ptWindow->m_tMenu.m_tMenuItemHead.m_ptNext;   /* 获取标题功能项链表头后的第一个节点 */

    /* 3.绘制背景色 */
    DrawFullRect(_ptFbHandler, _ptWindow->m_tMenu.m_u32MenuBegin_x, _ptWindow->m_tMenu.m_u32MenuBegin_y,
                 _ptWindow->m_tMenu.m_u32MenuEnd_x, _ptWindow->m_tMenu.m_u32MenuEnd_y, _ptWindow->m_tMenu.m_u32BackgroundColor);

    /* 4.绘制功能选项(BMP) */
    while (ptMenuItemNode != DB_NULL)
    {
        if (ptMenuItemNode->m_tMenuItem.m_u8Type == BMP)
            DrawBmp(_ptFbHandler, &ptMenuItemNode->m_tMenuItem, PART_MENU);
        /* 移动到下一个节点 */
        ptMenuItemNode = ptMenuItemNode->m_ptNext;
    }

    return DB_OK;
}

/**********************************************************************
* Function:         RefreshDrawBoard
* Description:      刷新画板(根据画板链表中的图案 重新绘制画板)
* Input:            @_ptFbHandler           fb信息句柄
*                   @_ptWindow              窗口句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           RefreshWindow函数
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 RefreshDrawBoard(const T_FbHandler *_ptFbHandler, const T_Window *_ptWindow)
{
    /* 1.入口参数判断 */
    if (_ptFbHandler == DB_NULL || _ptWindow == DB_NULL)
    {
        printf("RefreshDrawBoard: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    const T_GraphicalNode *ptGraphicalNode = &_ptWindow->m_DrawingBoard.GraphicalHead;   /* 获取标题功能项链表头节点 */
    DB_UINT8 u8i;   /* 遍历变量 */

    /* 3.绘制背景色 */
    DrawFullRect(_ptFbHandler, _ptWindow->m_DrawingBoard.m_u32DrawingBoardBegin_x , _ptWindow->m_DrawingBoard.m_u32DrawingBoardBegin_y,
                 _ptWindow->m_DrawingBoard.m_u32DrawingBoardEnd_x, _ptWindow->m_DrawingBoard.m_u32DrawingBoardEnd_y, _ptWindow->m_DrawingBoard.m_u32BackgroundColor);

    /* 4.遍历到链表末尾*/
    while (ptGraphicalNode->m_ptNext != DB_NULL)
        ptGraphicalNode = ptGraphicalNode->m_ptNext;

    /* 5.从后往前绘制图形(因为用了头插法插入链表，为了先画最早添加的图形，所以从后往前画) */
    for (u8i = ptGraphicalNode->m_tGraphical.m_ptDarwingBoard->m_u8GraphicalNum; u8i != 0; u8i--)
    {
        /* 选择绘制图形的类型 */
        switch (ptGraphicalNode->m_tGraphical.m_u8Type)
        {
        case LINE:
            DrawLine(_ptFbHandler, &ptGraphicalNode->m_tGraphical);
            break;
        // case RECTANGE:
        //     DrawRect(_ptFbHandler, ptGraphicalNode->m_tGraphical);
        //     break;
        // case RING:
        //     DrawCircle(_ptFbHandler, ptGraphicalNode->m_tGraphical);
        //     break;
        default:
            break;
        }

        /* 移动到上一个节点 */
        ptGraphicalNode = ptGraphicalNode->m_ptPrior;
    }

    return DB_OK;
}

/**********************************************************************
* Function:         RefreshWindow
* Description:      刷新窗口，重绘窗口及其部件图像
* Input:            @_ptFbHandler           fb信息句柄
*                   @_ptWindow              窗口句柄
* Attention:        如果某个部件没有刷新成功 只会打印错误信息 不会错误返回
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 RefreshWindow(const T_FbHandler *_ptFbHandler, const T_Window *_ptWindow)
{
    /* 1.入口参数判断 */
    if (_ptFbHandler == DB_NULL || _ptWindow == DB_NULL)
    {
        printf("RefreshWindow: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.刷新窗口部件 */
    if (RefreshTitle(_ptFbHandler, _ptWindow) == DB_ERR)
        printf("RefreshTitle fail\n");
    if (RefreshMenu(_ptFbHandler, _ptWindow) == DB_ERR)
        printf("RefreshMenu fail\n");
    if (RefreshDrawBoard(_ptFbHandler, _ptWindow) == DB_ERR)
        printf("RefreshDrawBoard fail\n");

    /* 3.返回 */
    return DB_OK;
}

/**********************************************************************
* Function:         DrawTitle
* Description:      根据标题栏的变化来绘制标题栏
* Input:            @_ptFbHandler           fb信息句柄
*                   @_ptWindow              窗口句柄
* Attention:        只改动较之前有变化的地方
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 DrawTitle(const T_FbHandler *_ptFbHandler, T_Window *_ptWindow)
{
    /* 1.入口参数合法性判断 */
    if (_ptFbHandler == DB_NULL || _ptWindow == DB_NULL)
    {
        printf("DrawMenu: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    T_TitleItemNode *ptTitleItemNode; /* 临时标题节点变量 */

    /* 3.获取菜单选项头节点 */
    ptTitleItemNode = &_ptWindow->m_tTitle.m_tTitleItemHead;

    /* 4.遍历到链表末尾把需要刷新的进行刷新 */
    while (ptTitleItemNode->m_ptNext != DB_NULL)
    {
        /* 移动的下一个节点 */
        ptTitleItemNode = ptTitleItemNode->m_ptNext;
        if (ptTitleItemNode->m_tTitleItem.m_u8IsRefresh == REFRESH)
        {
            if (ptTitleItemNode->m_tTitleItem.m_u8Type == BMP)
            {
                /* 修改图形标识 */
                DrawBmp(_ptFbHandler, &ptTitleItemNode->m_tTitleItem, PART_TITLE);
                if (ptTitleItemNode->m_tTitleItem.m_u8IsClicked == CLICKED)
                {
                    /* 根据对应功能来选择对应的处理函数 */
                    switch (ptTitleItemNode->m_tTitleItem.m_u8MenuFunction)
                    {
                    case CLOSE:
                        g_bMainIsClose = DB_TRUE;
                        break;
                    
                    default:
                        break;
                    }
                    ptTitleItemNode->m_tTitleItem.m_u8IsClicked = NO_CLICKED; /* 设置未点击 */
                }
            }
            ptTitleItemNode->m_tTitleItem.m_u8IsRefresh = NO_REFRESH;    /* 设置为不需要刷新 */
        }
    }
    return DB_OK;
}

/**********************************************************************
* Function:         DrawGar
* Description:      根据画板的更新来绘制图案
* Input:            @_ptFbHandler           fb信息句柄
*                   @_ptWindow              窗口句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           DarwWindow函数
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 DrawGar(const T_FbHandler *_ptFbHandler, T_Window *_ptWindow)
{
    /* 1.入口参数判断 */
    if (_ptFbHandler == DB_NULL || _ptWindow == DB_NULL)
    {
        printf("DrawGar: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    T_GraphicalNode *ptGraphical;   /* 临时图形节点变量 */

    /* 3.获取画板链表头后的第一个节点 */
    ptGraphical = _ptWindow->m_DrawingBoard.GraphicalHead.m_ptNext;
    
    /* 4.按照类型进行绘图 */
    if (ptGraphical != DB_NULL)
    {
        /* 选择绘制图形的类型 */
        switch (ptGraphical->m_tGraphical.m_u8Type)
        {
        case LINE:
            DrawLine(_ptFbHandler, &ptGraphical->m_tGraphical);
            break;
        // case RECTANGE:
        //     DrawRect(_ptFbHandler, ptGraphical->m_tGraphical);
        //     break;
        // case RING:
        //     DrawCircle(_ptFbHandler, ptGraphical->m_tGraphical);
        //     break;
        default:

            break;
        }
    }

    return DB_OK;
}

/**********************************************************************
* Function:         DrawMenu
* Description:      根据菜单的更新来绘制菜单
* Input:            @_ptFbHandler           fb信息句柄
*                   @_ptWindow              窗口句柄
* Attention:        调用该函数会修改全局窗口变量中画板链表头的参数，用来设
*                   置当前选中的是哪一种绘图模式(线条,圆环,矩形)
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           DarwWindow函数
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 DrawMenu(const T_FbHandler *_ptFbHandler, T_Window *_ptWindow)
{
    /* 1.入口参数合法性判断 */
    if (_ptFbHandler == DB_NULL || _ptWindow == DB_NULL)
    {
        printf("DrawMenu: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    T_MenuItemNode *ptMenuItemNode; /* 临时菜单节点变量 */

    /* 3.获取菜单选项头节点 */
    ptMenuItemNode = &_ptWindow->m_tMenu.m_tMenuItemHead;

    /* 4.遍历到链表末尾把需要刷新的进行刷新 */
    while (ptMenuItemNode->m_ptNext != DB_NULL)
    {
        /* 移动的下一个节点 */
        ptMenuItemNode = ptMenuItemNode->m_ptNext;
        if (ptMenuItemNode->m_tMenuItem.m_u8IsRefresh == REFRESH)
        {
            if (ptMenuItemNode->m_tMenuItem.m_u8Type == BMP)
            {
                /* 修改图形标识 */
                DrawBmp(_ptFbHandler, &ptMenuItemNode->m_tMenuItem, PART_MENU);
                if (ptMenuItemNode->m_tMenuItem.m_u8IsClicked == CLICKED)
                {
                    if (ptMenuItemNode->m_tMenuItem.m_u8MenuFunction == CLEAN)
                    {
                        CleanGarItems(&_ptWindow->m_DrawingBoard);  /* 清空画板节点 */
                        RefreshDrawBoard(_ptFbHandler, _ptWindow);  /* 刷新画板 */
                    }
                    /* 修改对应全局画板链表头中的图形类型 */
                    _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u8Type = ptMenuItemNode->m_tMenuItem.m_u8MenuFunction;
                }
            }
            ptMenuItemNode->m_tMenuItem.m_u8IsRefresh = NO_REFRESH;    /* 设置为不需要刷新 */
        }
    }

    return DB_OK;
}

/**********************************************************************
* Function:         DarwWindow
* Description:      根据部件或者窗口标志位时候更新，来进行窗口的绘制
* Input:            @_ptFbHandler           fb信息句柄
*                   @_ptWindow              窗口句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 DarwWindow(const T_FbHandler *_ptFbHandler, T_Window *_ptWindow)
{
    /* 1.入口参数判断 */
    if (_ptWindow == DB_NULL)
    {
        printf("DarwWindow: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.整个窗口是否需要刷新 */
    if (_ptWindow->m_u8IsRefresh == UPDATE)
    {
        RefreshWindow(_ptFbHandler, _ptWindow);             /* 刷新窗口 */
        _ptWindow->m_u8IsRefresh = NO_UPDATE;               /* 清除窗口刷新标志位 */
        _ptWindow->m_tTitle.m_u8IsRefresh = NO_UPDATE;      /* 清除标题栏刷新标志位 */
        _ptWindow->m_DrawingBoard.m_u8IsRefresh = NO_UPDATE;/* 清除画板刷新标志位 */
        _ptWindow->m_tMenu.m_u8IsRefresh = NO_UPDATE;       /* 清除菜单刷新标志位 */
    }

    /* 2.标题需要更新 */
    if (_ptWindow->m_tTitle.m_u8IsRefresh == UPDATE)
    {
        DrawTitle(_ptFbHandler, _ptWindow);                 /* 绘制标题 */
        _ptWindow->m_tTitle.m_u8IsRefresh = NO_UPDATE;      /* 清除标题栏刷新标志位 */
    }

    /* 3.画板需要更新 */
    if (_ptWindow->m_DrawingBoard.m_u8IsRefresh == UPDATE)
    {
        DrawGar(_ptFbHandler, _ptWindow);                   /* 绘制画板图案 */
        _ptWindow->m_DrawingBoard.m_u8IsRefresh = NO_UPDATE;/* 清除画板刷新标志位 */
    }

    /* 4.菜单栏需要更新 */
    if (_ptWindow->m_tMenu.m_u8IsRefresh == UPDATE)
    {
        DrawMenu(_ptFbHandler, _ptWindow);                  /* 绘制菜单 */
        _ptWindow->m_tMenu.m_u8IsRefresh = NO_UPDATE;       /* 清除菜单刷新标志位 */
    }

    /* 设置鼠标下次更新不需要还原 */
    return DB_OK;
}

/**********************************************************************
* Function:         SetDefaultGarVal
* Description:      设置图形的默认属性
* Input:            @_ptWindow              窗口句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Attention:        本程序用全局窗口链表的画板的头结点属性(图形类型、颜色、线宽)做全局的图形参数的参考
*                   用来表示当前需要绘制的图形类型、线宽、颜色。设置不同的绘图类型时，该全局变量也会改
*                   变，此处只是设置一个默认的初始值。
* Fan-in:           SetWindow函数
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 SetDefaultGarVal(T_Window *_ptWindow)
{
    /* 1.入口参数判断 */
    if (_ptWindow == DB_NULL)
    {
        printf("SetDefaultGarVal: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.设置默认图形属性 */
    _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u8Type = DEFAULT_TYPE;
    _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u8Width = DEFAULT_WIDTH;
    _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u32Color_32 = DEFAULT_COLOR;
    _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u16Color_16 = RGB888ToRGB565(DEFAULT_COLOR);

    return DB_OK;
}

/**********************************************************************
* Function:         GetXmlInfo
* Description:      读取xml文件,并把文件中菜单信息添加到全局链表中
* Input:            @_ptWindow              窗口句柄
*                   @_strXmlPath            xml文件的路径           
* Output:           参数合法性判断：终端打印输出错误信息
* Fan-in:           SetWindow函数
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
static DB_INT32 GetXmlInfo(T_Window *_ptWindow, DB_CHAR *_strXmlPath)
{
    /* 1.入口参数判断 */
    if (_ptWindow == DB_NULL || _strXmlPath == DB_NULL)
    {
        printf("GetXmlInfo: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    FILE *fp;                               /* 文件流指针 */
    mxml_node_t *ptTree;                    /* xml文件树指针 */
    mxml_node_t *ptNode;                    /* xml元素节点指针 */
    const DB_CHAR *pcDefault;               /* 默认菜单资源路径 */
    const DB_CHAR *pcClicked;               /* 选中时菜单资源路径 */
    const DB_CHAR *pcType;                  /* 菜单类型(获取xml信息用) */
    DB_UINT8 u8Type = CLEAN;                /* 菜单的类型(调用AddMenuItem用) */

    /* 3.打开文件 */
    fp = fopen(_strXmlPath, "r");             /* 打开文件 */
    if (fp == DB_NULL)
    {
        perror("GetXmlInfo: fopen fail");
        return DB_ERR;
    }
    ptTree = mxmlLoadFile(NULL, fp, NULL);/* 获取文件信息 */
    if (ptTree == DB_NULL)
    {
        perror("GetXmlInfo: mxmlLoadFile fail");
        return DB_ERR;
    }
    fclose(fp);                             /* 关闭文件 */

    /* 4.遍历xml树 */
    /* 第三个参数表示节点名，第四个关键字，第五个值 */
    for (ptNode = mxmlFindElement(ptTree, ptTree, "node", NULL, NULL, MXML_DESCEND);
                    ptNode != NULL;
                    ptNode = mxmlFindElement(ptNode, ptTree, "node", NULL, NULL, MXML_DESCEND))
    {
        /* 获取字段数据 */
        pcDefault = mxmlElementGetAttr(ptNode, "Default");
        pcClicked = mxmlElementGetAttr(ptNode, "Clicked");
        pcType = mxmlElementGetAttr(ptNode, "Type");
        //printf("cDefault = %s, cSelect = %s pcType = %s\n", pcDefault, pcSelect, pcType);
        /* 获取类型 */
        if(!strcmp(pcType, "LINE"))
            u8Type = LINE;
        else if (!strcmp(pcType, "RECTANGE"))
            u8Type = RECTANGE;
        else if (!strcmp(pcType, "RING"))
            u8Type = RING;
        else
            u8Type = CLEAN;
        /* 添加节点 */
        if (AddMenuItem(&_ptWindow->m_tMenu, u8Type, pcDefault, pcClicked) == DB_ERR)
            printf("GetXmlInfo: AddMenuItem fail\n");
    }

    /* 5.删除xml文件缓存 */
    mxmlDelete(ptTree);
    return DB_OK;
}

/**********************************************************************
* Function:         SetWindow
* Description:      初始化设置窗口(结构体初始化、添加窗口部件、设置图形的默认属性)
* Input:            @_ptFbHandler           fb信息句柄
*                   @_ptWindow              窗口句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 SetWindow(const T_FbHandler *_ptFbHandler, T_Window *_ptWindow)
{
    /* 1.入口参数判断 */
    if (_ptWindow == DB_NULL || _ptWindow == DB_NULL)
    {
        printf("SetWindow: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.初始化窗口 */
    InitWindow(_ptWindow, WINDOW_SIZE_X, WINDOW_SIZE_Y);

    /* 3.添加标题功能项 */
    AddTitleItem(&_ptWindow->m_tTitle, CLOSE, BMP_CLOSE_PATH_DEFAULT, BMP_CLOSE_PATH_SELECT);

    /* 4.添加菜单项 */
    /* 判断xml文件是否存在 */
    if (access(MENU_XML_PATH, F_OK) == DB_OK)
    {
        GetXmlInfo(_ptWindow, MENU_XML_PATH);
    }
    else
    {
        if (AddMenuItem(&_ptWindow->m_tMenu, LINE, BMP_LINE_PATH_DEFAULT, BMP_LINE_PATH_SELECT) == DB_ERR)
            printf("AddMenuOptions line fail\n");
        if (AddMenuItem(&_ptWindow->m_tMenu, RING, BMP_YUAN_PATH_DEFAULT, BMP_YUAN_PATH_SELECT) == DB_ERR)
            printf("AddMenuOptions yuan fail\n");
        if (AddMenuItem(&_ptWindow->m_tMenu, RECTANGE, BMP_RECTANGE_PATH_DEFAULT, BMP_RECTANGE_PATH_SELECT) == DB_ERR)
            printf("AddMenuOptions rectange fail\n");
        if (AddMenuItem(&_ptWindow->m_tMenu, CLEAN, BMP_CLEAN_PATH_DEFAULT, BMP_CLEAN_PATH_SELECT) == DB_ERR)
            printf("AddMenuOptions clean fail\n");
    }

    /* 5.设置图形默认属性 */
    SetDefaultGarVal(_ptWindow);

    /* 6.刷新显示窗口 */
    RefreshWindow(_ptFbHandler, _ptWindow);

    return DB_OK;
}

/**********************************************************************
* Function:         CloseWindow
* Description:      关闭窗口，做窗口申请资源的释放
* Input:            @_ptWindow              窗口句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 CloseWindow(T_Window *_ptWindow)
{
    /* 1.入口参数合法性判断 */
    if (_ptWindow == DB_NULL)
    {
        printf("CloseWindow: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.清空窗口申请的节点 */
    CleanGarItems(&_ptWindow->m_DrawingBoard);  /* 清空图形节点 */
    CleanMenuItems(&_ptWindow->m_tMenu);        /* 清空菜单项 */
    CleanTitleItems(&_ptWindow->m_tTitle);      /* 清空标题项 */

    /* 3.返回 */
    return DB_OK;
}

/**********************************************************************
* Function:         GetTerminalInput
* Description:      获取终端输入的信息，改变全局的图形属性
* Input:            @_ptWindow              窗口句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 GetTerminalInput(T_Window *_ptWindow)
{
    /* 1.入口参数合法性判断 */
    if (_ptWindow == DB_NULL)
    {
        printf("GetTerminalInput: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.局部静态变量，用于标识选中状态 */
    static DB_UINT8 s_u8ColorState = NO_SELECT;
    static DB_UINT8 s_u8WidthState = NO_SELECT;
    static DB_UINT8 s_u8ColorStart = 0;            /* 开始记录颜色标志位 */
    static DB_UINT8 s_uWidthStart = 0;            /* 开始记录宽度标志位 */
    static DB_UINT8 s_u8RecordCount = 0;           /* 记录终端输入的次数 */
    static DB_UINT8 s_u8RecordNum = 0;             /* 记录终端输入的数字 */
    static DB_UINT32 u32TempWidth;              /* 宽度值 */  
    static DB_UINT32 u32TempColor;              /* 颜色值 */    
    DB_CHAR cReceiveChar;

    /* 3.获取字符 */
    while (1)
    {
        cReceiveChar = getchar();
        // printf("接收：%d\n",cReceiveChar);
        if (s_u8WidthState == SELECT)           /* 获取宽度处理 */
        {
            if (cReceiveChar >= '0' && cReceiveChar <= '9' && s_u8RecordCount < 2)
            {
                s_u8RecordNum = cReceiveChar - '0';                 /* 获取输入值大小 */
                u32TempWidth = u32TempWidth * 10 + s_u8RecordNum;   /* 设置宽度值 */
                s_uWidthStart = START;                              /* 宽度是否开始记录标志 */
                s_u8RecordCount++;                                  /* 接收计数+1 */
            }
            /* 记录完成 */
            if (s_uWidthStart == START && cReceiveChar == ENTER)    /* 计数开始并且按下Enter按键完成记录 */
            {   
                if (u32TempWidth > MAX_WIDTH)                      /* 判断是否符合最大宽度 */
                    _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u8Width = MAX_WIDTH;
                else
                    _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u8Width = u32TempWidth;/* 设置宽度值 */
                printf("u32TempWidth: %d\n", _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u8Width);
                u32TempWidth = 0;                       /* 宽度信息清0 */
                s_u8WidthState = NO_SELECT;             /* 修改为未选中状态 */
                s_u8RecordCount = 0;                    /* 接收计数改为0 */
                s_uWidthStart = NO_START;               /* 修改为没有开始记录状态 */
                goto RET_REFRESH_OK;
            }
        }

        if (s_u8ColorState == SELECT)           /* 获取长度处理 */
        {
            if (cReceiveChar >= '0' && cReceiveChar <= '9')
            {
                s_u8RecordNum = cReceiveChar - '0';                 /* 获取输入值大小 */
                s_u8ColorStart = START;                             /* 颜色是否开始记录标志 */
                if (s_u8RecordCount < 8 &&cReceiveChar != ENTER)
                {
                    u32TempColor = u32TempColor * 16 + s_u8RecordNum;
                    s_u8RecordCount++;  /* 接收计数+1 */
                }
            }
            if(cReceiveChar >= 'a' && cReceiveChar <= 'f')
            {

                s_u8RecordNum = cReceiveChar + 10 - 'a';            /* 获取输入值大小 */
                s_u8ColorStart = START;                             /* 颜色是否开始记录标志 */
                if (s_u8RecordCount < 8 &&cReceiveChar != ENTER)
                {
                    u32TempColor = u32TempColor * 16 + s_u8RecordNum;
                    s_u8RecordCount++;  /* 接收计数+1 */
                }
            }

            /* 记录完成 */
            if (s_u8ColorStart == START && cReceiveChar == ENTER)       /* 0x0d 即回车 */
            {   
                _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_u32Color_32 = u32TempColor;   /* 设置颜色值 */
                printf("u32TempColor: %x\n", u32TempColor);
                u32TempColor = 0;                           /* 颜色信息清0 */
                s_u8ColorState = NO_SELECT;                 /* 修改为未选中状态 */
                s_u8RecordCount = 0;                        /* 接收计数改为0 */
                s_u8ColorStart = NO_START;                  /* 修改为没有开始记录状态 */
                goto RET_REFRESH_OK;
            }
        }

        /* 根据终端输入颜色及宽度 */
        switch(cReceiveChar)
        {
        case SET_COLOR:
            /* 此处没有判断16位颜色数据还是32位颜色数据 */
            s_u8ColorState = SELECT;    /* 修改为选中状态 */
            s_u8WidthState = NO_SELECT; /* 修改为未选中状态 */
            if (s_uWidthStart == START)
            {
                u32TempWidth = 0;                       /* 宽度清0 */
                s_u8RecordCount = 0;                    /* 接收计数改为0 */
                s_uWidthStart = NO_START;               /* 修改为没有开始记录状态 */
            }
            break;
        case SET_WIDTH:
            s_u8WidthState = SELECT;    /* 修改为选中状态 */
            s_u8ColorState = NO_SELECT; /* 修改为未选中状态 */
            if (s_u8ColorStart == START)
            {
                u32TempColor = 0;                           /* 颜色信息清0 */
                s_u8RecordCount = 0;                        /* 接收计数改为0 */
                s_u8ColorStart = NO_START;                  /* 修改为没有开始记录状态 */
            }
            break;
        default:
            if (cReceiveChar == ENTER)
            {
                goto RET_REFRESH_ERR;
            }
            break;
        }
    }
RET_REFRESH_OK:
    RefreshWindow(&g_tFbInfo, &g_tWindow);
    return DB_OK;
RET_REFRESH_ERR:
    RefreshWindow(&g_tFbInfo, &g_tWindow);
    return DB_ERR;
}
