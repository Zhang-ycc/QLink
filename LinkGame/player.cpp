#include <player.h>

Player::Player()
{
    total_link = 0;
    iflink = 0;
    other_ifdizzy = 0;
    other_canmove = 1;
}

void Player::setPlayer(int n){
    total_link = 0;
    iflink = 0;
    other_ifdizzy = 0;
    other_canmove = 1;
    number = n;
    if (n==1){
        x =250;
        y =250;
    }
    else if (n==2){
        x =1550;
        y =250;
    }
}

void Player::chooseBlock(int k)
{
    int i = (x-350)/w->length;
    int j= (y-110)/w->length;
    if (x>=350 && x<=1450 && y>=110 && y <= 1210)
        if (w->map_2[i][j] !=0){
            int help = w->map_2[i][j];
            w->map_2[i][j] = 0;
            proHelp(help);
        }
    int thisx = -1 ,thisy = -1;
    switch (k) {
    case 1:
        if (x == (w->length*i+370)){
            thisx = i-1;
            thisy = j;
        }
        break;
    case 2:
        if (x == (w->length*(i+1)+330)){
            thisx = i+1;
            thisy = j;
        }
        break;
    case 3:
        if (y == (w->length*j+130)){
            thisx = i;
            thisy = j-1;
        }
        break;
    case 4:
        if (y == (w->length*(j+1)+90)){
            thisx = i;
            thisy = j+1;
        }
        break;
    }
    if (thisx >= 1 && thisx <= w->width && thisy >= 1 && thisy <= w->width)
        chooseLink(thisx,thisy);
}

void Player::chooseLink(int X, int Y)
{
    if ((w->map[X][Y] >0 && w->map[X][Y] <10)||(w->map[X][Y] >20 && w->map[X][Y] <30))
    {
        if (number ==1)
            w->map[X][Y] = 10+(w->map[X][Y]%10);
        else
            w->map[X][Y] = 100+(w->map[X][Y]%10);
        if (!iflink){
            fx = X;
            fy = Y;
            iflink = 1;
        }
        else{
            ifLink(fx,fy,X,Y);
            iflink = 0;
        }
    }
}

void Player::ifLink(int fx, int fy, int sx, int sy)
{
    if (w->canLink(fx,fy,sx,sy)){
        w->drawLink(fx,fy,sx,sy);
        total_link += w->map[fx][fy]%10;
        w->disapear(fx,fy,sx,sy);
    }
    else{
        w->sleep(30);
        w->map[fx][fy] %= 10;
        w->map[sx][sy] %= 10;
    }
    w->ifDead();
}

void Player::proHelp(int k){
    switch(k){
    case 10001:
        w->timePlus();
        break;
    case 10000:
        w->shuffle();
        break;
    case 10002:
        w->hint_time = 1;
        w->prehint = 1;
        QTimer::singleShot(20000,this,SLOT(dehint()));
        break;
    case 10003:
        other_ifdizzy = 1;
        QTimer::singleShot(10000,this,SLOT(dedizzy()));
        break;
    case 10004:
        other_canmove = 0;
        QTimer::singleShot(3000,this,SLOT(defreeze()));
        break;
    }   
}

void Player::defreeze()
{
    other_canmove = 1;
}

void Player::dedizzy()
{
    other_ifdizzy = 0;
}

void Player::dehint()
{
    w->hint_time = 0;
}



