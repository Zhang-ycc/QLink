/*#include "test.h"
#include "mainwindow.h"
#include <iostream>
#include <fstream>
using namespace std;

Test::Test()
{
    ifstream file("./in.txt");
    for (int i=0; i<=9; i++)
        for (int j=0; j<=9; j++){
            file >> m.map[i][j];
            m.map_2[i][j]=0;
        }
    file.close();
}

void Test::case0_none()
{
    QVERIFY(m.canbyLine(1,1,3,3)==1);
}

void Test::case1_one()
{
    QVERIFY(m.canbyOne(1,1,3,3)==1);
}

void Test::case2_two()
{
    QVERIFY(m.canbyTwo(1,1,3,3)==1);
}

void Test::case_canLink()
{
    QVERIFY(m.canLink(1,1,3,3)==1);
}*/
