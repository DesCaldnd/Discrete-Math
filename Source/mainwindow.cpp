//
// Created by Des Caldnd on 2/19/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "../Headers/mainwindow.h"
#include "ui_MainWindow.h"
#include <QRegularExpressionValidator>


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setMinimumSize(800, 450);
    QRegularExpressionValidator validator{QRegularExpression{"^[-]?[\\(]*[-]?[a-zA-Z10]?(([&|+<\\\\])[-]?[\\(]*([-]?[a-zA-Z10]?)[\\)]*)*$"}, this};
//    ^[-]?[\(]*[-]?[a-zA-Z10]?(([&|+<\\])[-]?[\(]*([-]?[a-zA-Z10]?)[\)]*)*$
    ui->expr_edit->setValidator(&validator);
}

MainWindow::~MainWindow() {
    delete ui;
}
