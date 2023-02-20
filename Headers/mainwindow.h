//
// Created by Des Caldnd on 2/19/2023.
//

#ifndef DISCRETEMATH_MAINWINDOW_H
#define DISCRETEMATH_MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>

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

    bool check_string_for_brackets(const QString&);

    QString expr_to_postfix(const QString&);

    QMessageBox errorMessageBox{this};

private slots:

    void eval_button_clicked();


};


#endif //DISCRETEMATH_MAINWINDOW_H
