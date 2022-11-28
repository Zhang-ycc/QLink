#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<stdio.h>
#include<stdlib.h>
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QKeyEvent>
#include <QMenuBar>
#include <QFileDialog>
#include <QtWidgets>
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static int width;//地图宽度
    static int length;//方块边长
    int map[10][10];//随机地图
    int map_2[10][10];//道具地图
    int mode;//模式
    bool hint_time;//是否处于提示时间
    bool prehint;//是否上一次提示未清零

    void paintEvent(QPaintEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual bool eventFilter(QObject *obj, QEvent *event) override;//事件过滤器
    void sleep(int time);//延迟

    void movePlayer_1(QKeyEvent *event);//控制移动玩家一
    void movePlayer_2(QKeyEvent *event);//控制移动玩家二

    void createMenus();//菜单栏
    void chooseMode();//选择模式
    void newGame();//游戏初始化
    void creatMap();//生成地图
    void endDialog();//结束窗口

    bool canLink(int fx, int fy, int sx, int sy);//判断两个方块是否可以连接消去
    void disapear(int fx, int fy, int sx, int sy);//方块消失
    bool canbyLine(int fx, int fy, int sx, int sy);//判断是否可通过一条直线连接
    bool canbyOne(int fx, int fy, int sx, int sy);//判断是否可折一次连接
    bool canbyTwo(int fx, int fy, int sx, int sy);//判断是否可折两次连接
    void ifDead();//判断是否死局

    void drawLink(int fx, int fy, int sx, int sy);//画线
    void drawLine(int fx, int fy, int sx, int sy, bool b);//单线
    void drawbyOne(int fx, int fy, int sx, int sy);//一次折线
    void drawbyTwo(int fx, int fy, int sx, int sy);//两次折线

    //道具函数
    void timePlus();//+1
    void shuffle();//重排

private:
    Ui::MainWindow *ui;
    QTimer *timer;//计时器
    bool flag;//判断是否暂停
    QPixmap icons[10][10];//图片数组
    bool ifhelp;//道具次数是否剩余
    int n;//道具已出现次数

protected:


private slots:
    void openFile();//读档
    void saveFile();//存档
    void timeEvent();//进度条
    void endGame();//时间耗尽游戏结束
    void showScore();//显示分数
    void pauseGame();//暂停游戏
    void dropHelp();//掉落道具
    void hint();//提示
};


#endif // MAINWINDOW_H
