#ifndef PLAYER_H
#define PLAYER_H

#include "mainwindow.h"
#include <iostream>
#include <stdio.h>
using namespace std;

class Player: public QObject
{
    Q_OBJECT
public:
    int x;//位置横坐标
    int y;//位置纵坐标
    int number;//玩家编号
    int total_link;//分数
    bool iflink;//是否已选中一个
    int fx;//上一选中行号
    int fy;//上一选中列号
    bool other_canmove;//对方是否可以移动
    bool other_ifdizzy;//对方是否眩晕
    MainWindow *w;

    Player();
    void setPlayer(int n);//初始化
    void chooseBlock(int k);//选中方块
    void chooseLink(int X, int Y);//选中
    void ifLink(int fx, int fy, int sx, int sy);//判断是否连接
    void proHelp(int k);//道具

public slots:
    void defreeze();//解冻
    void dedizzy();//解眩晕
    void dehint();//解提示
};

#endif // PLAYER_H
