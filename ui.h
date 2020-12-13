#ifndef UI_H
#define UI_H
 
#include "DB_TypeDef.h"
#include "bmp.h"


/***********************************************************
*                       宏定义                              *
************************************************************/
/* 图形的类型 */
#define NONE                        0           /* 无图形 */
#define BMP                         1           /* BMP图案*/
#define LINE                        2           /* 线条 */
#define RECTANGE                    3           /* 矩形 */
#define RING                        4           /* 圆环 */

/* 功能性类型 */
#define CLEAN                       20          /* 清除 */
#define CLOSE                       21          /* 关闭 */


/* 窗口位置相关定义 */
#define DEFAULT_WINDOW_BEGIN_X      0           /* 默认窗口起始x位置 */
#define DEFAULT_WINDOW_BEGIN_Y      0           /* 默认窗口起始y位置 */
#define TITLE_BAR_HEIGHT            20          /* 标题栏高度 */
#define MENU_ITEN_HEIGHT            54          /* 菜单项的高度(高度即宽度，菜单项为正方形) */
#define LIMIT_WINDOW_MAX_X          800         /* 窗口横向最大值 */
#define LIMIT_WINDOW_MAX_Y          600         /* 窗口纵向最大值 */
#define LIMIT_WINDOW_MIN_X          1           /* 窗口横向最小值 */
#define LIMIT_WINDOW_MIN_Y          1           /* 窗口横向最小值 */

/* 刷新 */
#define REFRESH                     1
#define NO_REFRESH                  0

/* 图形是否被选中 */
#define SELECT                      0x1
#define NO_SELECT                   0x0

/* 按钮是否点击 */
#define CLICKED                     0x1
#define NO_CLICKED                  0x0

/* 窗口部件默认颜色 */
#define DEFAULT_TITLE_COLOR                 0x00ffdbff
#define DEFAULT_MENU_COLOR                  0x00ffffff
#define DEFAULT_DRAW_COLOR                  0x00000000

/* 部件类型 */
#define PART_NONE                                0      /* 无部件(直接显示在fb) */
#define PART_MAIN_WINDOW                         1      /* 主窗口 */
#define PART_MENU                                2      /* 菜单 */
#define PART_TITLE                               3      /* 标题栏 */
#define PART_DRAWING_BOARD                       4      /* 画板 */

/***********************************************************
*                     自定义类型                            *
************************************************************/

/* 自定义类型声明 */
struct WindowAttribute;                         /* 窗口 */
struct TitleAttribute;                          /* 标题栏 */
struct MenuAttribute;                           /* 菜单 */
struct DarwingBoardAttribute;                   /* 画板 */

/* 标题项属性 */
typedef struct TitleItemAttribute
{
    /* 标题位置参数 */
    DB_UINT32 m_u32Begin_x;          /* 图形起始x坐标 */
    DB_UINT32 m_u32Begin_y;          /* 图形起始y坐标 */
    DB_UINT32 m_u32End_x;            /* 图形结束x坐标 */
    DB_UINT32 m_u32End_y;            /* 图形结束y坐标 */
    /* 标题选项参数 */
    DB_UINT8* m_u8pResources_Default; /* 默认状态下图形的资源位置 */
    DB_UINT8* m_u8pResources_Select;  /* 选中状态下图形的资源位置(暂时未用) */
    DB_UINT8* m_u8pResources_Clicked; /* 点击状态下图形的资源位置 */

    DB_UINT8 m_u8BitCount_Default;   /* 默认图形资源每个像素的位数 */
    DB_UINT8 m_u8BitCount_Select;    /* 选中图形资源每个像素的位数 */
    DB_UINT8 m_u8BitCount_Clicked;   /* 点击图形资源每个像素的位数 */
    DB_UINT8 m_u8Type;               /* 标题项的类型，目前都为BMP */

    DB_UINT8 m_u8IsSelcet;           /* 是否被选中 */
    DB_UINT8 m_u8MenuFunction;       /* 菜单选项的功能 */
    DB_UINT8 m_u8IsRefresh;          /* 是否需要刷新 */
    DB_UINT8 m_u8IsClicked;          /* 是否被点击 */
    /* 父对象 */
    struct TitleAttribute *m_ptTitle;/* 指向包含该菜单项的菜单 */
}T_TitleItem;

/* 菜单项属性 */
typedef struct MenuItemAttribute
{
    /* 菜单位置参数 */
    DB_UINT32 m_u32Begin_x;          /* 图形起始x坐标 */
    DB_UINT32 m_u32Begin_y;          /* 图形起始y坐标 */
    DB_UINT32 m_u32End_x;            /* 图形结束x坐标 */
    DB_UINT32 m_u32End_y;            /* 图形结束y坐标 */
    /* 菜单选项参数 */
    DB_UINT8* m_u8pResources_Default; /* 默认状态下图形的资源位置 */
    DB_UINT8* m_u8pResources_Select;  /* 选中状态下图形的资源位置(暂时未用) */
    DB_UINT8* m_u8pResources_Clicked; /* 点击状态下图形的资源位置 */

    DB_UINT8 m_u8BitCount_Default;   /* 默认图形资源每个像素的位数 */
    DB_UINT8 m_u8BitCount_Select;    /* 选中图形资源每个像素的位数 */
    DB_UINT8 m_u8BitCount_Clicked;   /* 点击图形资源每个像素的位数 */
    DB_UINT8 m_u8Type;               /* 菜单项的类型，目前都为BMP */

    DB_UINT8 m_u8IsSelcet;           /* 是否被选中 */
    DB_UINT8 m_u8MenuFunction;       /* 菜单选项的功能 */
    DB_UINT8 m_u8IsRefresh;          /* 是否需要刷新 */
    DB_UINT8 m_u8IsClicked;          /* 是否被点击 */

    /* 父对象 */
    struct MenuAttribute *m_ptMenu;  /* 指向包含该菜单项的菜单 */
}T_MenuItem;

/* 图形属性 */
typedef struct GraphicalAttribute
{
    /* 线条，矩形相关参数 */
    DB_UINT32 m_u32Begin_x;          /* 图形起始x坐标 */
    DB_UINT32 m_u32Begin_y;          /* 图形起始y坐标 */
    DB_UINT32 m_u32End_x;            /* 图形结束x坐标 */
    DB_UINT32 m_u32End_y;            /* 图形结束y坐标 */
    /* 圆形相关参数 */
    DB_UINT32 m_u32Center_x;         /* 圆心x坐标 */
    DB_UINT32 m_u32Center_y;         /* 圆心y坐标 */
    DB_UINT32 m_u32Radius;           /* 半径 */
    /* 共有参数 */
    DB_UINT32 m_u32Color_32;         /* 图形颜色 */
    DB_UINT32 m_u16Color_16;         /* 图形颜色 */
    DB_UINT8  m_u8Type;              /* 图形的类型 */
    DB_UINT8  m_u8Width;             /* 图形线条的宽度(单位：像素) */

    /* 父对象 */
    struct DarwingBoardAttribute *m_ptDarwingBoard;/* 画板 */
}T_Graphical;

/* 标题项节点 */
typedef struct TitleItemNode
{
    T_TitleItem m_tTitleItem;
    struct TitleItemNode *m_ptNext;
    struct TitleItemNode *m_ptPrior;
}T_TitleItemNode;
/* 标题 */
typedef struct TitleAttribute
{
    DB_UINT32 m_u32PartType;            /* 部件类型 */
    struct WindowAttribute *m_ptWindow; /* 指向父窗口 */
    T_TitleItemNode m_tTitleItemHead;   /* 菜单项链表头 */
    DB_UINT32 m_u32TitleBegin_x;        /* 标题栏起始坐标 */
    DB_UINT32 m_u32TitleBegin_y;
    DB_UINT32 m_u32TitleEnd_x;          /* 标题栏结束坐标 */
    DB_UINT32 m_u32TitleEnd_y;
    DB_UINT32 m_u32BackgroundColor;     /* 标题背景颜色 */
    DB_UINT8 m_u8TitleItemNum;          /* 菜单中菜单项的数量 */
    DB_UINT8 m_u8IsRefresh;             /* 是否需要刷新 */
}T_Title;

/* 菜单项节点 */
typedef struct MenuItemNode
{
    T_MenuItem m_tMenuItem;
    struct MenuItemNode *m_ptNext;
    struct MenuItemNode *m_ptPrior;
}T_MenuItemNode;
/* 菜单 */
typedef struct MenuAttribute
{
    DB_UINT32 m_u32PartType;            /* 部件类型 */
    struct WindowAttribute *m_ptWindow; /* 指向父窗口 */
    T_MenuItemNode m_tMenuItemHead;     /* 菜单项链表头 */
    DB_UINT32 m_u32MenuBegin_x;         /* 菜单栏起始坐标 */
    DB_UINT32 m_u32MenuBegin_y;
    DB_UINT32 m_u32MenuEnd_x;           /* 菜单栏结束坐标 */
    DB_UINT32 m_u32MenuEnd_y;
    DB_UINT32 m_u32BackgroundColor;     /* 画板背景颜色 */
    DB_UINT8 m_u8MenuItemNum;           /* 菜单中菜单项的数量 */
    DB_UINT8 m_u8IsRefresh;             /* 是否需要刷新 */
}T_Menu;

/* 图形节点 */
typedef struct GraphicalNode
{
    T_Graphical m_tGraphical;
    struct GraphicalNode *m_ptNext;
    struct GraphicalNode *m_ptPrior;
}T_GraphicalNode;
/* 画板 */
typedef struct DarwingBoardAttribute
{
    DB_UINT32 m_u32PartType;            /* 部件类型 */
    struct WindowAttribute *m_ptWindow; /* 指向父窗口 */
    T_GraphicalNode GraphicalHead;      /* 图形链表头 */
    DB_UINT32 m_u32DrawingBoardBegin_x; /* 画板起始坐标 */
    DB_UINT32 m_u32DrawingBoardBegin_y;
    DB_UINT32 m_u32DrawingBoardEnd_x;   /* 画板结束坐标 */
    DB_UINT32 m_u32DrawingBoardEnd_y;
    DB_UINT32 m_u32BackgroundColor;     /* 画板背景颜色 */
    DB_UINT8 m_u8GraphicalNum;          /* 画板中图形的数量 */
    DB_UINT8 m_u8IsRefresh;             /* 是否需要刷新 */   
}T_DarwingBoard;

/* 窗口 */
typedef struct WindowAttribute
{
    DB_UINT32 m_u32PartType;            /* 部件类型 */
    /* 窗口起始和结束坐标 */
    DB_UINT32 m_u32WindowBegin_x;
    DB_UINT32 m_u32WindowBegin_y;
    DB_UINT32 m_u32WindowEnd_x;
    DB_UINT32 m_u32WindowEnd_y;
    /* 成员结构体 */
    T_Title m_tTitle;                   /* 标题栏 */
    T_Menu m_tMenu;                     /* 菜单 */
    T_DarwingBoard m_DrawingBoard;      /* 画板 */
    /* 是否需要刷新 */
    DB_UINT8 m_u8IsRefresh;             /* 是否需要刷新 */
}T_Window;

/***********************************************************
*                       函数声明                            *
************************************************************/
DB_INT32 InitWindow(T_Window *_ptWindow, const DB_UINT32 _u32WindowMax_X, const DB_UINT32 _u32WindowMax_Y); /* 初始化窗口 */
DB_INT32 AddGarItem(const T_Graphical *_ptGraphical, T_DarwingBoard *_ptDarwingBoard);                      /* 向画板中添加图形节点 */ 
DB_INT32 AddMenuItem(T_Menu *_ptMene, DB_UINT8 _u8Type, const DB_CHAR *_strResources_default,
                     const DB_CHAR *_strResources_Clicked);                                                 /* 向菜单中添加菜单项 */
DB_INT32 AddTitleItem(T_Title *_ptTitle, DB_UINT8 _u8Type, const DB_CHAR *_strResources_default,
                      const DB_CHAR *_strResources_Clicked);                                                /* 向标题链表中添加标题功能选项 */
DB_INT32 CleanGarItems(T_DarwingBoard *_ptDarwingBoard);                                                    /* 清空画板图形节点 */
DB_INT32 CleanTitleItems(T_Title *_ptTitle);                                                                /* 清空标题项 */
DB_INT32 CleanMenuItems(T_Menu *_ptMenu);                                                                   /* 清空菜单项 */
#endif /* UI_H */