#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <mxml.h>
#include "app.h"

/***********************************************************
*                       宏定义                              *
************************************************************/
/* 设备驱动路径 */
#define FB_PATH_NAME        "/dev/fb0"
#define MICE_PATH_NAME      "/dev/input/event2"
#define BD_STDIN            0

/***********************************************************
*                       全局变量                            *
************************************************************/
T_MouseHandler g_tMouseHandler;     /* 全局鼠标操作句柄 */
T_FbHandler g_tFbInfo;              /* 全局fb信息 */
T_Window g_tWindow;                 /* 全局的窗口变量 */


DB_BOOL g_bMainIsClose = DB_FALSE;  /* 判断是否关闭主线程 */

/***********************************************************
*                        主函数                             *
************************************************************/
DB_INT32 main(DB_INT32 argc, DB_CHAR **argv)
{
    /* 1.定义变量 */
    /* 返回值 */
    DB_UINT32 s32RetState = DB_OK;  /* 主函数返回值 */
    DB_INT32 s32Ret = DB_ERR;       /* 调用函数返回值 */
    /* select相关 */
    DB_INT32 s32MaxFd;              /* 最大文件描述符个数 */
    fd_set tRead_fds;               /* 读文件描述符表 */
    fd_set tBackup_fds;             /* 备份文件描述符表 */
    struct timeval tTimeout;        /* select设置超时变量 */

    /* 2.初始化操作 */
    /* 初始化fb 并填充句柄 */
    if (FbInfoInit(&g_tFbInfo, FB_PATH_NAME) == DB_NULL)
    {
        perror("FbInfoInit fail");
        s32RetState = DB_ERR;
        goto ERROR_FBINFO_DEINIT;
    }
    /* 初始化鼠标 并填充句柄 */
    if (MouseInit(&g_tMouseHandler, MICE_PATH_NAME) == DB_ERR)
    {
        perror("MouseInit fail");
        s32RetState = DB_ERR;
        goto ERROR_MOUSE_DEINIT;
    }
    /* 初始化窗口 */
    SetWindow(&g_tFbInfo, &g_tWindow);

    /* 3.清屏 */
    //ClearScreen(&g_tFbInfo);

    /* 4.IO多路复用 */
    FD_ZERO(&tBackup_fds);                          /* 清空文件描述符表 */
    FD_SET(g_tMouseHandler.m_s32Fd, &tBackup_fds);  /* 添加鼠标文件描述符 */
    FD_SET(BD_STDIN, &tBackup_fds);                 /* 添加标准输入 */
    tTimeout.tv_sec = 1;                            /* 设置延时1s */
    tTimeout.tv_usec = 0;                           /* 设置延时微秒数 */
    s32MaxFd = g_tMouseHandler.m_s32Fd + 1;         /* 设置最大文件描述符个数 */   /* 此处不太严谨 */
    while(!g_bMainIsClose)
    {
        tTimeout.tv_sec = 1;                     /* 设置延时1s */
        tRead_fds = tBackup_fds;                 /* 更新文件描述符需要监听的表 */
        s32Ret = select(s32MaxFd, &tRead_fds, NULL, NULL, &tTimeout);
        if (s32Ret > 0)                          /* 有文件返回 */
        {
            if(FD_ISSET(g_tMouseHandler.m_s32Fd, &tRead_fds))        /* 是否是读文件描述符可用 */
            {
                /* 以下顺序不能乱 */
                UpdateMouseState(&g_tMouseHandler);
                UpdateWindow(&g_tFbInfo, &g_tWindow, &g_tMouseHandler);
                CleanMouse(&g_tFbInfo, &g_tMouseHandler, &g_tWindow);
                DarwWindow(&g_tFbInfo, &g_tWindow);
                DrawMouse(&g_tFbInfo, &g_tMouseHandler);
            }
            if(FD_ISSET(BD_STDIN, &tRead_fds))                      /* 是否标准输入可用 */
            {
                GetTerminalInput(&g_tWindow);                       /* 获取终端的输入，并设置宽度、颜色值 */
            }
        }
        else if (s32Ret == 0)   /* 超时 */
        {
            // printf("select timeout\n");      /* 超时打印 */
            continue;
        }
        else /* s32Ret < 0 */
        {
            printf("select fail\n");
            usleep(300000);            /* 延时300ms再次进入select函数 */
            continue;
        }
    }

    /*5.清空屏幕*/
    memset(g_tFbInfo.m_pu8StartAddr, 0, g_tFbInfo.m_tFix.smem_len);
    system("clear");    /* linux 终端清屏 */

    /* 6.关闭窗口 */
    CloseWindow(&g_tWindow);

    /* 7.反向初始化 */
    /* 鼠标反初始化 */
    ERROR_MOUSE_DEINIT:
        MouseDeInit(&g_tMouseHandler);

    /* fb反初始化 */
    ERROR_FBINFO_DEINIT:
        FbInfoDeInit(&g_tFbInfo);

    return s32RetState;
}
