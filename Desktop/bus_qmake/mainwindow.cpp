#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->end->setEnabled(false);
    connect(ui->clock, &QPushButton::clicked, this, &MainWindow::start);
    connect(ui->end, &QPushButton::clicked, this, &MainWindow::end);
    connect(ui->clockwise, &QPushButton::clicked, this, [this]() {
        input(QString("clockwise"), ui->selectStation->value());
    });
    connect(ui->counterclockwise, &QPushButton::clicked, this, [this]() {
        input(QString("counterclockwise"), ui->selectStation->value());
    });
    connect(ui->target, &QPushButton::clicked, this, [this]() {
        input(QString("target"), ui->selectStation->value());
    });
}

MainWindow::~MainWindow() {
    delete ui;
}


