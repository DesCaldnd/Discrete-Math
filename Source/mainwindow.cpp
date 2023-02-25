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


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setMinimumSize(500, 250);
    QRegularExpressionValidator *validator = new QRegularExpressionValidator{QRegularExpression{"[A-Za-z10()\\-&|<~+\\/]*"}, this};
//    ^[-]?[\(]*[-]?[a-zA-Z10]?(([&|+<\\])[-]?[\(]*([-]?[a-zA-Z10]?)[\)]*)*$
    ui->expr_edit->setValidator(validator);

    connect(ui->eval_button, &QPushButton::clicked, this, &MainWindow::eval_button_clicked);
    connect(ui->expr_edit, &QLineEdit::returnPressed, this, &MainWindow::eval_button_clicked);

    connect(ui->actionClear_expression, &QAction::triggered, this, &MainWindow::action_clear_expression);
    connect(ui->actionNegative, &QAction::triggered, [&](){ action_insert('-'); });
    connect(ui->actionConjunction, &QAction::triggered, [&](){ action_insert('&'); });
    connect(ui->actionDisjunction, &QAction::triggered, [&](){ action_insert('|'); });
    connect(ui->actionImplication, &QAction::triggered, [&](){ action_insert('<'); });
    connect(ui->actionEquivalent, &QAction::triggered, [&](){ action_insert('~'); });
    connect(ui->actionSymmetric_Complement, &QAction::triggered, [&](){ action_insert('+'); });
    connect(ui->actionRealative_Complement, &QAction::triggered, [&](){ action_insert('/'); });
    connect(ui->actionOpen_Bracket, &QAction::triggered, [&](){ action_insert('('); });
    connect(ui->actionClose_Bracket, &QAction::triggered, [&](){ action_insert(')'); });
    connect(ui->actionA, &QAction::triggered, [&](){ action_insert('A'); });
    connect(ui->actionB, &QAction::triggered, [&](){ action_insert('B'); });
    connect(ui->actionC, &QAction::triggered, [&](){ action_insert('C'); });
    connect(ui->actionBackspace, &QAction::triggered, this, &MainWindow::action_backspace);
    connect(ui->actionDeveloper, &QAction::triggered, this, &MainWindow::action_developer);


    errorMessageBox.setWindowTitle("Error");
//    ui->answer_label->hide();
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
//    QString x;
//    for(int i = 0; i < expression.size(); i++){
//        x.append(expression[i]->getSymbol());
//    }
//    qDebug() << x;
    if (variables.size() > 63){
        errorMessageBox.setText("Too many variables (max=63)");
        errorMessageBox.exec();
        return;
    }
    evaluate_expression(expression, string);

    bool isAllTrue = true, isAllFalse = true;
    for (int i = 0; i < fAnswer.size(); i++){
        bool x = fAnswer[i][fAnswer[i].size()-1];
        if(!x)
            isAllTrue = false;
        else
            isAllFalse = false;
        if (!(isAllTrue || isAllFalse))
            break;
    }
    if (isAllTrue){
        ui->answer_label->setText("This expression is absolute true");
    } else if(isAllFalse){
        ui->answer_label->setText("This expression is absolute false");
    } else{
        QString answer("This expression is false at (");
        for (int i = 0; i < variables.size(); i++){
            if (i != 0)
                answer.append(", ");
            answer.append(QString(variables[i].getSymbol()));
        }
        answer.append("):");
        for(int i = 0; i < fAnswer.size(); i++){
            if (!fAnswer[i][fAnswer[i].size()-1]){
                if(i != 0)
                    answer.append(',');
                answer.append(" (");
                for(int j = 0; j < variables.size(); j++){
                    if(j != 0)
                        answer.append(", ");
                    answer.append(QString::number(fAnswer[i][j]));
                }
                if(fAnswer[i].size() == variables.size())
                    answer.append("0");
                answer.append(")");
            }
        }
        ui->answer_label->setText(answer);
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
                operCount++;
                int order = Operation::order(sym);
                while (operationStack.size() != 0) {
                    if (operationStack.top().getOrder() > order) {
                        answer.push_back(new Operation(operationStack.top()));
                        operationStack.pop();
                    } else
                        break;
                }
                operationStack.push(Operation{sym, i});
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
                answer[answer.size()-1]->value = (sym == '1');
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
    bool isOperator = false, isMinus = false;
    for (int i = 0; i < string.size(); i++){
        char sym = string[i].toLatin1();
        if (sym == '&' || sym == '|' || sym == '/' || sym == '+' || sym == '<' || sym == '~'){
            if (isOperator)
                return false;
            isOperator = true;
        } else if(sym != '-') {
            isOperator = false;
            isMinus = false;
        } else{
            if (isMinus)
                return false;
            isMinus = true;
        }
    }
    return !(isMinus || isOperator);
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

void MainWindow::evaluate_expression(std::vector<ExpressionSymbol*> expression, QString string) {
    fAnswer.clear();
    ui->table->clear();
    unsigned long long vars_2 = power_of_2(variables.size());
    ui->table->setRowCount(vars_2);
    ui->table->setColumnCount(variables.size() + operCount);
    QStringList labels;
    bool isFirst = true;
    for(unsigned long long i = vars_2; i > 0; i--){
        fAnswer.push_back(std::vector<bool>());
        int opers = 0;
        for(int j = 0; j < variables.size(); j++){
            variables[j].value = (1ULL << variables.size() - (j + 1)) & (i - 1);
            if (isFirst)
                labels << QString(variables[j].getSymbol());
            ui->table->setItem(vars_2 - i, j, new QTableWidgetItem(QString::number(variables[j].value)));
            fAnswer[vars_2 - i].push_back(variables[j].value);
        }
        auto var_expression = change_var_to_value(expression);
        std::stack<ExpressionSymbol*> exprStack;
        for (int j = 0; j < var_expression.size(); j++){
            if(var_expression[j]->getType() == ExpressionSymbol::Type::Var){
                exprStack.push(var_expression[j]);
            } else{
                Variable result('A');
                if(var_expression[j]->getSymbol() == '-'){
                    result = *dynamic_cast<Variable*>(exprStack.top());
                    result.positionOfStart--;
                    exprStack.pop();
                    result.value = !result.value;
                } else {
                    auto second_val = exprStack.top();
                    exprStack.pop();
                    auto first_val = exprStack.top();
                    exprStack.pop();
                    result = calc_value(*dynamic_cast<Variable *>(first_val), *dynamic_cast<Variable *>(second_val),
                                        expression[j]->getSymbol());
                }
                    ui->table->setItem(vars_2 - i, variables.size() + opers,
                                   new QTableWidgetItem(QString::number(result.value)));
                    if (isFirst){
                        while (result.positionOfStart > 0 && result.positionOfEnd < string.length() - 1){
                            if (string[result.positionOfStart - 1].toLatin1() == '(' && string[result.positionOfEnd + 1].toLatin1() == ')'){
                                result.positionOfStart--;
                                result.positionOfEnd++;
                            } else
                                break;
                        }
                        labels << string.mid(result.positionOfStart, result.positionOfEnd - result.positionOfStart + 1);
                    }
                    exprStack.push(new Variable(result));
                fAnswer[vars_2 - i].push_back(result.value);
                opers++;
            }
        }
//        ui->table->setItem(vars_2 - i, ui->table->columnCount() - 1, new QTableWidgetItem(QString::number(exprStack.top()->value)));
        if (isFirst)
            ui->table->setHorizontalHeaderLabels(labels);
        isFirst = false;
    }
    ui->table->resizeColumnsToContents();
}

unsigned long long MainWindow::power_of_2(unsigned int pow) {
    unsigned long long result = 1;
    for (int i = 0; i < pow; i++){
        result *= 2u;
    }
    return result;
}

std::vector<ExpressionSymbol*> MainWindow::change_var_to_value(std::vector<ExpressionSymbol*> &expression) {
    for(int i = 0; i < expression.size(); i++){
        if(symType(expression[i]->getSymbol()) == MainWindow::SymType::Var){
            expression[i]->value = value_of_var(expression[i]->getSymbol());
        }
    }
    return expression;
}

bool MainWindow::value_of_var(char var) {
    for (int i = 0; i < variables.size(); i++){
        if (var == variables[i].getSymbol())
            return variables[i].value;
    }
    return false;
}

Variable MainWindow::calc_value(Variable first_var, Variable second_var, char oper) {
    Variable result{'1', first_var.positionOfStart};
    result.positionOfEnd = second_var.positionOfEnd;
    bool f = first_var.value, s = second_var.value;
    switch (oper){
        case '&':{
            f = f && s;
            break;
        }
        case '|':{
            f = f || s;
            break;
        }
        case '<':{
            f = f <= s;
            break;
        }
        case '~':{
            f = f == s;
            break;
        }
        case '/':{
            f = f && !s;
            break;
        }
        case '+':{
            f = f != s;
            break;
        }
    }
    result.value = f;
    return result;
}

void MainWindow::action_clear_expression() {
    ui->expr_edit->setText("");
}

void MainWindow::action_insert(char sym) {
    int pos = ui->expr_edit->cursorPosition();
    ui->expr_edit->setText(ui->expr_edit->text().insert(pos, sym));
    ui->expr_edit->setCursorPosition(pos + 1);
}

void MainWindow::action_backspace() {
    int pos = ui->expr_edit->cursorPosition();
    if (pos == 0)
        return;
    QString result, string = ui->expr_edit->text();
    result.append(string.left(pos-1));
    result.append(string.right(string.length() - pos));
    ui->expr_edit->setText(result);
    ui->expr_edit->setCursorPosition(pos - 1);
}

void MainWindow::action_developer() {
    dev.show();
}
