#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "ui.h"
 

/**********************************************************************
* Function:         InitWindow
* Description:      初始化窗口 填充窗口结构体的成员信息(每个模块的坐标尺寸)
* Input:            @_ptWindow              窗口句柄
*                   @_u32WindowMax_X        窗口横向尺寸
*                   @_u32WindowMax_Y        窗口纵向尺寸
* Attention:        对于窗口的初始位置可同归修改ui.h里定义的宏来修改
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 InitWindow(T_Window *_ptWindow, const DB_UINT32 _u32WindowMax_X, const DB_UINT32 _u32WindowMax_Y)
{
    /* 1.入口参数判断 */
    if (_ptWindow == DB_NULL)
    {
        printf("InitWindow:Invalid Parameter(Null pointer)\n");
        return DB_ERR;
    }
    if (_u32WindowMax_X > LIMIT_WINDOW_MAX_X || _u32WindowMax_Y > LIMIT_WINDOW_MAX_Y)
    {
        printf("InitWindow:Invalid Parameter(parameter is too large)\n");
        return DB_ERR;
    }

    if (_u32WindowMax_X < LIMIT_WINDOW_MIN_X || _u32WindowMax_Y < LIMIT_WINDOW_MIN_Y)
    {
        printf("InitWindow:Invalid Parameter(parameter is too small)\n");
        return DB_ERR;
    }


    /* 2.设置窗口信息 */
    /* 设置起始和结束坐标 */
    _ptWindow->m_u32WindowBegin_x = DEFAULT_WINDOW_BEGIN_X;                     /* 设置起始坐标 */
    _ptWindow->m_u32WindowBegin_y = DEFAULT_WINDOW_BEGIN_Y;
    _ptWindow->m_u32WindowEnd_x = DEFAULT_WINDOW_BEGIN_X + _u32WindowMax_X - 1; /* 起始坐标从0开始,故结束坐标应为输入尺寸-1 */
    _ptWindow->m_u32WindowEnd_y = DEFAULT_WINDOW_BEGIN_Y + _u32WindowMax_Y - 1;
    /* 设置部件类型 */
    _ptWindow->m_u32PartType = PART_MAIN_WINDOW;

    /* 3.设置标题信息 */
    /* 设置起始结束坐标 */
    _ptWindow->m_tTitle.m_u32TitleBegin_x = _ptWindow->m_u32WindowBegin_x;
    _ptWindow->m_tTitle.m_u32TitleBegin_y = _ptWindow->m_u32WindowBegin_y;
    _ptWindow->m_tTitle.m_u32TitleEnd_x = _ptWindow->m_u32WindowEnd_x;
    _ptWindow->m_tTitle.m_u32TitleEnd_y = _ptWindow->m_u32WindowBegin_y + TITLE_BAR_HEIGHT - 1;
    /* 设置标题栏的父对象 */
    _ptWindow->m_tTitle.m_ptWindow = _ptWindow;
    /* 设置标题栏头节点父对象 */
    _ptWindow->m_tTitle.m_tTitleItemHead.m_tTitleItem.m_ptTitle = &_ptWindow->m_tTitle;
    /* 设置标题栏标题功能选项头节点指针指向NULL */
    _ptWindow->m_tTitle.m_tTitleItemHead.m_ptNext = DB_NULL;
    _ptWindow->m_tTitle.m_tTitleItemHead.m_ptPrior = DB_NULL;
    /* 设置标题栏颜色 */
    _ptWindow->m_tTitle.m_u32BackgroundColor = DEFAULT_TITLE_COLOR;
    /* 设置标题栏功能项数量 */
    _ptWindow->m_tTitle.m_u8TitleItemNum = 0;
    /* 设置为不需要刷新 */
    _ptWindow->m_tTitle.m_u8IsRefresh = NO_REFRESH;
    /* 设置部件类型 */
    _ptWindow->m_tTitle.m_u32PartType = PART_TITLE;

    /* 4.设置菜单栏信息 */
    /* 设置起始结束坐标 */
    _ptWindow->m_tMenu.m_u32MenuBegin_x = _ptWindow->m_u32WindowBegin_x;
    _ptWindow->m_tMenu.m_u32MenuBegin_y = _ptWindow->m_tTitle.m_u32TitleEnd_y + 1;
    _ptWindow->m_tMenu.m_u32MenuEnd_x = _ptWindow->m_u32WindowEnd_x;
    _ptWindow->m_tMenu.m_u32MenuEnd_y = _ptWindow->m_tMenu.m_u32MenuBegin_y + MENU_ITEN_HEIGHT - 1;
    /* 设置菜单栏的父对象 */
    _ptWindow->m_tMenu.m_ptWindow = _ptWindow;
    /* 设置菜单栏头节点父对象 */
    _ptWindow->m_tMenu.m_ptWindow->m_tMenu.m_tMenuItemHead.m_tMenuItem.m_ptMenu = &_ptWindow->m_tMenu;
    /* 设置菜单栏菜单头节点指针指向NULL */
    _ptWindow->m_tMenu.m_tMenuItemHead.m_ptNext = DB_NULL;
    _ptWindow->m_tMenu.m_tMenuItemHead.m_ptPrior = DB_NULL;
    /* 设置菜单栏颜色 */
    _ptWindow->m_tMenu.m_u32BackgroundColor = DEFAULT_MENU_COLOR;
    /* 设置初始菜单项数量 */
    _ptWindow->m_tMenu.m_u8MenuItemNum = 0;
    /* 设置为不需要刷新 */
    _ptWindow->m_tMenu.m_u8IsRefresh = NO_REFRESH;
    /* 设置部件类型 */
    _ptWindow->m_tMenu.m_u32PartType = PART_MENU;

    /* 5.设置画板信息*/
    /* 设置起始结束坐标 */
    _ptWindow->m_DrawingBoard.m_u32DrawingBoardBegin_x = _ptWindow->m_u32WindowBegin_x;
    _ptWindow->m_DrawingBoard.m_u32DrawingBoardBegin_y = _ptWindow->m_tMenu.m_u32MenuEnd_y + 1; 
    _ptWindow->m_DrawingBoard.m_u32DrawingBoardEnd_x =  _ptWindow->m_u32WindowEnd_x;
    _ptWindow->m_DrawingBoard.m_u32DrawingBoardEnd_y = _ptWindow->m_u32WindowEnd_y;
    /* 设置画板的父对象 */
    _ptWindow->m_DrawingBoard.m_ptWindow = _ptWindow;
    /* 设置画板头节点父对象 */
    _ptWindow->m_DrawingBoard.GraphicalHead.m_tGraphical.m_ptDarwingBoard = &_ptWindow->m_DrawingBoard;
    /* 设置画板上画板头节点指针指向NULL */
    _ptWindow->m_DrawingBoard.GraphicalHead.m_ptNext = DB_NULL;
    _ptWindow->m_DrawingBoard.GraphicalHead.m_ptPrior = DB_NULL;
    /* 设置画板背景颜色 */
    _ptWindow->m_DrawingBoard.m_u32BackgroundColor = DEFAULT_DRAW_COLOR;
    /* 设置画板上图形数量 */
    _ptWindow->m_DrawingBoard.m_u8GraphicalNum = 0;
    /* 设置为不需要刷新 */
    _ptWindow->m_DrawingBoard.m_u8IsRefresh = NO_REFRESH;
    /* 设置部件类型 */
    _ptWindow->m_DrawingBoard.m_u32PartType = PART_DRAWING_BOARD;

    #if DISABLE
    printf("窗口起始坐标：%d, %d\n", _ptWindow->m_u32WindowBegin_x, _ptWindow->m_u32WindowBegin_y);
    printf("窗口结束坐标：%d, %d\n", _ptWindow->m_u32WindowEnd_x, _ptWindow->m_u32WindowEnd_y);
    printf("标题栏起始坐标：%d,%d\n", _ptWindow->m_tTitle.m_u32TitleBegin_x, _ptWindow->m_tTitle.m_u32TitleBegin_y);
    printf("标题栏结束坐标：%d,%d\n", _ptWindow->m_tTitle.m_u32TitleEnd_x, _ptWindow->m_tTitle.m_u32TitleEnd_y);
    printf("菜单栏起始坐标：%d, %d\n", _ptWindow->m_tMenu.m_u32MenuBegin_x, _ptWindow->m_tMenu.m_u32MenuBegin_y);
    printf("菜单栏结束坐标：%d, %d\n", _ptWindow->m_tMenu.m_u32MenuEnd_x, _ptWindow->m_tMenu.m_u32MenuEnd_y);
    printf("画板起始坐标：%d, %d\n", _ptWindow->m_DrawingBoard.m_u32DrawingBoardBegin_x, _ptWindow->m_DrawingBoard.m_u32DrawingBoardBegin_y);
    printf("画板结束坐标：%d, %d\n", _ptWindow->m_DrawingBoard.m_u32DrawingBoardEnd_x, _ptWindow->m_DrawingBoard.m_u32DrawingBoardEnd_y);
    #endif
    return DB_OK;
}

/**********************************************************************
* Function:         GetBmpResource
* Description:      获取图片资源 并加载到内存中
* Input:            @_strResources          图片资源位置
*                   @_u8pBitCount           每个像素点的位数
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           内存中资源位置           成功
*                   DB_NULL                 失败
**********************************************************************/
static DB_UINT8* GetBmpResource(const DB_CHAR *_strResources, DB_UINT8 *_u8pBitCount)
{    
    /* 1.入口参数判断 */
    if (_strResources == DB_NULL)
    {
        printf("GetBmpResource: Invalid parameter(Null pointer)\n");
        return DB_NULL;
    }

    /* 2.定义局部变量 */
    DB_INT32 s32Fd = DB_ERR;                    /* 文件描述符 */
    DB_UINT8 *u8ResourcesAddr = DB_NULL;        /* 资源地址 */
    struct BmpFileHeader tFileHead;             /* bmp文件头信息     在后面进行赋值 */
    struct BmpInfoHeader tInfoHead;             /* bmp位图信息       在后面进行赋值 */ 

    /* 3.打开文件 */
    s32Fd = open(_strResources, O_RDONLY);/* 打开图片 */
    if (s32Fd == DB_ERR)
    {
       perror("GetBmpResource: Cannot open Bmp picture");
       return DB_NULL;
    }

    /* 4.获取bmp文件头信息 */
    if (read(s32Fd, &tFileHead, sizeof(struct BmpFileHeader)) == DB_ERR)
    {
        close(s32Fd);    /* 关闭文件描述符 */
        perror("GetBmpResource: Error read BmpFileHeader");
        return DB_NULL;
    } 

    /* 5.判断是否是Bmp格式文件(字符 0x42(B) 0x42(M)) */
    if (tFileHead.m_u16Type != 0x4d42)
    {
        close(s32Fd);    /* 关闭文件描述符 */
        printf("GetBmpResource: Not in BMP format\n");
        return DB_NULL;
    }

    /* 6.获取bmp位图信息 */
    if (read(s32Fd, &tInfoHead, sizeof(struct BmpInfoHeader)) == DB_ERR)
    {
        close(s32Fd);    /* 关闭文件描述符 */
        perror("GetBmpResource: Error read BmpInfoHeader");
        return DB_NULL;
    } 

    #if DISABLE
    /* 打印图片信息 */
    printf("文件头:\n");
    printf(" 文件类型:m_u16Type = %x\n 文件大小:m_u32Size = %d\n 四个字节保留\n 图像数据偏移:m_u32OffBits = %d\n",
            tFileHead.m_u16Type, tFileHead.m_u32Size, tFileHead.m_u32OffBits);
    printf("位图信息:\n");   
    printf(" 信息头大小:m_u32Size = %d\n 位图的高度:m_s32Height = %d\n 位图的宽度:m_s32Width = %d\n 位图plane:总是1\n 每个像素的位数:m_u16BitCount = %d\n 压缩说明:m_u32Compression = %d\n",
            tInfoHead.m_u32Size, tInfoHead.m_s32Height, tInfoHead.m_s32Width, tInfoHead.m_u16BitCount, tInfoHead.m_u32Compression);
    printf(" 图像大小:m_u32SizeImage = %d\n 水平分辨率:m_s32x_pels_per_meter = %d\n 垂直分辨率:m_s32y_pels_per_meter = %d\n 位图使用的颜色数:m_u32ClrUsed = %d\n 指定重要的颜色数:m_u32ClrImportant = %d\n",
            tInfoHead.m_u32SizeImage, tInfoHead.m_s32x_pels_per_meter, tInfoHead.m_s32y_pels_per_meter, tInfoHead.m_u32ClrUsed, tInfoHead.m_u32ClrImportant);
    #endif
    
    /* 7.设置资源每个像素点的大小(字节)*/
    *_u8pBitCount = tInfoHead.m_u16BitCount;

    /* 8.申请存放颜色数据缓冲区 */
    u8ResourcesAddr = malloc(tInfoHead.m_u32SizeImage);
    if (u8ResourcesAddr == DB_NULL)
    {
        printf("GetBmpResource: u8pBmpBuffer malloc fail\n");
        return DB_NULL;
    }

    /* 9.读取bmp文件中的数据到缓存中 */
    if(read(s32Fd, u8ResourcesAddr, tInfoHead.m_u32SizeImage) == DB_ERR)
    {
        free(u8ResourcesAddr);
        close(s32Fd);
        printf("GetBmpResource: read bmp fail\n");
        return DB_NULL;
    }

    /* 10.关闭文件描述符 */
    close(s32Fd);

    return u8ResourcesAddr;
}

/**********************************************************************
* Function:         AddGarItem
* Description:      向画板链表中添加节点
* Input:            @_ptGraphical           图形节点
*                   @_ptDarwingBoard        画板句柄
* Attention:        该函数会向画板链表中添加图形，并且将刷新标志位置位
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 AddGarItem(const T_Graphical *_ptGraphical, T_DarwingBoard *_ptDarwingBoard)
{
    /* 1.入口参数判断 */
    if (_ptGraphical == DB_NULL || _ptDarwingBoard == DB_NULL)
    {
        printf("AddGarItem: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.新建图形节点 */
    /* 申请空间 */
    T_GraphicalNode *ptGraphicalNode;
    ptGraphicalNode = malloc(sizeof(T_GraphicalNode));
    if (ptGraphicalNode == DB_NULL)
    {
        printf("AddGarItem: malloc fail(default)\n");
        return DB_ERR;
    }
    /* 添加图像信息 */
    ptGraphicalNode->m_tGraphical = *_ptGraphical;

    /* 3.设置图形节点的父对象 */
    ptGraphicalNode->m_tGraphical.m_ptDarwingBoard = _ptDarwingBoard;

    /* 4.将节点加入画板的图形链表中 */
    ptGraphicalNode->m_ptNext = _ptDarwingBoard->GraphicalHead.m_ptNext;        /* 新节点后继指向头节点的下个节点 */
    ptGraphicalNode->m_ptPrior = &_ptDarwingBoard->GraphicalHead;               /* 新节点前驱指向头节点 */
    _ptDarwingBoard->GraphicalHead.m_ptNext = ptGraphicalNode;                  /* 头节点后继指向新节点 */
    if (ptGraphicalNode->m_ptNext != DB_NULL)
        ptGraphicalNode->m_ptNext->m_ptPrior = ptGraphicalNode;                 /* 下个节点的前驱指向新节点 */

    /* 5.图形数量+1 */
    _ptDarwingBoard->m_u8GraphicalNum++;

    /* 6.置位刷新标志位 */
    _ptDarwingBoard->m_u8IsRefresh = REFRESH;

    return DB_OK;
}

/**********************************************************************
* Function:         CleanGarItems
* Description:      清空画板的图形
* Input:            @_ptDarwingBoard        画板句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 CleanGarItems(T_DarwingBoard *_ptDarwingBoard)
{
    /* 1.入口参数判断 */
    if (_ptDarwingBoard == DB_NULL)
    {
        printf("CleanGarItems: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    T_GraphicalNode *ptGraphicalNode;           /* 图形节点，遍历用 */
    T_GraphicalNode *ptFreeGraphicalNode;       /* 图形节点，释放用 */

    /* 3.获取链表头的下一个节点 */
    ptGraphicalNode = _ptDarwingBoard->GraphicalHead.m_ptNext;

    /* 4.链表头指向空 */
    _ptDarwingBoard->GraphicalHead.m_ptNext = DB_NULL;

    /* 5.清除节点 */
    while (ptGraphicalNode != DB_NULL)
    {
        /* 获取当前节点，准备释放用 */
        ptFreeGraphicalNode = ptGraphicalNode;
        /* 移动到下一个 */
        ptGraphicalNode = ptGraphicalNode->m_ptNext;
        /* 释放节点 */
        free(ptFreeGraphicalNode);
    }

    /* 6.清空节点数量 */
    _ptDarwingBoard->m_u8GraphicalNum = 0;

    return DB_OK;
}

/**********************************************************************
* Function:         AddMenuItem
* Description:      向菜单链表中添加菜单节点
* Input:            @_ptMenu                菜单
*                   @_u8Type                菜单功能类型
*                   @_strResources_default  默认资源位置
*                   @_strResources_Clicked  点击时资源位置
* Attention:        该函数会向菜单链表中添加图形，并且将刷新标志位置位
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 AddMenuItem(T_Menu *_ptMenu, DB_UINT8 _u8Type, const DB_CHAR *_strResources_default, const DB_CHAR *_strResources_Clicked)
{
    /* 1.入口参数判断 */
    if (_strResources_default == DB_NULL || _strResources_Clicked == DB_NULL || _ptMenu == DB_NULL)
    {
        printf("AddMenuItem: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.申请节点空间 */
    T_MenuItemNode *ptMenuItemNode;                 /*节点指针变量, 用于申请空间和赋值时使用*/

    ptMenuItemNode = malloc(sizeof(T_MenuItemNode));
    if (ptMenuItemNode == DB_NULL)
    {
        perror("AddMenuItem:malloc fail");
        return DB_ERR;
    }

    /* 3.设置节点父对象 */
    ptMenuItemNode->m_tMenuItem.m_ptMenu = _ptMenu;

    /* 4.将节点加入菜单项链表中 */
    ptMenuItemNode->m_ptNext = _ptMenu->m_tMenuItemHead.m_ptNext;      /* 新节点后继指向头节点的下个节点 */
    ptMenuItemNode->m_ptPrior = &_ptMenu->m_tMenuItemHead;             /* 新节点前驱指向头节点 */
    _ptMenu->m_tMenuItemHead.m_ptNext = ptMenuItemNode;                /* 头节点后继指向新节点 */
    if (ptMenuItemNode->m_ptNext != DB_NULL)
        ptMenuItemNode->m_ptNext->m_ptPrior = ptMenuItemNode;          /* 下个节点的前驱指向新节点 */

    /* 5.设置节点菜单项属性 */
    ptMenuItemNode->m_tMenuItem.m_u32Begin_x = ptMenuItemNode->m_tMenuItem.m_ptMenu->m_u32MenuBegin_x +
                                               _ptMenu->m_u8MenuItemNum * MENU_ITEN_HEIGHT;             /* 设置图形起始x坐标 */
    ptMenuItemNode->m_tMenuItem.m_u32Begin_y = ptMenuItemNode->m_tMenuItem.m_ptMenu->m_u32MenuBegin_y;  /* 设置图形起始y坐标 */
    ptMenuItemNode->m_tMenuItem.m_u32End_x = ptMenuItemNode->m_tMenuItem.m_ptMenu->m_u32MenuBegin_x +
                                               (_ptMenu->m_u8MenuItemNum + 1) * MENU_ITEN_HEIGHT - 1;   /* 设置图形结束x坐标 */
    ptMenuItemNode->m_tMenuItem.m_u32End_y = ptMenuItemNode->m_tMenuItem.m_ptMenu->m_u32MenuEnd_y;      /* 设置图形结束y坐标 */
    ptMenuItemNode->m_tMenuItem.m_u8pResources_Default = GetBmpResource(_strResources_default,
                                                        &ptMenuItemNode->m_tMenuItem.m_u8BitCount_Default);/* 设置默认图片资源 */
    ptMenuItemNode->m_tMenuItem.m_u8pResources_Clicked = GetBmpResource(_strResources_Clicked,
                                                        &ptMenuItemNode->m_tMenuItem.m_u8BitCount_Clicked);/* 设置点击图片资源 */
    ptMenuItemNode->m_tMenuItem.m_u8pResources_Select = GetBmpResource(_strResources_Clicked,
                                                        &ptMenuItemNode->m_tMenuItem.m_u8BitCount_Select); /* 设置选中图片资源 */
    ptMenuItemNode->m_tMenuItem.m_u8Type = BMP;                                                 /* 设置菜单项的图片类型 */
    ptMenuItemNode->m_tMenuItem.m_u8MenuFunction = _u8Type;                                     /* 菜单选项的功能 */
    ptMenuItemNode->m_tMenuItem.m_u8IsSelcet = NO_SELECT;
    ptMenuItemNode->m_tMenuItem.m_u8IsClicked = NO_CLICKED;

    /* 6.菜单选项数量加1 */
    _ptMenu->m_u8MenuItemNum++;    

    /* 7.置位刷新标志位 */
    ptMenuItemNode->m_tMenuItem.m_u8IsRefresh = REFRESH;
    
    return DB_OK;
}

/**********************************************************************
* Function:         CleanMenuItems
* Description:      清空菜单的的图形
* Input:            @_ptMenu                菜单句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 CleanMenuItems(T_Menu *_ptMenu)
{
    /* 1.入口参数判断 */
    if (_ptMenu == DB_NULL)
    {
        printf("CleanMenuItems: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    T_MenuItemNode *ptMenuItemNode;           /* 图形节点，遍历用 */
    T_MenuItemNode *ptFreeMenuItemNode;       /* 图形节点，释放用 */

    /* 3.获取链表头的下一个节点 */
    ptMenuItemNode = _ptMenu->m_tMenuItemHead.m_ptNext;

    /* 4.链表头指向空 */
    _ptMenu->m_tMenuItemHead.m_ptNext = DB_NULL;

    /* 5.清除节点 */
    while (ptMenuItemNode != DB_NULL)
    {
        /* 获取当前节点，准备释放用 */
        ptFreeMenuItemNode = ptMenuItemNode;
        /* 移动到下一个 */
        ptMenuItemNode = ptMenuItemNode->m_ptNext;
        /* 释放节点图形资源 */
        if (ptFreeMenuItemNode->m_tMenuItem.m_u8pResources_Clicked != DB_NULL)
            free(ptFreeMenuItemNode->m_tMenuItem.m_u8pResources_Clicked);
        if (ptFreeMenuItemNode->m_tMenuItem.m_u8pResources_Select != DB_NULL)
            free(ptFreeMenuItemNode->m_tMenuItem.m_u8pResources_Select);
        if (ptFreeMenuItemNode->m_tMenuItem.m_u8pResources_Default != DB_NULL)
            free(ptFreeMenuItemNode->m_tMenuItem.m_u8pResources_Default);    
        /* 释放节点 */
        free(ptFreeMenuItemNode);
    }

    /* 6.清空节点数量 */
    _ptMenu->m_u8MenuItemNum = 0;
    
    return DB_OK;
}

/**********************************************************************
* Function:         AddTitleItem
* Description:      向标题链表中添加菜单节点
* Input:            @_ptTitle               标题栏
*                   @_u8Type                菜单功能类型
*                   @_strResources_default  默认资源位置
*                   @_strResources_Clicked  点击时资源位置
* Attention:        该函数会向标题链表中添加图形，并且将刷新标志位置位
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 AddTitleItem(T_Title *_ptTitle, DB_UINT8 _u8Type, const DB_CHAR *_strResources_default, const DB_CHAR *_strResources_Clicked)
{
    /* 1.入口参数判断 */
    if (_strResources_default == DB_NULL || _strResources_Clicked == DB_NULL || _ptTitle == DB_NULL)
    {
        printf("AddTitleItem: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.申请节点空间 */
    T_TitleItemNode *ptTitleItemNode;           /*节点指针变量, 用于申请空间和赋值时使用*/

    ptTitleItemNode = malloc(sizeof(T_MenuItemNode));
    if (ptTitleItemNode == DB_NULL)
    {
        perror("AddTitleItem: malloc fail");
        return DB_ERR;
    }

    /* 3.设置节点父对象 */
    ptTitleItemNode->m_tTitleItem.m_ptTitle = _ptTitle;

    /* 4.将标题加入标题项链表中 */
    ptTitleItemNode->m_ptNext = _ptTitle->m_tTitleItemHead.m_ptNext;      /* 新节点后继指向头节点的下个节点 */
    ptTitleItemNode->m_ptPrior = &_ptTitle->m_tTitleItemHead;             /* 新节点前驱指向头节点 */
    _ptTitle->m_tTitleItemHead.m_ptNext = ptTitleItemNode;                /* 头节点后继指向新节点 */
    if (ptTitleItemNode->m_ptNext != DB_NULL)
        ptTitleItemNode->m_ptNext->m_ptPrior = ptTitleItemNode;           /* 下个节点的前驱指向新节点 */  

    /* 5.设置标题项属性 */
    ptTitleItemNode->m_tTitleItem.m_u32Begin_x = ptTitleItemNode->m_tTitleItem.m_ptTitle->m_u32TitleEnd_x -
                                               (_ptTitle->m_u8TitleItemNum + 1) * TITLE_BAR_HEIGHT + 1;         /* 设置图形起始x坐标 */
    ptTitleItemNode->m_tTitleItem.m_u32Begin_y = ptTitleItemNode->m_tTitleItem.m_ptTitle->m_u32TitleBegin_y;    /* 设置图形起始y坐标 */
    ptTitleItemNode->m_tTitleItem.m_u32End_x = ptTitleItemNode->m_tTitleItem.m_ptTitle->m_u32TitleEnd_x -
                                               (_ptTitle->m_u8TitleItemNum) * TITLE_BAR_HEIGHT;                 /* 设置图形结束x坐标 */
    ptTitleItemNode->m_tTitleItem.m_u32End_y = ptTitleItemNode->m_tTitleItem.m_ptTitle->m_u32TitleEnd_y;        /* 设置图形结束y坐标 */
    ptTitleItemNode->m_tTitleItem.m_u8pResources_Default = GetBmpResource(_strResources_default,
                                                            &ptTitleItemNode->m_tTitleItem.m_u8BitCount_Default);/* 设置默认图片资源,并设置图片资源每个像素点的位数 */
    ptTitleItemNode->m_tTitleItem.m_u8pResources_Clicked = GetBmpResource(_strResources_Clicked,
                                                            &ptTitleItemNode->m_tTitleItem.m_u8BitCount_Clicked);/* 设置点击图片资源,并设置图片资源每个像素点的位数 */
    ptTitleItemNode->m_tTitleItem.m_u8pResources_Select = GetBmpResource(_strResources_Clicked,
                                                            &ptTitleItemNode->m_tTitleItem.m_u8BitCount_Select); /* 设置选中图片资源,并设置图片资源每个像素点的位数 */
    ptTitleItemNode->m_tTitleItem.m_u8Type = BMP;                                                 /* 设置菜单项的图片类型 */
    ptTitleItemNode->m_tTitleItem.m_u8MenuFunction = _u8Type;                                     /* 菜单选项的功能 */
    ptTitleItemNode->m_tTitleItem.m_u8IsSelcet = NO_SELECT;
    ptTitleItemNode->m_tTitleItem.m_u8IsClicked = NO_CLICKED;

    /* 6.标题选项数量加1 */
    _ptTitle->m_u8TitleItemNum++;    

    /* 7.置位刷新标志位 */
    ptTitleItemNode->m_tTitleItem.m_u8IsRefresh = REFRESH;

    #if DISABLE
    printf("标题项起始坐标%d, %d\n",ptTitleItemNode->m_tTitleItem.m_u32Begin_x, ptTitleItemNode->m_tTitleItem.m_u32Begin_y);
    printf("标题项结束坐标%d, %d\n",ptTitleItemNode->m_tTitleItem.m_u32End_x, ptTitleItemNode->m_tTitleItem.m_u32End_y);
    #endif
    return DB_OK;
}

/**********************************************************************
* Function:         CleanMenuItems
* Description:      清空标题的标题项
* Input:            @_ptTitle               标题句柄
* Output:           参数合法性判断：终端打印输出错误信息
* Return:           DB_OK                   成功
*                   DB_ERR                  失败
**********************************************************************/
DB_INT32 CleanTitleItems(T_Title *_ptTitle)
{
    /* 1.入口参数判断 */
    if (_ptTitle == DB_NULL)
    {
        printf("CleanMenuItems: Invalid parameter(Null pointer)\n");
        return DB_ERR;
    }

    /* 2.定义局部变量 */
    T_TitleItemNode *ptTitleItemNode;           /* 图形节点，遍历用 */
    T_TitleItemNode *ptFreeTitleItemNode;       /* 图形节点，释放用 */

    /* 3.获取链表头的下一个节点 */
    ptTitleItemNode = _ptTitle->m_tTitleItemHead.m_ptNext;

    /* 4.链表头指向空 */
    _ptTitle->m_tTitleItemHead.m_ptNext = DB_NULL;

    /* 5.清除节点 */
    while (ptTitleItemNode != DB_NULL)
    {
        /* 获取当前节点，准备释放用 */
        ptFreeTitleItemNode = ptTitleItemNode;
        /* 移动到下一个 */
        ptTitleItemNode = ptTitleItemNode->m_ptNext;
        /* 释放节点图形资源 */
        if (ptFreeTitleItemNode->m_tTitleItem.m_u8pResources_Clicked != DB_NULL)
            free(ptFreeTitleItemNode->m_tTitleItem.m_u8pResources_Clicked);
        if (ptFreeTitleItemNode->m_tTitleItem.m_u8pResources_Select != DB_NULL)
            free(ptFreeTitleItemNode->m_tTitleItem.m_u8pResources_Select);
        if (ptFreeTitleItemNode->m_tTitleItem.m_u8pResources_Default != DB_NULL)
            free(ptFreeTitleItemNode->m_tTitleItem.m_u8pResources_Default);    
        /* 释放节点 */
        free(ptFreeTitleItemNode);
    }

    /* 6.清空节点数量 */
    _ptTitle->m_u8TitleItemNum = 0;
    
    return DB_OK;
}