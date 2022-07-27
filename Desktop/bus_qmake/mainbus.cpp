#include "mainwindow.h"
#include "./ui_mainwindow.h"

bus::bus(int TOTAL_STATION, MainWindow *w, QWidget *parent): QLabel(parent), w(w) {
    Bus = new QLabel(this);
    self = this;
    time = 0;
    position = 0;
    aim = -1;
    direction = -1;
    isStop = 0;
    isEnded = false;
    target = (int *)calloc(TOTAL_STATION, sizeof(int));
    clockwise = (int *)calloc(TOTAL_STATION, sizeof(int));
    counterclockwise = (int *)calloc(TOTAL_STATION, sizeof(int));
    request = initReq();
    //Bus->setStyleSheet("background:transparent");
    //Bus->raise();
    Bus->setAutoFillBackground(true);
    QPixmap pixmap("://bus.png");
    Bus->setPixmap(pixmap);
    Bus->setAlignment(Qt::AlignAbsolute);
    this->resize(pixmap.width(), pixmap.height());
    Bus->resize(pixmap.width(), pixmap.height());
    //pixmap.~QPixmap();
    qDebug() << Bus->width() << Bus->height() << endl;
    Bus->setAlignment(Qt::AlignTop);
}
bus::~bus() {
    delete Bus;
    free(clockwise);
    free(counterclockwise);
    free(target);
}
/*void bus::setThread(bus *bus) { //main按这样写测试正确
    HANDLE hMutex;
    int flag;
    HANDLE hThread2;
    hMutex = CreateMutex(NULL, FALSE, NULL);
    lpParam *tmp = (lpParam *)malloc(sizeof(lpParam));
    tmp->bus = bus;
    tmp->hMutex = hMutex;
    hThread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Fun2Proc, tmp, 0, NULL);
    CloseHandle(hThread2);
    bus->w->output();
    while ((flag = input()) == 1) {
        tmp->bus->w->output();    //有请求就多一个输出
    }
    bus->w->end();
    reset(bus->request);
}*/
void bus::setThread() {
    emit w->signal1();
    std::thread t1(out, self);
    t1.detach();
}
void bus::out(bus *bus) {
    while(1) {
        Sleep(CLOCK);
        //if(bus->w->kt == 2) {
        /*} else {
            Sleep(CLOCK - ANIMATION_DURATION * bus->w->kt);
        }*/
        if(bus->isEnded == true) {
            return;
        }
        //qDebug() << "out: " << bus->time << endl;
        /*if(!m.try_lock()) {
            toSleep = true;
        }*/
        m.lock();
        /*if(toSleep == true) {
            Sleep(CLOCK / 2);
            toSleep = false;
        }*/
        //qDebug() << "isStop:" << bus->isStop << endl;
        bus->time++;
        if(bus->isStop == 0) {
            bus->drive();
            emit bus->signal2();
        } else {
            emit bus->w->signal1();
            emit bus->w->signal3();
        }
        bus->serve();
        m.unlock();
        /*if(bus->w->kt != 2) {
            Sleep(ANIMATION_DURATION * bus->w->kt + 20);
            emit bus->w->animate2();
        }*/
    }
}
/*DWORD WINAPI bus::Fun2Proc(LPVOID lpParameter) {
    lpParam *tmp = (lpParam *)lpParameter;
    while(1) {
        Sleep(2000);//间隔时间（可调）
        tmp->bus->time++;
        WaitForSingleObject(tmp->hMutex, INFINITE);
        //调度过程，与输入并行，不会被输入改变，但互斥锁可能会导致输入被阻塞（微乎其微的可能性）
        if(tmp->bus->isStop == 0) {
            tmp->bus->drive();
        }
        tmp->bus->serve();
        tmp->bus->w->output();
        ReleaseMutex(tmp->hMutex);
    }
    return 0;
}*/
void bus::drive() {
    if (aim == -1) {
        return;
    }
    if (direction == 1) {
        position = (position + 1) % (w->TOTAL_STATION * w->DISTANCE);
    }
    if (direction == 0) {
        position--;
        if (position < 0) {
            position += (w->TOTAL_STATION * w->DISTANCE);
        }
    }
}

void bus::serve() {
    if (w->STRATEGY == 0) {
        FCFS();
    }
    if (w->STRATEGY == 1) {
        SSTF();
    }
    /*if (w->STRATEGY == 2) {
        SCAN();
    }*/
}
void bus::refreshPixmap(int x1, int x2, int y1, int y2, int xNow, int xNext, int yNow, int yNext) {
    if(yNow == yNext && yNow == y1 && xNow == x1) {
        QImage pixmap = QImage(":/bus.png");
        Bus->setPixmap(QPixmap::fromImage(pixmap));
        Bus->resize(pixmap.width(), pixmap.height());
        Bus->setGeometry(x1 - pixmap.width() / 2, y1 - pixmap.height(), Bus->width(), Bus->height());
    }
    if(yNow == yNext && yNow == y2 && xNow == x2) {
        QImage pixmap = QImage(":/bus180.png");
        Bus->setPixmap(QPixmap::fromImage(pixmap));
        Bus->resize(pixmap.width(), pixmap.height());
        Bus->setGeometry(x2 - pixmap.width() / 2, y2, Bus->width(), Bus->height());
    }
    if(xNow == xNext && xNow == x2 && yNow == y1) {
        QImage pixmap = QImage(":/bus90.png");
        Bus->setPixmap(QPixmap::fromImage(pixmap));
        Bus->resize(pixmap.width(), pixmap.height());
        Bus->setGeometry(x2, y1 - pixmap.height() / 2, Bus->width(), Bus->height());
    }
    if(xNow == xNext && xNow == x1 && yNow == y2) {
        QImage pixmap = QImage(":/bus270.png");
        Bus->setPixmap(QPixmap::fromImage(pixmap));
        Bus->resize(pixmap.width(), pixmap.height());
        Bus->setGeometry(x1 - pixmap.width(), y2 - pixmap.height() / 2, Bus->width(), Bus->height());
    }
    if(xNow == xNext && xNow == x1 && yNow == y1) {
        QImage pixmap = QImage(":/bus-270.png");
        Bus->setGeometry(x1 - pixmap.width(), y1 - pixmap.height() / 2, Bus->width(), Bus->height());
        Bus->resize(pixmap.width(), pixmap.height());
        Bus->setPixmap(QPixmap::fromImage(pixmap));
    }
    if(xNow == xNext && xNow == x2 && yNow == y2) {
        QImage pixmap = QImage(":/bus-90.png");
        int hi = pixmap.height();
        Bus->setGeometry(x2, y2 - hi / 2, Bus->width(), Bus->height());
        Bus->resize(pixmap.width(), pixmap.height());
        Bus->setPixmap(QPixmap::fromImage(pixmap));
    }
    if(yNow == yNext && yNow == y1 && xNow == x2) {
        QImage pixmap = QImage(":/bus-.png");
        int wi = pixmap.width(), hi = pixmap.height();
        Bus->setGeometry(x2 - wi / 2, y1 - hi, Bus->width(), Bus->height());
        Bus->resize(pixmap.width(), pixmap.height());
        Bus->setPixmap(QPixmap::fromImage(pixmap));
    }
    if(yNow == yNext && yNow == y2 && xNow == x1) {
        QImage pixmap = QImage(":/bus-180.png");
        int wi = pixmap.width();
        Bus->setGeometry(x1 - wi / 2, y2, Bus->width(), Bus->height());
        Bus->resize(pixmap.width(), pixmap.height());
        Bus->setPixmap(QPixmap::fromImage(pixmap));
    }
}
void bus::_refreshPixmap(const QString x) {
    QImage t = QImage(x);
    Bus->setGeometry(Bus->x(), Bus->y(), t.width(), t.height());
    Bus->resize(t.width(), t.height());
    Bus->setPixmap(QPixmap::fromImage(t));
}
