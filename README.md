当前鼠标支持模式：
    绝对模式
    相对模式(功能未做)

程序存在的问题：
    1、调整窗口位置后图像在屏幕外的没有做处理
        包含函数 DrawFullRect DrawBmp DrawLine DrawFullCircle
    2、屏幕尺寸改变会出现问题
    3、如果未插入鼠标程序不会运行，应做动态监测是否插入鼠标，插入鼠标后做鼠标的初始化


未做功能：
    1、鼠标设备动态监测

比较1.0版本：
	1、对象继承思想