#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "player.h"
int MainWindow::width = 8;
int MainWindow::length = 880/width;
extern Player player1,player2;
const int total_time = 200000;
const int inv = 80;
const int help_time = 4;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);\

    ui->pushButton->installEventFilter(this);

    //设置图标和标题
    setWindowTitle("LinkGame");
    setWindowIcon(QIcon(":icon.ico"));

    //设置窗口大小
    resize(1800, 1200);
    //初始化菜单栏
    createMenus();

    // 设置背景图片
    this->setObjectName("mainWindow");
    this->setStyleSheet("#mainWindow{border-image:url(:image/images/if.jpg);}");

    newGame();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    //过滤button默认对上下左右键的处理
    if (obj == ui->pushButton) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            keyPressEvent(keyEvent);
            return true;
        } else
            return false;
    }
    else
        return QMainWindow::eventFilter(obj, event);
}

void MainWindow::newGame()
{
    chooseMode();

    //生成随机掉落时间
    ifhelp = true;
    n = 0;
    hint_time = 0;
    prehint = 0;

    //进度条初始化
    ui->timebar->setMaximum(total_time);//设置总时间
    ui->timebar->setMinimum(0);
    ui->timebar->setValue(total_time);//设置当前时间
    ui->timebar->setStyleSheet("QProgressBar::chunk{background:#B5D9DC}");

    //分数显示初始化
    ui->lcdNumber_1->setDigitCount(3);
    ui->lcdNumber_1->setMode(QLCDNumber::Dec);
    ui->lcdNumber_1->setStyleSheet("background-color: #DEC674");
    ui->lcdNumber_2->setDigitCount(3);
    ui->lcdNumber_2->setMode(QLCDNumber::Dec);
    ui->lcdNumber_2->setStyleSheet("background-color: #ADD8E6");
    if (mode == 1)
        ui->lcdNumber_2->hide();
    else
        ui->lcdNumber_2->show();

    // 构建一个计时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::update));//自动更新
    connect(timer, SIGNAL(timeout()), this, SLOT(timeEvent()));//进度条进度
    connect(timer, SIGNAL(timeout()), this, SLOT(showScore()));//显示分数
    connect(timer, SIGNAL(timeout()), this, SLOT(dropHelp()));//掉落道具
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(pauseGame()));//暂停键设置
    connect(timer, SIGNAL(timeout()), this, SLOT(hint()));
    timer->start(10);

    flag = 1;//默认开始
    ui->pushButton->setFont(QFont("宋体", 18));
    ui->pushButton->setText("暂停");

    creatMap();//生成地图
}

void MainWindow::showScore()
{
    ui->lcdNumber_1->display(player1.total_link);
    ui->lcdNumber_2->display(player2.total_link);
    //显示分数
}


void MainWindow::timeEvent()
{
    if(ui->timebar->value() == 0)//时间耗尽
    {
        timer->stop();
        QMessageBox::StandardButton result =
                QMessageBox::question(this, "游戏结束",
                              "You Lose !\n要再来一局吗？",
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::NoButton);
        if(result==QMessageBox::No)
            exit(0);
        newGame();
    }
    else
    {
        ui->timebar->setValue(ui->timebar->value() -10);
        if (ui->timebar->value() <= total_time/8)
            ui->timebar->setStyleSheet("QProgressBar::chunk{background:red}");
    }
}

void MainWindow::creatMap()
{
    //生成随机地图
    srand(time(NULL));
    for (int i=0; i<=width+1; i++)
        for (int j=0; j<=width+1; j++)
        {
            if (i==0 || i==width+1 || j==0 || j==width+1 )
                map[i][j]=0;
            else
                map[i][j] = rand()%9+1;
        }

    for (int i=0; i<=width+1; i++)
        for (int j=0; j<=width+1; j++)
         map_2[i][j] = 0;

    //初始化玩家位置
    player1.setPlayer(1);
    if (mode == 2)
        player2.setPlayer(2);
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("存取"));

    QAction *openAct = new QAction(tr("读档"), this);
    fileMenu->addAction(openAct);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);

    QAction *saveasAct = new QAction(tr("存档"), this);
    fileMenu->addAction(saveasAct);
    connect(saveasAct, &QAction::triggered, this, &MainWindow::saveFile);

    QAction *endAct = menuBar()->addAction(tr("退出游戏"));
    connect(endAct, &QAction::triggered, this, &MainWindow::endGame);
}

void MainWindow::endGame()
{
    timer->stop();
    QMessageBox::StandardButton result =
            QMessageBox::question(this, "游戏结束",
                          "要重开一局吗？",
                          QMessageBox::Yes | QMessageBox::No,
                          QMessageBox::NoButton);
    if(result==QMessageBox::No)
        exit(0);
    newGame();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    for (int i=0; i<=width+1; i++)
        for (int j=0; j<=width+1; j++)
        {
            //显示地图
            int x = length*i+350;
            int y = length*j+110;
            if (map_2[i][j] != 0)
                icons[i][j].load(QString::asprintf(":/image/images/%d.png", map_2[i][j]));
            else
                icons[i][j].load(QString::asprintf(":/image/images/%d.png", map[i][j]));
            painter.drawPixmap(x,y, length, length, icons[i][j]);
        }

    //显示玩家1
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QPoint(player1.x,player1.y),20,20);
    //显示玩家2
    if (mode == 2){
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        painter.drawEllipse(QPoint(player2.x,player2.y),20,20);
    }

    painter.end();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (flag){
        if (player2.other_canmove){
            //控制移动player1
            movePlayer_1(event);
        }
        if (player1.other_canmove){
            //player2
            movePlayer_2(event);
        }
    }
}

void MainWindow::movePlayer_1(QKeyEvent *event)
{
    if (!player2.other_ifdizzy){
        if (Qt::Key_A == event->key()){
            player1.chooseBlock(1);
            player1.x -=5;
        }
        if (Qt::Key_D == event->key()){
            player1.chooseBlock(2);
            player1.x +=5;
        }
        if (Qt::Key_W == event->key()){
            player1.chooseBlock(3);
            player1.y -=5;
        }
        if (Qt::Key_S == event->key()){
            player1.chooseBlock(4);
            player1.y +=5;
        }
    }
    else{
        if (Qt::Key_D == event->key()){
            player1.chooseBlock(1);
            player1.x -=5;
        }
        if (Qt::Key_A == event->key()){
            player1.chooseBlock(2);
            player1.x +=5;
        }
        if (Qt::Key_S == event->key()){
            player1.chooseBlock(3);
            player1.y -=5;
        }
        if (Qt::Key_W == event->key()){
            player1.chooseBlock(4);
            player1.y +=5;
        }
    }
}

void MainWindow::movePlayer_2(QKeyEvent *event){
    if (!player1.other_ifdizzy){
        if (Qt::Key_Left == event->key()){
            player2.chooseBlock(1);
            player2.x -=5;
        }
        if (Qt::Key_Right == event->key()){
            player2.chooseBlock(2);
            player2.x +=5;
        }
        if (Qt::Key_Up == event->key()){
            player2.chooseBlock(3);
            player2.y -=5;
        }
        if (Qt::Key_Down == event->key()){
            player2.chooseBlock(4);
            player2.y +=5;
        }
    }
    else{
        if (Qt::Key_Right == event->key()){
            player2.chooseBlock(1);
            player2.x -=5;
        }
        if (Qt::Key_Left == event->key()){
            player2.chooseBlock(2);
            player2.x +=5;
        }
        if (Qt::Key_Down == event->key()){
            player2.chooseBlock(3);
            player2.y -=5;
        }
        if (Qt::Key_Up == event->key()){
            player2.chooseBlock(4);
            player2.y +=5;
        }
    }
}

void MainWindow::openFile()
{
    pauseGame();
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),file.errorString());
            return;
        }

        QDataStream in(&file);
        for (int i=0; i<=width+1; i++)
            for (int j=0; j<=width+1; j++){
                in >> map[i][j];
                in >> map_2[i][j];
            }
        int value;
        in >> value >> mode ;

        in >> player1.x >> player1.y >> player1.fx >> player1.fy >> player1.iflink >>player1.total_link;
        if (mode == 1)
            ui->lcdNumber_2->hide();
        else{
            in >> player2.x >> player2.y >> player2.fx >> player2.fy >> player2.iflink >>player2.total_link;
            ui->lcdNumber_2->show();
        }
        ui->timebar->setValue(value);

        flag = 0;
        timer->stop();
        ui->lcdNumber_1->display(player1.total_link);
        ui->lcdNumber_2->display(player2.total_link);
        ui->pushButton->setText("继续");
    }
}

void MainWindow::saveFile()
{
    pauseGame();

    QString fileName = QFileDialog::getSaveFileName(this);
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"), file.errorString());
            return;
        }

        QDataStream out(&file);
        for (int i=0; i<=width+1; i++)
            for (int j=0; j<=width+1; j++){
                out << map[i][j];
                out << map_2[i][j];
            }
        int value;
        value = ui->timebar->value();
        out << value << mode ;

        out << player1.x << player1.y << player1.fx << player1.fy << player1.iflink << player1.total_link;
        if (mode == 2)
            out << player2.x << player2.y << player2.fx << player2.fy << player2.iflink << player2.total_link;
    }
}

bool MainWindow::canLink(int fx, int fy, int sx, int sy)
{
    if ((map[fx][fy]%10) != (map[sx][sy]%10)){
        return false;
    }//判断图案是否相同
    else{
        if ((map[fx-1][fy]!=0&&map[fx+1][fy]!=0&&map[fx][fy-1]!=0&&map[fx][fy+1]!=0)&&
                (map[sx-1][sy]!=0&&map[sx+1][sy]!=0&&map[sx][sy-1]!=0&&map[sx][sy+1]!=0))
            return false;
        else{
            if (!canbyLine(fx,fy,sx,sy) && !canbyOne(fx,fy,sx,sy) && !canbyTwo(fx,fy,sx,sy)){
                return false;
            }
            else{
                return true;
            }
        }
    }
}

void MainWindow::sleep(int time)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(time, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();
}

void MainWindow::disapear(int fx, int fy, int sx, int sy)
{
    map[fx][fy] = 0;
    map[sx][sy] = 0;
}

bool MainWindow::canbyLine(int fx, int fy, int sx, int sy)
{
    if (fx==sx){
        if (fy>sy)
            std::swap(fy,sy);
        int i = fy;
        while (i<sy){
            i++;
            if (map[fx][i]>0)
                break;
        }
        if (i==sy)
            return true;
    }
    if (fy==sy){
        if (fx>sx)
            std::swap(fx,sx);
        int i = fx;
        while (i<sx){
            i++;
            if (map[i][fy]>0)
                break;
        }
        if (i==sx)
            return true;
    }
    if (fx==sx && abs(fy-sy)==1)
        return true;
    if (fy==sy && abs(fx-sx)==1)
        return true;
    return false;
}

bool MainWindow::canbyOne(int fx, int fy, int sx, int sy)
{
    if (fy>sy){
        std::swap(fx,sx);
        std::swap(fy,sy);
    }
    if (fx<sx){
        if (canbyLine(fx,fy,sx,fy) && canbyLine(sx,fy,sx,sy) && map[sx][fy]==0)//右上折角
            return true;
        if (canbyLine(fx,fy,fx,sy) && canbyLine(fx,sy,sx,sy) && map[fx][sy]==0)//左下折角
            return  true;
    }
    if (fx>sx){
        if (canbyLine(sx,fy,fx,fy) && canbyLine(sx,fy,sx,sy) && map[sx][fy]==0)//左上折角
            return  true;
        if (canbyLine(sx,sy,fx,sy) && canbyLine(fx,sy,sx,sy) && map[fx][sy]==0)//右下折角
            return true;
    }
    return false;
}

bool MainWindow::canbyTwo(int fx, int fy, int sx, int sy)
{
    if (fx>sx){
        std::swap(fx,sx);
        std::swap(fy,sy);
    }
    for (int i=0; i<=width+1; i++){
        if (i==fy || i==sy)
            continue;
        bool b = true;
        for (int j=fx; j<=sx; j++)
            if (map[j][i]!=0)
                b = false;
        if (b)
            if (canbyLine(fx,fy,fx,i) && canbyLine(sx,sy,sx,i))
                return true;
    }
    if (fy>sy){
        std::swap(fx,sx);
        std::swap(fy,sy);
    }
    for (int i=0; i<=width+1; i++){
        if (i==fx || i==sx)
            continue;
        bool b = true;
        for (int j=fy; j<=sy; j++)
            if (map[i][j]!=0)
                b = false;
        if (b)
            if (canbyLine(fx,fy,i,fy) && canbyLine(sx,sy,i,sy))
                return true;
    }
    return false;
}

void MainWindow::drawLink(int fx, int fy, int sx, int sy)
{
    if (canbyLine(fx,fy,sx,sy)){
        drawLine(fx,fy,sx,sy,1);
        sleep(inv);
        drawLine(fx,fy,sx,sy,0);
        return;
    }
    if (canbyOne(fx,fy,sx,sy)){
        drawbyOne(fx,fy,sx,sy);
        return;
    }
    if (canbyTwo(fx,fy,sx,sy)){
        drawbyTwo(fx,fy,sx,sy);
        return;
    }
}

void MainWindow::drawLine(int fx, int fy, int sx, int sy, bool b)
{
    if (fx==sx){
        if (fy>sy)
            std::swap(fy,sy);
        for (int i=fy+1; i<sy; i++)
            if (b)
                map[fx][i] = 1000;//画线
            else map[fx][i] = 0;//擦线
    }
    else if (fy==sy){
        if (fx>sx)
            std::swap(fx,sx);
        for (int i=fx+1; i<sx; i++)
            if (b)
                map[i][fy] = 1001;
            else map[i][fy] = 0;
    }
}

void MainWindow::drawbyOne(int fx, int fy, int sx, int sy)
{
    if (fy>sy){
        std::swap(fx,sx);
        std::swap(fy,sy);
    }
    if (fx<sx){
        if (canbyLine(fx,fy,sx,fy) && canbyLine(sx,fy,sx,sy) && map[sx][fy]==0){
            drawLine(fx,fy,sx,fy,1);
            drawLine(sx,fy,sx,sy,1);
            map[sx][fy] = 1004;
            sleep(inv);
            drawLine(fx,fy,sx,fy,0);
            drawLine(sx,fy,sx,sy,0);
            map[sx][fy] = 0;
            return;
        }
        else if (canbyLine(fx,fy,fx,sy) && canbyLine(fx,sy,sx,sy) && map[fx][sy]==0){
            drawLine(fx,fy,fx,sy,1);
            drawLine(fx,sy,sx,sy,1);
            map[fx][sy] = 1002;
            sleep(inv);
            drawLine(fx,fy,fx,sy,0);
            drawLine(fx,sy,sx,sy,0);
            map[fx][sy] = 0;
            return;
        }
    }
    else if (fx>sx){
        if (canbyLine(sx,fy,fx,fy) && canbyLine(sx,fy,sx,sy) && map[sx][fy]==0){
            drawLine(sx,fy,fx,fy,1);
            drawLine(sx,fy,sx,sy,1);
            map[sx][fy] = 1003;
            sleep(inv);
            drawLine(sx,fy,fx,fy,0);
            drawLine(sx,fy,sx,sy,0);
            map[sx][fy] = 0;
            return;
        }
        else if (canbyLine(sx,sy,fx,sy) && canbyLine(fx,sy,sx,sy) && map[fx][sy]==0){
            drawLine(sx,sy,fx,sy,1);
            drawLine(fx,sy,sx,sy,1);
            map[fx][sy] = 1005;
            sleep(inv);
            drawLine(sx,sy,fx,sy,0);
            drawLine(sx,sy,sx,sy,0);
            map[fx][sy] = 0;
            return;
        }
    }
    return;
}

void MainWindow::drawbyTwo(int fx, int fy, int sx, int sy)
{
    if (fx>sx){
        std::swap(fx,sx);
        std::swap(fy,sy);
    }
    for (int i=0; i<=width+1; i++){
        if (i==fy || i==sy)
            continue;
        bool b = true;
        for (int j=fx; j<=sx; j++)
            if (map[j][i]!=0)
                b = false;
        if (b)
            if (canbyLine(fx,fy,fx,i) && canbyLine(sx,sy,sx,i)){
                int a,b;
                drawLine(fx,i,sx,i,1);
                drawLine(fx,fy,fx,i,1);
                drawLine(sx,sy,sx,i,1);
                if (i<fy && i<sy){
                    a = 1003; b = 1004;
                }
                else if (i>fy && i>sy){
                    a = 1002; b = 1005;
                }
                else if (fy<sy){
                    a = 1002; b = 1004;
                }
                else{
                    a = 1003; b = 1005;
                }
                map[fx][i] = a;
                map[sx][i] = b;
                sleep(inv);
                drawLine(fx,i,sx,i,0);
                drawLine(fx,fy,fx,i,0);
                drawLine(sx,sy,sx,i,0);
                map[fx][i] = 0;
                map[sx][i] = 0;
                return;
            }
    }
    if (fy>sy){
        std::swap(fx,sx);
        std::swap(fy,sy);
    }
    for (int i=0; i<=width+1; i++){
        if (i==fx || i==sx)
            continue;
        bool b = true;
        for (int j=fy; j<=sy; j++)
            if (map[i][j]!=0)
                b = false;
        if (b)
            if (canbyLine(fx,fy,i,fy) && canbyLine(sx,sy,i,sy)){
                int a,b;
                drawLine(i,fy,i,sy,1);
                drawLine(fx,fy,i,fy,1);
                drawLine(sx,sy,i,sy,1);
                if (i<fx && i<sx){
                    a = 1003; b = 1002;
                }
                else if (i>fx && i>sx){
                    a = 1004; b = 1005;
                }
                else if (fx<sx){
                    a = 1004; b = 1002;
                }
                else{
                    a = 1003; b = 1005;
                }
                map[i][fy] = a;
                map[i][sy] = b;
                sleep(inv);
                drawLine(i,fy,i,sy,0);
                drawLine(fx,fy,i,fy,0);
                drawLine(sx,sy,i,sy,0);
                map[i][fy] = 0;
                map[i][sy] = 0;
                return;
            }
    }
    return;
}

void MainWindow::ifDead()
{
    bool b = false;
    for (int i=1; i<=width; i++){
        for (int j=1; j<=width; j++){
            if (map[i][j]!=0){
                for (int m=i; m<=width; m++){
                    for (int n=1; n<=width; n++){
                        if (!(m==i&&n==j))
                            if (canLink(i,j,m,n)){
                                b = true;
                                break;
                            }
                        if (b) break;
                    }
                    if (b) break;
                }
                if (b) break;
            }
            if (b) break;
        }
        if (b) break;
    }
    if (!b){
        sleep(50);
        endDialog();//如果死局，判断结束
    }
}

void MainWindow::endDialog()
{
    timer->stop();
    if (mode ==1){
        QMessageBox::StandardButton result =
                QMessageBox::question(this,"游戏结束",
                             QString::asprintf("You Win !\n Score: %d\n要再来一局吗？",
                                               player1.total_link),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::NoButton);
        if(result==QMessageBox::No)
            exit(0);
    }
    else{
        if (player1.total_link > player2.total_link){
            QMessageBox::StandardButton result =
                    QMessageBox::question(this, "游戏结束",
                                  "Player1 Win !\n要再来一局吗？",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::NoButton);
            if(result==QMessageBox::No)
                exit(0);
        }
        else if (player1.total_link == player2.total_link){
            QMessageBox::StandardButton result =
                    QMessageBox::question(this, "游戏结束",
                                  "Draw !\n要再来一局吗？",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::NoButton);
            if(result==QMessageBox::No)
                exit(0);
        }
        else {
            QMessageBox::StandardButton result =
                    QMessageBox::question(this, "游戏结束",
                                  "Player2 Win !\n要再来一局吗？",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::NoButton);
            if(result==QMessageBox::No)
                exit(0);
        }
    }
    newGame();
}

void MainWindow::chooseMode()
{
    QStringList items;
    items << "单人模式" << "双人模式" ;
    QString dlgTitle = "选择模式";
    QString txtLabel = "请选择模式";
    bool ok = false;

    //默认为单人模式
    QString text = QInputDialog::getItem(this, dlgTitle,
                                         txtLabel, items, 0, false, &ok);
    if(!ok)
        exit(0);

    if (text=="单人模式")
        mode = 1;
    else if (text=="双人模式")
        mode = 2;
}

void MainWindow::pauseGame()
{
    if (flag){
        timer->stop();
        flag = 0;
        ui->pushButton->setText("继续");
    }
    else{
        timer->start();
        flag = 1;
        ui->pushButton->setText("暂停");
    }
}

void MainWindow::timePlus()
{
    if (total_time-ui->timebar->value() <= 30000)
        ui->timebar->setValue(total_time);
    else
        ui->timebar->setValue(ui->timebar->value() +30000);
    if (ui->timebar->value() >  total_time/8)
        ui->timebar->setStyleSheet("QProgressBar::chunk{background:#B5D9DC}");
}

void MainWindow::dropHelp()
{
    //srand((unsigned)time(NULL));
    int p= rand();//随机概率
    if (p <= 20 && ifhelp){
        n++;
        if (n >= help_time)//道具次数是否用尽
            ifhelp = 0;
        bool b = false;
        int i,j;
        while (!b){
            i = rand()%(width+2);
            j = rand()%(width+2);
            if (map[i][j] == 0)
                b = true;
            //随机位置
        }
        int help;
        if (mode ==1)
            help = rand()%3;
        else
            help = rand()%5;
        map_2[i][j] = 10000+3;
        //随机道具
    }
}

void MainWindow::shuffle(){
    player1.iflink = 0;
    player2.iflink = 0;
    vector<int> arr;
    for (int i=1; i<=width; i++)
        for (int j=1; j <=width; j++){
            if (map_2[i][j]!=0){
                arr.push_back(map_2[i][j]);
                map_2[i][j] = 0;
            }
            else
                arr.push_back(map[i][j]%10);
        }
    for (int i=arr.size()-1;i>=0;--i)
    {
        srand((unsigned)time(NULL));
        swap(arr[rand()%(i+1)],arr[i]);
    }
    for (int i=1; i<=width; i++)
        for (int j=1; j <=width; j++)
        {
            if (arr[arr.size()-1]>10000){
                map_2[i][j] = arr[arr.size()-1];
                map[i][j] = 0;
            }
            else
                map[i][j] = arr[arr.size()-1];
            arr.pop_back();
        }

}

void MainWindow::hint()
{
    if (hint_time==1){
        int t = 0,k = 0;
        int m = 0,n = 0;
        for (int i=1; i<=width; i++)
            for (int j=1; j <=width; j++){
                if (map[i][j]>20 && map[i][j]<30){
                    t++;
                    m = i; n = j;
                }
                if ((map[i][j]>10 && map[i][j]<20)||(map[i][j]>100 && map[i][j]<200))
                    k++;
            }
        if (t==1 && k ==0){
            t = 0;
            map[m][n] %= 10;
        }
        if (t==0){
            bool b = false;
            for (int i=1; i<=width; i++){
                for (int j=1; j<=width; j++){
                    if (map[i][j]!=0){
                        for (int m=i; m<=width; m++){
                            for (int n=1; n<=width; n++){
                                if (!(m==i&&n==j))
                                    if (map[i][j]<10 && map[m][n] <10)
                                        if (canLink(i,j,m,n)){
                                            map[i][j] +=20;
                                            map[m][n] +=20;
                                            b = true;
                                            break;
                                    }
                                if (b) break;
                            }
                            if (b) break;
                        }
                        if (b) break;
                    }
                    if (b) break;
                }
                if (b) break;
            }
        }
    }
    else{
        if (prehint == 1)
            for (int i=1; i<=width; i++)
                for (int j=1; j <=width; j++)
                    if (map[i][j]>20 && map[i][j]<30)
                        map[i][j] %= 10;
        prehint = 0;
    }
}



