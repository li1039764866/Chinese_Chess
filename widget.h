#ifndef WIDGET_H
#define WIDGET_H

#include<QWidget>
#include<QPaintEvent>
#include<QPainter>
#include<QPalette>
#include<QMouseEvent>
#include<QLabel>
#include<QTimer>
#include<string>
#include<QPushButton>
#include<QMessageBox>

class Widget : public QWidget
{
    Q_OBJECT
private:
    int Chessman_Coordinate[9][10];//棋子的坐标就是数组下标换算而来，值代表了属性
    int To_Move[9][10];//选中棋子后提示可移动的位置
    int Regret_Chessman_Array[6][6];//存储棋子移动前和移动后的坐标和属性，存储最后六次移动，最多一次悔棋六次，循环存储
    int Clicked_X;//记录点击的x坐标，显示点击棋子,并在移动之后给对方提示
    int Clicked_Y;//记录点击的y坐标，显示点击棋子
    int Regret_Game_Time;//悔棋次数，最多不会超过六次，前面的数据会被覆盖
    int Step_Number;//记录步数，判断哪一方移动棋子
    int Step_Time_Number;//记录步时
    int Sum_Time_Number;//记录局时
    bool Game_Is_Over;//判断游戏是否结束
    QLabel *Player1_Time;//显示玩家一的步时
    QLabel *Player2_Time;//显示玩家二的步时
    QLabel *Sum_Time;//显示对局时间
    QTimer *Timer;//每一秒更新时间
    QPushButton *Start_Game;//开始按钮
    QPushButton *Regret_Game;//悔棋按钮
    QPushButton *Game_Help;//游戏规则按钮
    QPushButton *Exit_Game;//退出游戏按钮
public:
    Widget(QWidget *parent = 0);
    ~Widget();
    void  Initialize_Array();//初始化Chessman_Coordinate数组
    void Initialize_Move_Array();//初始化To_Move数组
    void paintEvent(QPaintEvent *);//绘图事件函数
    void mousePressEvent(QMouseEvent *event);//鼠标点击事件
    void To_Move_Chessman(int i, int j, int value);//传入待移动的值和坐标，确定棋子可移动位置
    void Eat_Chessman(int Eat_x, int Eat_y, int Eat_value, int Eaten_x, int Eaten_y, int Eaten_value);
    //移动或吃子函数，传入移动棋子前后的坐标和属性，悔棋数组也在此函数里面获取值
    void Is_Win();//判断对局是否结束
public slots:
    void Time_Out_Slots();//计时函数，时间间隔为1秒
    void Start_Game_Slots();//开始按钮槽函数
    void Regret_Game_Slots();//悔棋函数槽函数
    void Game_Help_Slots();//游戏规则槽函数
    void Exit_Game_Slots();//退出游戏槽函数
};

#endif // WIDGET_H
