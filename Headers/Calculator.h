//
// Created by ivanl on 14.03.2023.
//

#ifndef CALCULATOR_H_
#define CALCULATOR_H_

#pragma once

#include <QString>
#include <vector>
#include "Variable.h"
#include <QThread>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>

class Calculator final : public QObject
{
 Q_OBJECT

 public:

	Calculator(QLineEdit* expr_ed, QTableWidget* tab, QLabel* answer_lab);

	void action_file(QString);

	void evaluate();

	~Calculator();

 private:

	static bool check_string_for_brackets(const QString&);

	static bool check_string_for_operators(const QString&);

	static bool check_string_for_end(const QString&);

	std::vector<ExpressionSymbol*> expr_to_postfix(const QString&);

	void evaluate_expression(std::vector<ExpressionSymbol*>, const QString&);

	bool hasVar(char);

	static unsigned int power_of_2(unsigned int);

	std::vector<ExpressionSymbol*> change_var_to_value(std::vector<ExpressionSymbol*>&);

	bool value_of_var(char);

	Variable calc_value(Variable, Variable, char);

	std::vector<Variable> variables{};

	std::vector<std::vector<bool>> fAnswer;

	QStringList labels;

	enum SymType
	{
		Var, Oper, OpenBracket, CloseBracket, Constant
	};

	static SymType symType(char);

	int operCount = 0;

	bool isCalculating = false;

	QThread thread;

	QLineEdit* expr_edit = nullptr;

	QTableWidget* table = nullptr;

	QLabel* answer_label = nullptr;

	unsigned int trues;

 private slots:

	void eval_button_clicked();

 signals:

	void expr_error(QString error);
};

#endif //CALCULATOR_H_
