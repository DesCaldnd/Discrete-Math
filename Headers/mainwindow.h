//
// Created by Des Caldnd on 2/19/2023.
//

#ifndef DISCRETEMATH_MAINWINDOW_H
#define DISCRETEMATH_MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <vector>
#include "Variable.h"
#include "Operation.h"

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

    bool check_string_for_operators(const QString&);

    bool check_string_for_end(const QString&);

    std::vector<ExpressionSymbol*> expr_to_postfix(const QString&);

    void evaluate_expression(std::vector<ExpressionSymbol*>, QString);

    bool hasVar(char);

    unsigned long long power_of_2(unsigned int);

    std::vector<ExpressionSymbol*> change_var_to_value(std::vector<ExpressionSymbol*>&);

    bool value_of_var(char);

    Variable calc_value(Variable, Variable, char);

    QMessageBox errorMessageBox{this};

    std::vector<Variable> variables{};

    std::vector<std::vector<bool>> fAnswer;

    enum SymType{
        Var, Oper, OpenBracket, CloseBracket, Constant
    };

    SymType symType(char);

    int operCount = 0;

private slots:

    void eval_button_clicked();


};


#endif //DISCRETEMATH_MAINWINDOW_H
