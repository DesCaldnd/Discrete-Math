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
    setMinimumSize(950, 500);
    QRegularExpressionValidator *validator = new QRegularExpressionValidator{QRegularExpression{"^([-]?[\(]{1})*[-]?[a-zA-Z10]?(([&|+</~])([-]?[\(]{1})*([-]?[a-zA-Z10]?)([\)]{1})*)*$"}, this};
//    ^[-]?[\(]*[-]?[a-zA-Z10]?(([&|+<\\])[-]?[\(]*([-]?[a-zA-Z10]?)[\)]*)*$
    ui->expr_edit->setValidator(validator);

    connect(ui->eval_button, &QPushButton::clicked, this, &MainWindow::eval_button_clicked);
    connect(ui->expr_edit, &QLineEdit::returnPressed, this, &MainWindow::eval_button_clicked);

    errorMessageBox.setWindowTitle("Error");
    ui->answer_label->hide();
    ui->statusbar->hide();
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
    if (!check_string_for_operators(string)){
        errorMessageBox.setText("More than one operator in a row");
        errorMessageBox.exec();
        return;
    }
    if (!check_string_for_end(string)){
        errorMessageBox.setText("This expression is not completed");
        errorMessageBox.exec();
        return;
    }
    auto expression = expr_to_postfix(string);
    QString x;
    for(int i = 0; i < expression.size(); i++){
        x.append(expression[i]->getSymbol());
    }
    qDebug() << x;
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

std::vector<ExpressionSymbol*> MainWindow::expr_to_postfix(const QString &string) {
    variables.clear();
    operCount = 0;

    std::vector<ExpressionSymbol*> answer;
    std::stack<Operation> operationStack;

    for (int i = 0; i < string.length(); i++){
        char sym = string[i].toLatin1();
        switch (symType(sym)) {
            case Var: {
                answer.push_back(new Variable(sym, i));
                if (!hasVar(sym))
                    variables.push_back(Variable{sym});
                break;
            }
            case Oper: {
                int order = Operation::order(sym);
                while (operationStack.size() != 0) {
                    if (operationStack.top().getOrder() > order) {
                        answer.push_back(new Operation(operationStack.top()));
                        operationStack.pop();
                    } else
                        break;
                }
                operationStack.push(Operation{sym, i});
                operCount++;
                break;
            }
            case OpenBracket: {
                operationStack.push(Operation{sym});
                break;
            }
            case CloseBracket: {
                while (operationStack.top().getSymbol() != '(') {
                    answer.push_back(new Operation(operationStack.top()));
                    operationStack.pop();
                }
                operationStack.pop();
                break;
            }
            case Constant: {
                answer.push_back(new Variable(sym, i));
                break;
            }
        }
    }

    while (operationStack.size() != 0) {
        answer.push_back(new Operation(operationStack.top()));
        operationStack.pop();
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
    else if (symbol == '1' || symbol == '0')
        return MainWindow::SymType::Constant;
    return MainWindow::SymType::Oper;
}

bool MainWindow::hasVar(char var) {
    for(int i = 0; i < variables.size(); i++){
        if (variables[i].getSymbol() == var)
            return true;
    }
    return false;
}

bool MainWindow::check_string_for_operators(const QString &string) {
    bool isOperator = false;
    for (int i = 0; i < string.size(); i++){
        char sym = string[i].toLatin1();
        if (sym == '&' || sym == '|' || sym == '/' || sym == '+' || sym == '<' || sym == '~'){
            if (isOperator)
                return false;
            isOperator = true;
        } else if(sym != '-')
            isOperator = false;
    }
    return true;
}

bool MainWindow::check_string_for_end(const QString &string) {
    bool hasFirstOperand = false, isOper = false;
    for (int i = 0; i < string.size(); i++){
        char sym = string[i].toLatin1();
        switch (symType(sym)) {
            case Oper:{
                if (sym != '-'){
                    if (!hasFirstOperand){
                        return false;
                    } else {
                        hasFirstOperand = false;
                    }
                }
                isOper = true;
                break;
            }
            case OpenBracket:{
                isOper = false;
                hasFirstOperand = false;
                break;
            }
            case CloseBracket:{
                if(isOper){
                    return false;
                }
                hasFirstOperand = true;
                break;
            }
            default:{
                if (hasFirstOperand)
                    return false;
                hasFirstOperand = true;
                isOper = false;
                break;
            }
        }
    }
    return true;
}

void MainWindow::evaluate_expression(const QString &expression) {
    for(unsigned int i = power_of_2(variables.size()) - 1; i >= 0; i--){
        for(int j = 0; j < variables.size(); j++){
            variables[j].value = (1u << variables.size() - (j + 1)) & i;
        }

    }
}

unsigned int MainWindow::power_of_2(unsigned int pow) {
    unsigned int result = 1;
    for (int i = 0; i < pow; i++){
        result *= 2u;
    }
    return result;
}

QString MainWindow::change_var_to_value(QString expression) {
    for(int i = 0; i < expression.length(); i++){
        if(symType(expression[i].toLatin1()) == MainWindow::SymType::Var){
            expression[i] = QChar(value_of_var(expression[i]));
        }
    }
    return QString();
}

bool MainWindow::value_of_var(char var) {
    for (int i = 0; i < variables.size(); i++){
        if (var == variables[i].getSymbol())
            return variables[i].value;
    }
}
