//
// Created by Des Caldnd on 2/19/2023.
//

#ifndef DISCRETEMATH_MAINWINDOW_H
#define DISCRETEMATH_MAINWINDOW_H

#pragma once

#include <QMainWindow>
#include <QString>
#include "developer.h"
#include "Calculator.h"
#include <QMessageBox>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

	QMessageBox errorMessageBox{this};

private:
    Ui::MainWindow *ui;

	Calculator* calculator = nullptr;

	Developer dev;

private slots:

	void eval_button_clicked();

    void action_clear_expression();

    void action_insert(char);

    void action_backspace();

    void action_developer();

	void show_error(QString);

	void action_save();
};


#endif //DISCRETEMATH_MAINWINDOW_H
