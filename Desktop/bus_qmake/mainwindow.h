
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//
#ifndef QtStuff
    #define QtStuff
    #include <iostream>
    #include <windows.h>
    #include <QDebug>
    #include <QMainWindow>
    #include <QPainter>
    #include <QLabel>
    #include <QTextBrowser>
    #include <QPushButton>
    #include <QPropertyAnimation>
    #include <QMessageBox>
    #include <QThread>
    #include <QTimer>
#endif //QtStuff

#include <math.h>
#include <string>
#include <windows.h>
#include <thread>
#include <mutex>

QString array2QString(int *arr, int size);

const int CLOCK = 5000;
const int ANIMATION_DURATION = 4000;
const QString staBusy = "font: 11pt \"Cascadia Code\"; color: rgb(41,36,33); min-width: 20px; min-height: 20px; max-width:20px; max-height: 20px; border-radius: 10px; border:1px red; background:red";
const QString staPass = "font: 11pt \"Cascadia Code\"; color: rgb(41,36,33); min-width: 20px; min-height: 20px; max-width:20px; max-height: 20px; border-radius: 10px; border:1px rgb(255,182,10); background:rgb(255,182,10)";
const QString staFree = "font: 11pt \"Cascadia Code\"; color: rgb(41,36,33); min-width: 20px; min-height: 20px; max-width:20px; max-height: 20px; border-radius: 10px; border:1px green; background-color:rgb(36,230,86); color black";
const QString stopPass = "min-width: 12px; min-height: 12px; max-width:12px; max-height: 12px; border-radius: 6px; border:1px rgb(255,182,10); background:rgb(255,182,10)";
const QString stopFree = "min-width: 12px; min-height: 12px; max-width:12px; max-height: 12px; border-radius: 6px; border:1px green; background-color:rgb(36,230,86)";
const QVector<QPair<int, int>> scale = {{2, 1}, {3, 2}, {4, 3}, {1, 1}};
//
extern std::mutex m;
// linklist
typedef struct req {
    int type; // type 1:clockwise;type 2:counterclockwise;type 3:target
    int aim;
    struct req *next;
} req;
//
req *initReq();
void add(req *head, int type, int aim);
void del(req *head, req *to_del);
void delAimx(req *head, int aim, int type);
void reset(req *head);
//
QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
    class canvas;
    class bus;
    class busStop;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void start();
    void initparam();
    void setCanvas();
    void stop();
    void goOn();
    void end();
    void input(QString s, int station);
signals:
    void signal1();
    void signal3();
    void startAnim();
    void animate2();
public:
    int TOTAL_STATION;
    int STRATEGY; // FCFS:0  SSTF:1  SCAN:2
    int DISTANCE;
    float kt;
    QPropertyAnimation *anim;
    static MainWindow *self;
    std::unique_lock<std::mutex> lock2(std::mutex m);
public slots:
    void output();
    void appendClock();
    int animate(bool toTurn = 0);
private slots:
    void on_InputInfo_blockCountChanged(int newBlockCount);

private:
    void showParam();
private:
    Ui::MainWindow *ui;
    std::mutex mOutputInfo;
};
class bus: public QLabel {
    Q_OBJECT
public:
    bus(int TOTAL_STATION, MainWindow *w, QWidget *parent = 0);
    void initBus(int TOTAL_STATION, MainWindow *w);
    ~bus();
public:
    void refreshPixmap(int x1, int x2, int y1, int y2, int xNow, int xNext, int yNow, int yNext);
    void _refreshPixmap(const QString x);
    void setThread();
    static void out(bus *bus);
    //static DWORD WINAPI Fun2Proc(LPVOID lpParameter);
    void serve();
    void drive();
    //dispatch
    int get_direction(int aim);
    int getNextReq();
    void FCFS();
    int regain_aim( int pos );
    int direction_decision( int aim );
    void getBy( int totDistance, int distance );
    void SSTF();
    req *getShortest();
    int getDirection(req *aimNow);
    void refreshAim();
    void SCAN();
signals:
    void signal2();
public:
    QLabel *Bus;
    int time;
    int position;
    int direction; // clockwise:1  counterclockwise:0
    int aim;
    int isStop; // 0:running  1:to stop  2:stop for 1 sec  3:to start again
    int *target;
    int *clockwise;
    int *counterclockwise;
    req *request;
    int dist;
    //QList<req> request;
    MainWindow *w;
    static class bus *self;
    bool isEnded;
};
class busStop : public QLabel {
    Q_OBJECT

public:
    busStop(int x, int y, int isStation = 0, QWidget *parent = 0);
    ~busStop();
    void busy();
    void pass();
    void free();
public:
    int x;
    int y;
    int isStation;
private:
    QLabel *stop;
};
class canvas : public QWidget {
    Q_OBJECT

public:
    //canvas(int x1, int x2, int y1, int y2, int numStation, QWidget *parent=0);
    canvas(QWidget *parent = 0);
    ~canvas();
    void setPos(int posx1, int posx2, int posy1, int posy2);
    void setBus(int TOTAL_STATION, int DISTANCE);
    void setBusStops(int TOTAL_STATION, int DISTANCE);
    virtual void paintEvent(QPaintEvent *event);
public:
    bool isPainting;
    MainWindow *w;
    class bus *bus;
    QVector<busStop *> busStops;
    int x1, x2, y1, y2;
private:
    QWidget *canv;
};


#endif //UI_MAINWINDOW_H
