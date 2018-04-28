#include "widget.h"
#include<QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setFixedSize(1100,1000);
    this->setWindowTitle("李子的象棋");

    QPalette palette;
    palette.setBrush(QPalette::Background,QBrush(QPixmap(":/new/prefix1/img/Chess_Board.jpg")));
    this->setPalette(palette);


    Player1_Time=new QLabel("步时: 00:00:00",this);
    Player1_Time->setGeometry(920,180,150,40);
    Player2_Time=new QLabel("步时: 00:00:00",this);
    Player2_Time->setGeometry(920,780,150,40);
    Sum_Time=new QLabel("局时: 00:00:00",this);
    Sum_Time->setGeometry(920,325,150,40);

    Timer =new QTimer(this);
    connect(Timer,SIGNAL(timeout()),this,SLOT(Time_Out_Slots()));

    Step_Time_Number=0;
    Sum_Time_Number=0;
    Game_Is_Over=true;

    Start_Game=new QPushButton("开始游戏",this);
    Start_Game->setGeometry(930,400,100,40);
    Start_Game->setFlat(1);

    Regret_Game=new QPushButton("悔棋",this);
    Regret_Game->setGeometry(930,450,100,40);
    Regret_Game->setFlat(1);

    Game_Help=new QPushButton("游戏规则",this);
    Game_Help->setGeometry(930,500,100,40);
    Game_Help->setFlat(1);

    Exit_Game=new QPushButton("离开游戏",this);
    Exit_Game->setGeometry(930,550,100,40);
    Exit_Game->setFlat(1);

    connect(Start_Game,SIGNAL(clicked(bool)),this,SLOT(Start_Game_Slots()));
    connect(Regret_Game,SIGNAL(clicked(bool)),this,SLOT(Regret_Game_Slots()));
    connect(Game_Help,SIGNAL(clicked(bool)),this,SLOT(Game_Help_Slots()));
    connect(Exit_Game,SIGNAL(clicked(bool)),this,SLOT(Exit_Game_Slots()));
}

Widget::~Widget()
{
    delete this;
}

void Widget::Initialize_Array()
{
    memset(Chessman_Coordinate,0,90*sizeof(int));//初始化数组
    memset(Regret_Chessman_Array,0,36*sizeof(int));
    Chessman_Coordinate[4][0]=1;//将，黑色棋子
    Chessman_Coordinate[3][0]=2;//士
    Chessman_Coordinate[5][0]=2;
    Chessman_Coordinate[2][0]=3;//相
    Chessman_Coordinate[6][0]=3;
    Chessman_Coordinate[1][0]=4;//马
    Chessman_Coordinate[7][0]=4;
    Chessman_Coordinate[0][0]=5;//車
    Chessman_Coordinate[8][0]=5;
    Chessman_Coordinate[1][2]=6;//炮
    Chessman_Coordinate[7][2]=6;
    for(int i=0;i<5;i++)//兵
    {
        Chessman_Coordinate[2*i][3]=7;
    }
    Chessman_Coordinate[4][9]=-1;//帅，红色棋子
    Chessman_Coordinate[3][9]=-2;
    Chessman_Coordinate[5][9]=-2;
    Chessman_Coordinate[2][9]=-3;
    Chessman_Coordinate[6][9]=-3;
    Chessman_Coordinate[1][9]=-4;
    Chessman_Coordinate[7][9]=-4;
    Chessman_Coordinate[0][9]=-5;
    Chessman_Coordinate[8][9]=-5;
    Chessman_Coordinate[1][7]=-6;
    Chessman_Coordinate[7][7]=-6;
    for(int i=0;i<5;i++)
    {
        Chessman_Coordinate[2*i][6]=-7;
    }
    Initialize_Move_Array();//初始化移动数组的值为Chessman_Coordinate相应的值，利用其值判断可移动位置
}

void Widget::Initialize_Move_Array()//在选中棋子或者移动操作之后调用
{
    for(int i=0;i<9;i++)
    {
        for(int j=0;j<10;j++)
        {
            To_Move[i][j]=Chessman_Coordinate[i][j];
        }
    }
}
void Widget::mousePressEvent(QMouseEvent *event)//鼠标点击事件
{
    if(10<event->pos().x()&&event->pos().x()<=890&&
            10<event->pos().y()&&event->pos().y()<=990&&!Game_Is_Over)
    {
        for(int i=0;i<9;i++)//判断鼠标点击的坐标在哪一个区域内，转化为棋子左上角的坐标
        {
            for(int j=0;j<10;j++)
            {
                if(10+100*i<=event->pos().x()&&event->pos().x()<10+100*(i+1)&&
                        10+100*j<=event->pos().y()&&event->pos().y()<10+100*(j+1))
                {
                    if(To_Move[i][j]==10)//如果为该值，代表棋子在该位置是可以移动的
                    {//移动或者吃子操作
                        Eat_Chessman(Clicked_X,Clicked_Y,Chessman_Coordinate[Clicked_X][Clicked_Y],i,j,Chessman_Coordinate[i][j]);
                    }
                    else  if(Chessman_Coordinate[i][j]==0)
                    {//0值代表该位置空白且不可移动到该位置，恢复上次点击的To_Move数组
                        Initialize_Move_Array();
                        return;
                    }
                    else if(Step_Number%2==0&&Chessman_Coordinate[i][j]>0)
                    {//轮到黑棋走子且点击的坐标为黑色棋子
                        Initialize_Move_Array();
                        To_Move_Chessman(i,j,Chessman_Coordinate[i][j]);//找出该子可移动的位置
                    }
                    else if(Step_Number%2==1&&Chessman_Coordinate[i][j]<0)
                    {//轮到红棋走子且点击的坐标为红色棋子
                        Initialize_Move_Array();
                        To_Move_Chessman(i,j,Chessman_Coordinate[i][j]);//找出该子可移动的位置
                    }
                    else
                    {//其他情况，如红色落子点到黑子，则恢复To_Move数组
                        Initialize_Move_Array();
                        return;
                    }
                    Clicked_X=i;//将点击得到的坐标存到其中，作为走子的提示
                    Clicked_Y=j;
                    this->repaint();
                }
            }
        }
    }
}

void Widget::To_Move_Chessman(int i,int j,int value)
{//为所有棋子的移动制定规则，并记录下它们每次受到点击之后可以移动的位置到
    bool Is_Skip=false;//记录炮是否跳过
    switch (value) {
    case 1://将的移动规则
        if(i-1>=3&&Chessman_Coordinate[i-1][j]<=0) To_Move[i-1][j]=10;
        if(i+1<=5&&Chessman_Coordinate[i+1][j]<=0) To_Move[i+1][j]=10;
        if(j-1>=0&&Chessman_Coordinate[i][j+1]<=0) To_Move[i][j-1]=10;
        if(j+1<=2&&Chessman_Coordinate[i][j+1]<=0) To_Move[i][j+1]=10;
        break;
    case 2://士的移动规则
        if(i-1>=3&&j-1>=0&&Chessman_Coordinate[i-1][j-1]<=0) To_Move[i-1][j-1]=10;
        if(i-1>=3&&j+1<=2&&Chessman_Coordinate[i-1][j+1]<=0) To_Move[i-1][j+1]=10;
        if(i+1<=5&&j-1>=0&&Chessman_Coordinate[i+1][j-1]<=0) To_Move[i+1][j-1]=10;
        if(i+1<=5&&j+1<=2&&Chessman_Coordinate[i+1][j+1]<=0) To_Move[i+1][j+1]=10;
        break;
    case 3://相的移动规则
        if(i-2>=0&&j-2>=0&&Chessman_Coordinate[i-2][j-2]<=0) To_Move[i-2][j-2]=10;
        if(i-2>=0&&j+2<=4&&Chessman_Coordinate[i-2][j+2]<=0) To_Move[i-2][j+2]=10;
        if(i+2<=8&&j-2>=0&&Chessman_Coordinate[i+2][j-2]<=0) To_Move[i+2][j-2]=10;
        if(i+2<=8&&j+2<=4&&Chessman_Coordinate[i+2][j+2]<=0) To_Move[i+2][j+2]=10;
        break;
    case 4://马的移动规则，蹩脚马不能移动
        if(i-2>=0&&j-1>=0&&Chessman_Coordinate[i-1][j]==0&&Chessman_Coordinate[i-2][j-1]<=0) To_Move[i-2][j-1]=10;
        if(i-1>=0&&j-2>=0&&Chessman_Coordinate[i][j-1]==0&&Chessman_Coordinate[i-1][j-2]<=0) To_Move[i-1][j-2]=10;
        if(i-2>=0&&j+1<=9&&Chessman_Coordinate[i-1][j]==0&&Chessman_Coordinate[i-2][j+1]<=0) To_Move[i-2][j+1]=10;
        if(i-1>=0&&j+2<=9&&Chessman_Coordinate[i][j+1]==0&&Chessman_Coordinate[i-1][j+2]<=0) To_Move[i-1][j+2]=10;
        if(i+2<=8&&j-1>=0&&Chessman_Coordinate[i+1][j]==0&&Chessman_Coordinate[i+2][j-1]<=0) To_Move[i+2][j-1]=10;
        if(i+1<=8&&j-2>=0&&Chessman_Coordinate[i][j-1]==0&&Chessman_Coordinate[i+1][j-2]<=0) To_Move[i+1][j-2]=10;
        if(i+2<=8&&j+1<=9&&Chessman_Coordinate[i+1][j]==0&&Chessman_Coordinate[i+2][j+1]<=0) To_Move[i+2][j+1]=10;
        if(i+1<=8&&j+2<=9&&Chessman_Coordinate[i][j+1]==0&&Chessman_Coordinate[i+1][j+2]<=0) To_Move[i+1][j+2]=10;
        break;
    case 5://車的移动规则
        for(int x=1;x<=8;x++)
        {//这是車在向右方向可以选择移动的代码，赋值为10，代表该点可以移动
            if(i+x<=8&&Chessman_Coordinate[i+x][j]==0) To_Move[i+x][j]=10;
            else  if(i+x<=8&&Chessman_Coordinate[i+x][j]<0)
            {//在向右方向碰到另一方棋子，该点可以移动，之后跳出循环
                To_Move[i+x][j]=10;
                break;
            }
            else  break;
        }
        for(int x=1;x<=8;x++)
        {
            if(i-x>=0&&Chessman_Coordinate[i-x][j]==0) To_Move[i-x][j]=10;
            else  if(i-x>=0&&Chessman_Coordinate[i-x][j]<0)
            {
                To_Move[i-x][j]=10;
                break;
            }
            else  break;
        }
        for(int x=1;x<=9;x++)
        {
            if(j+x<=9&&Chessman_Coordinate[i][j+x]==0) To_Move[i][j+x]=10;
            else  if(j+x<=9&&Chessman_Coordinate[i][j+x]<0)
            {
                To_Move[i][j+x]=10;
                break;
            }
            else  break;
        }
        for(int x=1;x<=9;x++)
        {
            if(j-x>=0&&Chessman_Coordinate[i][j-x]==0) To_Move[i][j-x]=10;
            else  if(j-x>=0&&Chessman_Coordinate[i][j-x]<0)
            {
                To_Move[i][j-x]=10;
                break;
            }
            else  break;
        }

        break;
    case 6://炮的移动规则
        Is_Skip=false;//默认没有跳过
        for(int x=1;x<=8;x++)
        {
            if(i+x<=8&&Chessman_Coordinate[i+x][j]==0&&!Is_Skip) To_Move[i+x][j]=10;
            else  if(i+x<=8&&Chessman_Coordinate[i+x][j]!=0&&!Is_Skip)
            {//如果碰到棋子，设置Is_Skip为真
                Is_Skip=true;
            }
            else if(i+x<=8&&Chessman_Coordinate[i+x][j]<0&&Is_Skip)
            {//跳过之后再遇到另一方的棋子，则可以移动
                To_Move[i+x][j]=10;
                break;
            }
        }
        Is_Skip=false;
        for(int x=1;x<=8;x++)
        {
            if(i-x>=0&&Chessman_Coordinate[i-x][j]==0&&!Is_Skip) To_Move[i-x][j]=10;
            else  if(i-x>=0&&Chessman_Coordinate[i-x][j]!=0&&!Is_Skip)
            {
                Is_Skip=true;
            }
            else if(i-x>=0&&Chessman_Coordinate[i-x][j]<0&&Is_Skip)
            {
                To_Move[i-x][j]=10;
                break;
            }
        }
        Is_Skip=false;
        for(int x=1;x<=9;x++)
        {
            if(j+x<=9&&Chessman_Coordinate[i][j+x]==0&&!Is_Skip) To_Move[i][j+x]=10;
            else  if(j+x<=9&&Chessman_Coordinate[i][j+x]!=0&&!Is_Skip)
            {
                Is_Skip=true;
            }
            else if(j+x<=9&&Chessman_Coordinate[i][j+x]<0&&Is_Skip)
            {
                To_Move[i][j+x]=10;
                break;
            }
        }
        Is_Skip=false;
        for(int x=1;x<=9;x++)
        {
            if(j-x>=0&&Chessman_Coordinate[i][j-x]==0&&!Is_Skip) To_Move[i][j-x]=10;
            else  if(j-x>=0&&Chessman_Coordinate[i][j-x]!=0&&!Is_Skip)
            {
                Is_Skip=true;
            }
            else if(j-x>=0&&Chessman_Coordinate[i][j-x]<0&&Is_Skip)
            {
                To_Move[i][j-x]=10;
                break;
            }
        }
        break;
    case 7://兵的移动规则
        if(j<5)//未过河只能向前走
        {
            To_Move[i][j+1]=10;
        }
        else
        {
            if(i-1>=0&&Chessman_Coordinate[i-1][j]<=0) To_Move[i-1][j]=10;
            if(i+1<=8&&Chessman_Coordinate[i+1][j]<=0) To_Move[i+1][j]=10;
            if(j+1<=9&&Chessman_Coordinate[i][j+1]<=0) To_Move[i][j+1]=10;
        }
        break;
    case -1:
        if(i-1>=3&&Chessman_Coordinate[i-1][j]>=0) To_Move[i-1][j]=10;
        if(i+1<=5&&Chessman_Coordinate[i+1][j]>=0) To_Move[i+1][j]=10;
        if(j-1>=7&&Chessman_Coordinate[i][j-1]>=0) To_Move[i][j-1]=10;
        if(j+1<=9&&Chessman_Coordinate[i][j+1]>=0) To_Move[i][j+1]=10;
        break;
    case -2:
        if(i-1>=3&&j-1>=7&&Chessman_Coordinate[i-1][j-1]>=0) To_Move[i-1][j-1]=10;
        if(i-1>=3&&j+1<=9&&Chessman_Coordinate[i-1][j+1]>=0) To_Move[i-1][j+1]=10;
        if(i+1<=5&&j-1>=7&&Chessman_Coordinate[i+1][j-1]>=0) To_Move[i+1][j-1]=10;
        if(i+1<=5&&j+1<=9&&Chessman_Coordinate[i+1][j+1]>=0) To_Move[i+1][j+1]=10;
        break;
    case -3:
        if(i-2>=0&&j-2>=5&&Chessman_Coordinate[i-2][j-2]>=0) To_Move[i-2][j-2]=10;
        if(i-2>=0&&j+2<=9&&Chessman_Coordinate[i-2][j+2]>=0) To_Move[i-2][j+2]=10;
        if(i+2<=8&&j-2>=5&&Chessman_Coordinate[i+2][j-2]>=0) To_Move[i+2][j-2]=10;
        if(i+2<=8&&j+2<=9&&Chessman_Coordinate[i+2][j+2]>=0) To_Move[i+2][j+2]=10;
        break;
    case -4:
        if(i-2>=0&&j-1>=0&&Chessman_Coordinate[i-1][j]==0&&Chessman_Coordinate[i-2][j-1]>=0) To_Move[i-2][j-1]=10;
        if(i-1>=0&&j-2>=0&&Chessman_Coordinate[i][j-1]==0&&Chessman_Coordinate[i-1][j-2]>=0) To_Move[i-1][j-2]=10;
        if(i-2>=0&&j+1<=9&&Chessman_Coordinate[i-1][j]==0&&Chessman_Coordinate[i-2][j+1]>=0) To_Move[i-2][j+1]=10;
        if(i-1>=0&&j+2<=9&&Chessman_Coordinate[i][j+1]==0&&Chessman_Coordinate[i-1][j+2]>=0) To_Move[i-1][j+2]=10;
        if(i+2<=8&&j-1>=0&&Chessman_Coordinate[i+1][j]==0&&Chessman_Coordinate[i+2][j-1]>=0) To_Move[i+2][j-1]=10;
        if(i+1<=8&&j-2>=0&&Chessman_Coordinate[i][j-1]==0&&Chessman_Coordinate[i+1][j-2]>=0) To_Move[i+1][j-2]=10;
        if(i+2<=8&&j+1<=9&&Chessman_Coordinate[i+1][j]==0&&Chessman_Coordinate[i+2][j+1]>=0) To_Move[i+2][j+1]=10;
        if(i+1<=8&&j+2<=9&&Chessman_Coordinate[i][j+1]==0&&Chessman_Coordinate[i+1][j+2]>=0) To_Move[i+1][j+2]=10;
        break;
    case -5:
        for(int x=1;x<=8;x++)
        {
            if(i+x<=8&&Chessman_Coordinate[i+x][j]==0) To_Move[i+x][j]=10;
            else  if(i+x<=8&&Chessman_Coordinate[i+x][j]>0)
            {
                To_Move[i+x][j]=10;
                break;
            }
            else break;
        }
        for(int x=1;x<=8;x++)
        {
            if(i-x>=0&&Chessman_Coordinate[i-x][j]==0) To_Move[i-x][j]=10;
            else  if(i-x>=0&&Chessman_Coordinate[i-x][j]>0)
            {
                To_Move[i-x][j]=10;
                break;
            }
            else break;
        }
        for(int x=1;x<=9;x++)
        {
            if(j+x<=9&&Chessman_Coordinate[i][j+x]==0) To_Move[i][j+x]=10;
            else  if(j+x<=9&&Chessman_Coordinate[i][j+x]>0)
            {
                To_Move[i][j+x]=10;
                break;
            }
            else break;
        }
        for(int x=1;x<=9;x++)
        {
            if(j-x>=0&&Chessman_Coordinate[i][j-x]==0) To_Move[i][j-x]=10;
            else  if(j-x>=0&&Chessman_Coordinate[i][j-x]>0)
            {
                To_Move[i][j-x]=10;
                break;
            }
            else break;
        }
        break;
    case -6:
        Is_Skip=false;
        for(int x=1;x<=8;x++)
        {
            if(i+x<=8&&Chessman_Coordinate[i+x][j]==0&&!Is_Skip) To_Move[i+x][j]=10;
            else  if(i+x<=8&&Chessman_Coordinate[i+x][j]!=0&&!Is_Skip)
            {
                Is_Skip=true;
            }
            else if(i+x<=8&&Chessman_Coordinate[i+x][j]>0&&Is_Skip)
            {
                To_Move[i+x][j]=10;
                break;
            }
        }
        Is_Skip=false;
        for(int x=1;x<=8;x++)
        {
            if(i-x>=0&&Chessman_Coordinate[i-x][j]==0&&!Is_Skip) To_Move[i-x][j]=10;
            else  if(i-x>=0&&Chessman_Coordinate[i-x][j]!=0&&!Is_Skip)
            {
                Is_Skip=true;
            }
            else if(i-x>=0&&Chessman_Coordinate[i-x][j]>0&&Is_Skip)
            {
                To_Move[i-x][j]=10;
                break;
            }
        }
        Is_Skip=false;
        for(int x=1;x<=9;x++)
        {
            if(j+x<=9&&Chessman_Coordinate[i][j+x]==0&&!Is_Skip) To_Move[i][j+x]=10;
            else  if(j+x<=9&&Chessman_Coordinate[i][j+x]!=0&&!Is_Skip)
            {
                Is_Skip=true;
            }
            else if(j+x<=9&&Chessman_Coordinate[i][j+x]>0&&Is_Skip)
            {
                To_Move[i][j+x]=10;
                break;
            }
        }
        Is_Skip=false;
        for(int x=1;x<=9;x++)
        {
            if(j-x>=0&&Chessman_Coordinate[i][j-x]==0&&!Is_Skip) To_Move[i][j-x]=10;
            else  if(j-x>=0&&Chessman_Coordinate[i][j-x]!=0&&!Is_Skip)
            {
                Is_Skip=true;
            }
            else if(j-x>=0&&Chessman_Coordinate[i][j-x]>0&&Is_Skip)
            {
                To_Move[i][j-x]=10;
                break;
            }
        }
        break;
    case -7:
        if(j>4)
        {
            To_Move[i][j-1]=10;
        }
        else
        {
            if(i-1>=0&&Chessman_Coordinate[i-1][j]>=0) To_Move[i-1][j]=10;
            if(i+1<=8&&Chessman_Coordinate[i+1][j]>=0) To_Move[i+1][j]=10;
            if(j-1>=0&&Chessman_Coordinate[i][j-1]>=0) To_Move[i][j-1]=10;
        }
        break;
    default:  break;
    }

}

void Widget::Eat_Chessman(int Eat_x, int Eat_y, int Eat_value, int Eaten_x, int Eaten_y, int Eaten_value)
{//有效的移动棋子和吃子操作
    Step_Time_Number=0;//有效移动或者吃子之后，步时归为0
    Step_Number++;
    Regret_Game_Time=0;//移动棋子后，悔棋数重置为0
    Regret_Chessman_Array[Step_Number%6][0]=Eat_x;//所有值保存数据到悔棋的数组中
    Regret_Chessman_Array[Step_Number%6][1]=Eat_y;
    Regret_Chessman_Array[Step_Number%6][2]=Eat_value;
    Regret_Chessman_Array[Step_Number%6][3]=Eaten_x;
    Regret_Chessman_Array[Step_Number%6][4]=Eaten_y;
    Regret_Chessman_Array[Step_Number%6][5]=Eaten_value;
    if(Eaten_value==0)//
    {
        Chessman_Coordinate[Eat_x][Eat_y]=0;
        Chessman_Coordinate[Eaten_x][Eaten_y]=Eat_value;
    }
    else  //(Eaten_value!=0)
    {
        Chessman_Coordinate[Eat_x][Eat_y]=0;
        Chessman_Coordinate[Eaten_x][Eaten_y]=Eat_value;
    }
    Initialize_Move_Array();
    Is_Win();
}

void Widget::Is_Win()
{
    Game_Is_Over=true;
    int Boss_Is_Meet[4]={0,0,0,0};//判断将帅是否见面
    for(int i=1;i<2;i++)
    {   //判断将是否被吃
        for(int i=3;i<=5;i++)
        {
            for(int j=0;j<=2;j++)
            {
                if(Chessman_Coordinate[i][j]==1)
                {
                    Game_Is_Over=false;
                    Boss_Is_Meet[0]=i;
                    Boss_Is_Meet[1]=j;
                }
            }
        }
        if(Game_Is_Over) break;//如果已经判断游戏结束，那么直接退出循环

        Game_Is_Over=true;
        for(int i=3;i<=5;i++)
        {  //判断帅是否被吃
            for(int j=7;j<=9;j++)
            {
                if(Chessman_Coordinate[i][j]==-1)
                {
                    Game_Is_Over=false;
                    Boss_Is_Meet[2]=i;
                    Boss_Is_Meet[3]=j;
                }
            }
        }
        if(Game_Is_Over) break;

        if(Boss_Is_Meet[0]==Boss_Is_Meet[2])//判断将帅是否见面
        {
            Game_Is_Over=true;
            for(int i=Boss_Is_Meet[1]+1;i<Boss_Is_Meet[3];i++)
            {
                if(Chessman_Coordinate[Boss_Is_Meet[0]][i]!=0)
                {
                    Game_Is_Over=false;
                    break;
                }
            }
            if(Game_Is_Over==true)
            {
                Step_Number++; //与被吃子的情况不同，走方失败，被吃子胜负为走方胜利，故此次走位步数加二
            }
        }
    }
    if(Game_Is_Over==true)
    {
        QString Winer=(Step_Number%2==0? "红棋":"黑棋");
        Timer->stop();
        Step_Number=0;
        QMessageBox::information(this,"对局结果：",Winer+"取得胜利！",QMessageBox::Yes);
    }
}
void Widget::Time_Out_Slots()
{
    Step_Time_Number++;
    Sum_Time_Number++;
    QString step_time="步时："+QString("%1").arg(Step_Time_Number/3600%60, 2, 10,QChar('0'))
            +":"+QString("%2").arg(Step_Time_Number/60%60, 2, 10, QChar('0'))
            +":"+QString("%3").arg(Step_Time_Number%60, 2, 10, QChar('0'));
    (Step_Number%2==0? Player1_Time:Player2_Time)->setText(step_time);
    //根据步数判断哪一个显示步时
    QString sum_time="局时："+QString("%1").arg(Sum_Time_Number/3600%60, 2, 10,QChar('0'))
            +":"+QString("%2").arg(Sum_Time_Number/60%60, 2, 10, QChar('0'))
            +":"+QString("%3").arg(Sum_Time_Number%60, 2, 10, QChar('0'));
    Sum_Time->setText(sum_time);

}

void Widget::Start_Game_Slots()
{
    Regret_Game_Time=0;
    Clicked_X=0;
    Clicked_Y=0;
    Step_Number=0;
    Step_Time_Number=0;
    Sum_Time_Number=0;
    Game_Is_Over=false;
    Start_Game->setText("重新开始游戏");
    Initialize_Array();
    this->repaint();
    Timer->start(1000);
    Player1_Time->setText("步时: 00:00:00");
    Player2_Time->setText("步时: 00:00:00");
}

void Widget::Regret_Game_Slots()
{
    if(Step_Number<=0||Regret_Game_Time>6)  return;//步数为0或者连续悔棋6次则不作操作

    int Eat_x=Regret_Chessman_Array[Step_Number%6][0];
    int Eat_y=Regret_Chessman_Array[Step_Number%6][1];
    Chessman_Coordinate[Eat_x][Eat_y]=Regret_Chessman_Array[Step_Number%6][2];
    int Eaten_x=Regret_Chessman_Array[Step_Number%6][3];
    int Eaten_y=Regret_Chessman_Array[Step_Number%6][4];
    Chessman_Coordinate[Eaten_x][Eaten_y]=Regret_Chessman_Array[Step_Number%6][5];
    Initialize_Move_Array();
    //恢复步数操作
    Clicked_X=Regret_Chessman_Array[Step_Number%6][0];
    Clicked_Y=Regret_Chessman_Array[Step_Number%6][1];
    Step_Number--;
    Regret_Game_Time++;

    this->repaint();
}

void Widget::Game_Help_Slots()
{
    QMessageBox::information(this,"游戏帮助:","你需要遵守如下规则:\n"
                                          "将军不离九宫内，士止相随不出官。\n"
                                          "象飞四方营四角，马行一步一尖冲。\n"
                                          "炮须隔子打一子，车行直路任西东。\n"
                                          "唯卒只能行一步，过河横进退无踪。\n"
                                           "黑棋先手。\n"
                                          "(悔棋一次不能超过六步,蹩脚马不可移动。)"
                                          ,QMessageBox::Yes);
}

void Widget::Exit_Game_Slots()
{
    if(QMessageBox::Yes==QMessageBox::question(this,"提示：","你想要离开游戏吗?",QMessageBox::Yes|QMessageBox::No))
    {
        exit(0);
    }
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    for(int i=0;i<9;i++)
    {//画出所有棋子的位置
        for(int j=0;j<10;j++)
        {
            switch (Chessman_Coordinate[i][j])
            {
            case 1:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/黑将.png"));  break;
            case 2:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/黑士.png"));  break;
            case 3:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/黑相.png"));  break;
            case 4:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/黑马.png"));  break;
            case 5:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/黑車.png"));  break;
            case 6:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/黑炮.png"));  break;
            case 7:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/黑兵.png"));  break;
            case -1:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/红帅.png"));  break;
            case -2:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/红士.png"));  break;
            case -3:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/红相.png"));  break;
            case -4:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/红马.png"));  break;
            case -5:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/红車.png"));  break;
            case -6:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/红炮.png"));  break;
            case -7:painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/红兵.png"));  break;
            default:       break;
            }
            if(To_Move[i][j]==10)
            {//画出所有可以移动棋子的位置
                painter.drawImage(QRect(10+100*i,10+100*j,80,80),QImage(":/Chessman/img/待移动位置.png"));
            }
        }
    }//画出对方上次移动的位置
    painter.drawImage(QRect(10+100*Clicked_X,10+100*Clicked_Y,80,80),QImage(":/Chessman/img/选中棋子.png"));
}

