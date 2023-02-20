//
// Created by Des Caldnd on 2/19/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "../Headers/mainwindow.h"
#include "ui_MainWindow.h"
#include <QRegularExpressionValidator>
#include <QPushButton>
#include <QDebug>
#include <stack>
#include "../Headers/Operation.h"


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setMinimumSize(800, 450);
    QRegularExpressionValidator *validator = new QRegularExpressionValidator{QRegularExpression{"^([-]?[\(]{1})*[-]?[a-zA-Z10]?(([&|+<\\~])([-]?[\(]{1})*([-]?[a-zA-Z10]?)([\)]{1})*)*$"}, this};
//    ^[-]?[\(]*[-]?[a-zA-Z10]?(([&|+<\\])[-]?[\(]*([-]?[a-zA-Z10]?)[\)]*)*$
    ui->expr_edit->setValidator(validator);

    connect(ui->eval_button, &QPushButton::clicked, this, &MainWindow::eval_button_clicked);
    connect(ui->expr_edit, &QLineEdit::returnPressed, this, &MainWindow::eval_button_clicked);

    errorMessageBox.setWindowTitle("Error");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::eval_button_clicked() {
    QString string = ui->expr_edit->text();
    if (string.isEmpty()){
        errorMessageBox.setText("Expression is empty");
        errorMessageBox.exec();
        return;
    }
    if (!check_string_for_brackets(string)){
        errorMessageBox.setText("Brackets error");
        errorMessageBox.exec();
        return;
    }
}

bool MainWindow::check_string_for_brackets(const QString &string) {
    int brackets = 0;
    for (int i = 0; i < string.length(); i++){
        if (string[i] == '(')
            brackets++;
        else if (string[i] == ')')
            brackets--;
        if (brackets < 0)
            return false;
    }
    if (brackets == 0)
        return true;
    return false;
}

QString MainWindow::expr_to_postfix(const QString &string) {
    QString answer;
    answer.resize(string.length());
    std::stack<Operation> operationStack;
    return QString();
}
