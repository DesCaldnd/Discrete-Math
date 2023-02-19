//
// Created by Des Caldnd on 2/19/2023.
//

#ifndef DISCRETEMATH_MAINWINDOW_H
#define DISCRETEMATH_MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    Ui::MainWindow *ui;


};


#endif //DISCRETEMATH_MAINWINDOW_H
