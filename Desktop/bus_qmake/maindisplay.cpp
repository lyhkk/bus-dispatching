#include "mainwindow.h"
#include "./ui_mainwindow.h"

void MainWindow::start() {
    self = this;
    initparam();
    showParam();
    setCanvas();
    this->setFixedSize(this->width(), this->height());
    disconnect(ui->clock, &QPushButton::clicked, this, &MainWindow::start);
    connect(ui->clock, &QPushButton::clicked, this, &MainWindow::stop);
    connect(ui->paint->bus, SIGNAL(signal2()), this, SLOT(animate()));
    connect(ui->paint->bus, SIGNAL(signal2()), this, SLOT(output()));
    connect(ui->paint->bus, SIGNAL(signal2()), this, SLOT(appendClock()));
    connect(this, SIGNAL(signal1()), this, SLOT(output()));
    connect(this, &MainWindow::animate2, this, [ = ]() {
        animate(1);
    });
    connect(this, SIGNAL(signal3()), this, SLOT(appendClock()));
    ui->OutputInfo->clear();
    ui->InputInfo->clear();
    ui->clock->setText("暂停");
    ui->end->setEnabled(true);
    ui->clockwise->setEnabled(true);
    ui->counterclockwise->setEnabled(true);
    ui->target->setEnabled(true);
    ui->InputInfo->setEnabled(true);
    ui->selectStation->setValue(1);
    ui->selectStation->setMaximum(TOTAL_STATION);
    ui->paint->bus->setThread();
    /*anim = new QPropertyAnimation();
    anim->setTargetObject(ui->paint->bus);
    anim->setPropertyName("pos");
    anim->setDuration(1500);
    anim->setStartValue(ui->paint->bus->pos());
    anim->setEndValue(ui->paint->bus->pos() + QPoint(114, 0));
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    group->addAnimation(anim);*/
}
void MainWindow::initparam() {
    TOTAL_STATION = 5;
    STRATEGY = 0;
    DISTANCE = 2;
    FILE *fp = fopen("C:\\Users\\86136\\Desktop\\bus_qmake\\dict.dic", "r");
    if (fp == NULL) {
        qDebug() << "Error: unable to read dict.dic" << endl;
        return;
    }
    char t[50] = {0};
    while (!feof(fp)) {
        fgets(t, 49, fp);
        if (strncmp(t, "TOTAL_STATION", 13) == 0) {
            TOTAL_STATION = atoi(t + 16);
        }
        if (strncmp(t, "STRATEGY", 8) == 0) {
            if (strncmp(t + 11, "FCFS", 4) == 0) {
                STRATEGY = 0;
            }
            if (strncmp(t + 11, "SSTF", 4) == 0) {
                STRATEGY = 1;
            }
            if (strncmp(t + 11, "SCAN", 4) == 0) {
                STRATEGY = 2;
            }
        }
        if (strncmp(t, "DISTANCE", 8) == 0) {
            DISTANCE = atoi(t + 11);
        }
    }
    if (TOTAL_STATION <= 1 || TOTAL_STATION > 20 || DISTANCE <= 0 || DISTANCE >= 6) {
        TOTAL_STATION = 5, STRATEGY = 0, DISTANCE = 2;
        qDebug() << "Error: incorrect parameters" << endl;
    }
    fclose(fp);
}

void MainWindow::setCanvas() {
    ui->paint->w = this;
    int posx1 = 70;
    int posx2 = ui->paint->geometry().width() - 90;
    int posy1 = 80;
    int posy2 = ui->paint->geometry().height() - 60;
    int awidth = posx2 - posx1;
    int aheight = posy2 - posy1;
    qDebug() << posx1 << posx2 << posy1 << posy2 << endl;
    int i = 0;
    for(; i < scale.size(); i++) {
        if((TOTAL_STATION * DISTANCE) % (scale[i].first + scale[i].second) == 0) {
            qDebug() << scale[i].first << scale[i].second << endl;
            break;
        }
    }
    int k = TOTAL_STATION * DISTANCE / (scale[i].first + scale[i].second);
    int l = TOTAL_STATION * DISTANCE * ceil((awidth + aheight) / (TOTAL_STATION * DISTANCE));
    if(i < scale.size()) {
        int bwidth = k * scale[i].first * ceil(awidth / (k * scale[i].first));
        int bheight = bwidth  * scale[i].second / scale[i].first;
        int cheight = k * scale[i].second * ceil(aheight / (k * scale[i].second));
        int cwidth = cheight  * scale[i].first / scale[i].second;
        if(bheight >= aheight)
            qDebug() << ">" << endl,
                     this->setGeometry(this->geometry().x(), this->geometry().y(), this->geometry().width() - awidth + bwidth, this->geometry().height() - aheight + bheight),
                     ui->paint->setPos(posx1, posx2 - awidth + bwidth, posy1, posy2 - aheight + bheight);
        else
            this->setGeometry(this->geometry().x(), this->geometry().y(), this->geometry().width() - awidth + cwidth, this->geometry().height() - aheight + cheight),
                 ui->paint->setPos(posx1, posx2 - awidth + cwidth, posy1, posy2 - aheight + cheight);
    } else {
        this->setGeometry(this->geometry().x(), this->geometry().y(), this->geometry().width(), this->geometry().height() + l - awidth - aheight);
        ui->paint->setPos(posx1, posx2, posy1, posy2 + l - awidth - aheight);
    }
    ui->paint->isPainting = true;
    ui->paint->setBus(TOTAL_STATION, DISTANCE);
    ui->paint->setBusStops(TOTAL_STATION, DISTANCE);
    ui->paint->repaint();
    ui->paint->w = this;
}
void MainWindow::stop() {
    ui->clock->setText("继续");
    disconnect(ui->clock, &QPushButton::clicked, this, &MainWindow::stop);
    connect(ui->clock, &QPushButton::clicked, this, &MainWindow::goOn);
    m.lock();
}
void MainWindow::goOn() {
    ui->clock->setText("暂停");
    disconnect(ui->clock, &QPushButton::clicked, this, &MainWindow::goOn);
    connect(ui->clock, &QPushButton::clicked, this, &MainWindow::stop);
    m.unlock();
}
void MainWindow::output() {
    //qDebug() << "called output" << endl;
    std::lock_guard<std::mutex> lock3(mOutputInfo);
    Sleep(10);
    ui->OutputInfo->append("<font color=\"red\">TIME:" + QString::number(ui->paint->bus->time));
    Sleep(10);
    ui->OutputInfo->append("position:" + QString::number(ui->paint->bus->position));
    Sleep(10);
    ui->OutputInfo->append("target:" + array2QString(ui->paint->bus->target, TOTAL_STATION));
    Sleep(10);
    ui->OutputInfo->append("clockwise:" + array2QString(ui->paint->bus->clockwise, TOTAL_STATION));
    Sleep(10);
    ui->OutputInfo->append("counterclockwise:" + array2QString(ui->paint->bus->counterclockwise, TOTAL_STATION));
    lock3.~lock_guard();
}
int MainWindow::animate(bool toTurn) {
    static int prePosition = 0;
    static int preDirection = 1;
    static QString prePixmap = ":/bus.png";
    /*if(toTurn == 1) {
        qDebug() << "called animate1" << endl;
    } else {
        qDebug() << "called animate0" << endl;
    }*/
    if(ui->paint->bus->aim == -1 || ui->paint->bus->direction == -1) {
        return 0;
    }
    anim = new QPropertyAnimation();
    anim->setTargetObject(ui->paint->bus);
    anim->setPropertyName("geometry");
    anim->setDuration(ANIMATION_DURATION);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    int posNow = prePosition;
    int posNext = (ui->paint->bus->direction == 1) ? ((posNow + 1) % (TOTAL_STATION * DISTANCE)) : (posNow - 1);
    posNext = (posNext < 0) ? (posNext + TOTAL_STATION * DISTANCE) : posNext;
    prePosition = ui->paint->bus->position;
    qDebug() << "isStop:" << ui->paint->bus->isStop << "posNow: " << posNow << " posNext: " << posNext << endl;
    ui->paint->busStops[posNow]->free();
    int x1 = ui->paint->x1;
    int x2 = ui->paint->x2;
    int y1 = ui->paint->y1;
    int y2 = ui->paint->y2;
    int xNow = ui->paint->busStops[posNow]->x;
    int yNow = ui->paint->busStops[posNow]->y;
    int xNext = ui->paint->busStops[posNext]->x;
    int yNext = ui->paint->busStops[posNext]->y;
    //qDebug() << "xNow:" << xNow << "yNow:" << yNow << "xNext:" << xNext << "yNext:" << yNext << endl;
    if(!((xNow == xNext || yNow == yNext)) && toTurn == 0) {
        anim->setStartValue(QRect(ui->paint->bus->x(), ui->paint->bus->y(), ui->paint->bus->width(), ui->paint->bus->height()));
        anim->setEasingCurve(QEasingCurve::InSine);
        if(xNext == x1 || xNext == x2) {
            anim->setEndValue(QRect(ui->paint->bus->x() - xNow + xNext, ui->paint->bus->y(), ui->paint->bus->width(), ui->paint->bus->height()));
            kt = abs(xNext - xNow) / ui->paint->bus->dist;
            anim->setDuration(ANIMATION_DURATION * kt);
            //xNow = xNext;
        }
        if(yNext == y1 || yNext == y2) {
            anim->setEndValue(QRect(ui->paint->bus->x(), ui->paint->bus->y() - yNow + yNext, ui->paint->bus->width(), ui->paint->bus->height()));
            kt = abs(yNext - yNow) / ui->paint->bus->dist;
            anim->setDuration(ANIMATION_DURATION * kt);
            //yNow = yNext;
        }
    } else {
        if(!((xNow == xNext || yNow == yNext)) && toTurn == 1) {
            anim->setEasingCurve(QEasingCurve::OutSine);
            anim->setDuration(ANIMATION_DURATION - ANIMATION_DURATION * kt);
            kt = 2;
            if(xNext == x1 || xNext == x2) {
                xNow = xNext;
            }
            if(yNext == y1 || yNext == y2) {
                yNow = yNext;
            }
        }
        if(xNow == xNext || yNow == yNext) {
            if(yNow == yNext && yNow == y1 && xNow == x1) {
                //preDirection=1;
                prePixmap = ":/bus.png";
                QImage pixmap = QImage(prePixmap);
                ui->paint->bus->setPixmap(QPixmap::fromImage(pixmap));
                ui->paint->bus->resize(pixmap.width(), pixmap.height());
                ui->paint->bus->setGeometry(x1 - pixmap.width() / 2, y1 - pixmap.height(), ui->paint->bus->width(), ui->paint->bus->height());
            }
            if(yNow == yNext && yNow == y2 && xNow == x2) {
                //preDirection=1;
                prePixmap = ":/bus180.png";
                QImage pixmap = QImage(prePixmap);
                ui->paint->bus->setPixmap(QPixmap::fromImage(pixmap));
                ui->paint->bus->resize(pixmap.width(), pixmap.height());
                ui->paint->bus->setGeometry(x2 - pixmap.width() / 2, y2, ui->paint->bus->width(), ui->paint->bus->height());
            }
            if(xNow == xNext && xNow == x2 && yNow == y1) {
                //preDirection=1;
                prePixmap = ":/bus90.png";
                QImage pixmap = QImage(prePixmap);
                ui->paint->bus->setPixmap(QPixmap::fromImage(pixmap));
                ui->paint->bus->resize(pixmap.width(), pixmap.height());
                ui->paint->bus->setGeometry(x2, y1 - pixmap.height() / 2, ui->paint->bus->width(), ui->paint->bus->height());
            }
            if(xNow == xNext && xNow == x1 && yNow == y2) {
                preDirection = 1;
                prePixmap = ":/bus270.png";
                QImage pixmap = QImage(prePixmap);
                ui->paint->bus->setPixmap(QPixmap::fromImage(pixmap));
                ui->paint->bus->resize(pixmap.width(), pixmap.height());
                ui->paint->bus->setGeometry(x1 - pixmap.width(), y2 - pixmap.height() / 2, ui->paint->bus->width(), ui->paint->bus->height());
            }
            if(xNow == xNext && xNow == x1 && yNow == y1) {
                //preDirection=0;
                prePixmap = ":/bus-270.png";
                QImage pixmap = QImage(prePixmap);
                ui->paint->bus->setGeometry(x1 - pixmap.width(), y1 - pixmap.height() / 2, ui->paint->bus->width(), ui->paint->bus->height());
                ui->paint->bus->resize(pixmap.width(), pixmap.height());
                ui->paint->bus->setPixmap(QPixmap::fromImage(pixmap));
            }
            if(xNow == xNext && xNow == x2 && yNow == y2) {
                //preDirection=0;
                prePixmap = ":/bus-90.png";
                QImage pixmap = QImage(prePixmap);
                int hi = pixmap.height();
                ui->paint->bus->setGeometry(x2, y2 - hi / 2, ui->paint->bus->width(), ui->paint->bus->height());
                ui->paint->bus->resize(pixmap.width(), pixmap.height());
                ui->paint->bus->setPixmap(QPixmap::fromImage(pixmap));
            }
            if(yNow == yNext && yNow == y1 && xNow == x2) {
                //preDirection=0;
                prePixmap = ":/bus-.png";
                QImage pixmap = QImage(prePixmap);
                int wi = pixmap.width(), hi = pixmap.height();
                ui->paint->bus->setGeometry(x2 - wi / 2, y1 - hi, ui->paint->bus->width(), ui->paint->bus->height());
                ui->paint->bus->resize(pixmap.width(), pixmap.height());
                ui->paint->bus->setPixmap(QPixmap::fromImage(pixmap));
            }
            if(yNow == yNext && yNow == y2 && xNow == x1) {
                //preDirection=0;
                prePixmap = ":/bus-180.png";
                QImage pixmap = QImage(prePixmap);
                int wi = pixmap.width();
                ui->paint->bus->setGeometry(x1 - wi / 2, y2, ui->paint->bus->width(), ui->paint->bus->height());
                ui->paint->bus->resize(pixmap.width(), pixmap.height());
                ui->paint->bus->setPixmap(QPixmap::fromImage(pixmap));
            }
            ui->paint->bus->refreshPixmap(x1, x2, y1, y2, xNow, xNext, yNow, yNext);
            if(preDirection != ui->paint->bus->direction) {
                if(prePixmap[5] == '-') {
                    prePixmap.remove(5, 1);
                } else {
                    prePixmap.insert(5, QString("-"));
                }
                qDebug() << "prePixmap: " << prePixmap << endl;
                preDirection = ui->paint->bus->direction;
                QImage t = QImage(prePixmap);
                ui->paint->bus->setGeometry(ui->paint->bus->x(), ui->paint->bus->y(), t.width(), t.height());
                ui->paint->bus->resize(t.width(), t.height());
                ui->paint->bus->setPixmap(QPixmap::fromImage(t));
                ui->paint->bus->_refreshPixmap(prePixmap);
            }
            anim->setStartValue(QRect(ui->paint->bus->x(), ui->paint->bus->y(), ui->paint->bus->width(), ui->paint->bus->height()));
            anim->setEndValue(QRect(ui->paint->bus->x() - xNow + xNext, ui->paint->bus->y() - yNow + yNext, ui->paint->bus->width(), ui->paint->bus->height()));
        }
    }
    /*if(ui->paint->bus->direction==1){
        if()
    }else{

    }*/
    //anim->setStartValue(QRect(ui->paint->bus->x(), ui->paint->bus->y(), ui->paint->bus->width(), ui->paint->bus->height()));
    //anim->setEndValue(QRect(ui->paint->bus->x() + ui->paint->bus->dist, ui->paint->bus->y(), ui->paint->bus->width(), ui->paint->bus->height()));
    //ui->paint->bus->show();
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    QTimer::singleShot(3500, this, [ = ]() {
        if(ui->paint->bus->isStop == 1) {
            ui->paint->busStops[posNext]->busy();
        } else {
            ui->paint->busStops[posNext]->pass();
        }
    });
    if(xNow == xNext || yNow == yNext || toTurn == 1) {
        return 0;
    } else {
        //emit animate2();
        return 1;
    }
}
void MainWindow::on_InputInfo_blockCountChanged(int newBlockCount) {
    //qDebug() << "input from box" << endl;
    static QString last = "";
    QString str = ui->InputInfo->document()->toPlainText();
    QString tmp = str.mid(last.size());
    //qDebug() << "last: " << last << endl << "str: " << str << endl << "tmp: " << tmp << endl;
    if(tmp[0] == 'C') {
        input(QString("clockwise"), tmp.mid(1).toInt());
        output();
    }
    if(tmp[0] == 'U') {
        input(QString("counterclockwise"), tmp.mid(1).toInt());
        output();
    }
    if(tmp[0] == 'T') {
        input(QString("target"), tmp.mid(1).toInt());
        output();
    }
    if(tmp[0] == 'E') {
        end();
    }
    //ui->OutputInfo->append(tmp);
    last = str;
}
void MainWindow::input(QString s, int station) {
    station -= 1;
    //qDebug() << "inut waits for lock" << endl;
    bool t;
    t = m.try_lock();
    if(s == "clockwise" && ui->paint->bus->clockwise[station] == 0)
        add(ui->paint->bus->request, 1, station),
            ui->paint->bus->clockwise[station] = 1,
                    qDebug() << "input: clockwise " << station << endl;
    if(s == "counterclockwise" && ui->paint->bus->counterclockwise[station] == 0)
        add(ui->paint->bus->request, 2, station),
            ui->paint->bus->counterclockwise[station] = 1,
                    qDebug() << "input: clockwise " << station << endl;
    if(s == "target" && ui->paint->bus->target[station] == 0)
        add(ui->paint->bus->request, 3, station),
            ui->paint->bus->target[station] = 1,
                                              qDebug() << "input: clockwise " << station << endl;
    if(t != false) {
        m.unlock();
    }
    emit signal1();
}
void MainWindow::appendClock() {
    QString s = ui->InputInfo->document()->toPlainText();
    s.append("clock\n");
    ui->InputInfo->setPlainText(s);
    QTextCursor cursor = ui->InputInfo->textCursor();
    cursor.movePosition(QTextCursor::End) ;
    ui->InputInfo->setTextCursor(cursor);
}
void MainWindow::end() {
    ui->OutputInfo->append("<font color=\"red\">END");
    ui->end->setEnabled(false);
    ui->clockwise->setEnabled(false);
    ui->counterclockwise->setEnabled(false);
    ui->target->setEnabled(false);
    ui->clock->setText("开始");
    ui->InputInfo->setEnabled(false);
    ui->paint->bus->isEnded = true;
    connect(ui->clock, &QPushButton::clicked, this, &MainWindow::start);
    ui->paint->~canvas();
}
void canvas::setPos(int posx1, int posx2, int posy1, int posy2) {
    x1 = posx1, x2 = posx2, y1 = posy1, y2 = posy2;
    qDebug() << x1 << x2 << y1 << y2 << endl;
}
void canvas::setBus(int TOTAL_STATION, int DISTANCE) {
    bus = new class bus::bus(TOTAL_STATION, w, this);
    qDebug() << bus->width() << bus->height() << endl;
    bus->raise();
    bus->setGeometry(x1 - bus->width() / 2, y1 - bus->height(), bus->width(), bus->height());
    bus->show();
    bus->dist = 2 * (x2 - x1 + y2 - y1) / (TOTAL_STATION * DISTANCE);
}
void canvas::setBusStops(int TOTAL_STATION, int DISTANCE) {
    int alen = 2 * (x2 - x1 + y2 - y1) / TOTAL_STATION;
    int blen = 2 * (x2 - x1 + y2 - y1) / (TOTAL_STATION * DISTANCE);
    int l = 0, x = x1, y = y1;
    busStop *p;
    qDebug() << "before busStops.size:" << busStops.size() << endl;
    for(int i = 0; i < TOTAL_STATION; i++) {
        l = i * alen;
        if(l > (2 * x2 - 2 * x1 + y2 - y1)) {
            x = x1, y = y2 - (l - (2 * x2 - 2 * x1 + y2 - y1));
        } else if(l > (x2 - x1 + y2 - y1)) {
            x = x2 - (l - (x2 - x1 + y2 - y1)), y = y2;
        } else if(l > (x2 - x1)) {
            x = x2, y = y1 + (l - (x2 - x1));
        } else {
            x = x1 + l, y = y1;
        }
        x = (abs(x - x1) <= 6) ? x1 : x;
        x = (abs(x - x2) <= 6) ? x2 : x;
        y = (abs(y - y1) <= 6) ? y1 : y;
        y = (abs(y - y2) <= 6) ? y2 : y;
        p = new busStop(x, y, i + 1, this);
        p->setAlignment(Qt::AlignCenter);
        p->setText(QString::number(i + 1));
        busStops.push_back(p);
        for(int j = 0; j < DISTANCE; j++) {
            if(l >= (2 * x2 - 2 * x1 + y2 - y1)) {
                y -= blen;
            } else if(l + blen > (2 * x2 - 2 * x1 + y2 - y1)) {
                x = x1, y = y2 - (l + blen - (2 * x2 - 2 * x1 + y2 - y1));
            } else if(l >= (x2 - x1 + y2 - y1)) {
                x -= blen;
            } else if(l + blen > (x2 - x1 + y2 - y1)) {
                x = x2 - (l + blen - (x2 - x1 + y2 - y1)), y = y2;
            } else if(l >= (x2 - x1)) {
                y += blen;
            } else if(l + blen > (x2 - x1)) {
                x = x2, y = y1 + l + blen - (x2 - x1);
            } else {
                x += blen;
            }
            l += blen;
            if(j == DISTANCE - 1) {
                break;
            }
            p = new busStop(x, y, -1, this);
            busStops.push_back(p);
        }
    }
    qDebug() << "busStops.size:" << busStops.size() << endl;
    for(int i = 0; i < busStops.size(); i++) {
        qDebug() << "busStops[" << i << "]:" << busStops[i]->x << busStops[i]->y << endl;
    }
}
void canvas::paintEvent(QPaintEvent *event) {
    if(isPainting == false) {
        return;
    }
    QPainter paint(this);
    paint.setPen(QPen(Qt::black, 2));
    paint.drawLine(x1, y1, x2, y1);
    paint.drawLine(x2, y1, x2, y2);
    paint.drawLine(x2, y2, x1, y2);
    paint.drawLine(x1, y2, x1, y1);
}
void MainWindow::showParam() {
    QString tmp;
    if(STRATEGY == 0) {
        tmp = "FCFS";
    }
    if(STRATEGY == 1) {
        tmp = "SSTF";
    }
    if(STRATEGY == 2) {
        tmp = "SCAN";
    }
    ui->param->setText("车站数：" + QString::number(TOTAL_STATION) + " 站间距离：" + QString::number(DISTANCE) + " 策略：" + tmp + " ");
}
/*canvas::canvas(int x1, int x2, int y1, int y2, int numStation, QWidget *parent) : QWidget(),x1(x1),x2(x2),y1(y1),y2(y2)
{
    canv=new QWidget;
    canv->setParent(parent);
    canv->setGeometry(x1,y1,x2-x1,y2-y1);
    qDebug()<<x1<<x2<<y1<<y2<<endl;
    canv->show();
    busStops.resize(numStation);
    //busStops[0].
}*/
canvas::canvas(QWidget *parent) : QWidget(parent) {
    canv = new QWidget;
}
canvas::~canvas() {
    delete canv;
}
busStop::busStop(int x, int y, int isStation, QWidget *parent) : QLabel(parent) {
    this->x = x;
    this->y = y;
    stop = new QLabel;
    stop->setParent(parent);
    this->isStation = isStation;
    if(isStation == -1)
        stop->setGeometry(x - 4, y - 4, 8, 8),
             stop->setStyleSheet(stopFree);
    else
        stop->setGeometry(x - 8, y - 8, 16, 16),
             stop->setStyleSheet(staFree),
             stop->setText(QString::number(isStation)),
             stop->setAlignment(Qt::AlignCenter);
    stop->show();
}
busStop::~busStop() {
    delete stop;
}
void busStop::busy() {
    if(isStation != -1) {
        stop->setStyleSheet(staBusy);
    }
}
void busStop::pass() {
    if(isStation == -1) {
        stop->setStyleSheet(stopPass);
    } else {
        stop->setStyleSheet(staPass);
    }
}
void busStop::free() {
    if(isStation == -1) {
        stop->setStyleSheet(stopFree);
    } else {
        stop->setStyleSheet(staFree);
    }
}
QString array2QString(int *arr, int size) {
    QString res;
    for(int i = 0; i < size; i++) {
        res = res + (arr[i] + '0');
    }
    return res;
}
//module functions: [by wwy]

//connect init module to start button:
//(re)initialize parameters, call info change
//initialize variables, send to dispatch parts
//fix mainwindow size
//start output thread, draw thread

//connect stop/continue module to the 2nd button:
//make threads sleep

//connect end module to end button:
//stops threads, free variables
//refresh output
//unfix mainwindow size

//connect command module to right 3 buttons:
//add commands silently, do not affect threads

//animation with corner: single_shot(start_anim)
