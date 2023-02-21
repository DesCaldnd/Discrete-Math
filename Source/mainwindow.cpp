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
#include "../Headers/Variable.h"


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

std::vector<ExpressionSymbol> MainWindow::expr_to_postfix(const QString &string) {
    variables.clear();

    std::vector<ExpressionSymbol> answer;
    std::stack<Operation> operationStack;

    for (int i = 0; i < string.length(); i++){
        char sym = string[i].toLatin1();
        switch (symType(sym)) {
            case Var:
                answer.push_back(Variable(sym, i));
                if (!hasVar(sym))
                    variables.push_back(sym);
                break;
            case Oper:
                    int order = Operation::order(sym);
                    while (operationStack.top().getOrder() > order && operationStack.size() != 0){
                        answer.push_back(operationStack.top());
                        operationStack.pop();
                    }
                    operationStack.push(Operation{sym, i});
                break;
            case OpenBracket:
                    operationStack.push(Operation{sym});
                break;
            case CloseBracket:
                while (operationStack.top().getSymbol() != '('){
                    answer.push_back(operationStack.top());
                    operationStack.pop();
                }
                operationStack.pop();
                break;
        }
    }

    return answer;
}

MainWindow::SymType MainWindow::symType(char symbol) {
    if((symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z'))
        return MainWindow::SymType::Var;
    else if (symbol == '(')
        return MainWindow::SymType::OpenBracket;
    else if (symbol == ')')
        return MainWindow::SymType ::CloseBracket;
    return MainWindow::SymType::Oper;
}

bool MainWindow::hasVar(char var) {
    for(int i = 0; i < variables.size(); i++){
        if (variables[i] == var)
            return true;
    }
    return false;
}
