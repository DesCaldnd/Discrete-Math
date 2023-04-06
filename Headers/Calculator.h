//
// Created by ivanl on 14.03.2023.
//

#ifndef CALCULATOR_H_
#define CALCULATOR_H_

#pragma once

#include <QString>
#include <vector>
#include "Variable.h"
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include "GpuComputeTable.h"
#include <memory>
#include <QSettings>

class Calculator final : public QObject
{
 Q_OBJECT

 public:

	Calculator(QLineEdit* expr_ed, QTableWidget* tab, QLabel* answer_lab);

	void action_file(QString);

	void eval_button_clicked();

 private:

	static bool check_string_for_brackets(const QString&);

	static bool check_string_for_operators(const QString&);

	static bool check_string_for_end(const QString&);

	std::vector<std::shared_ptr<ExpressionSymbol>> expr_to_postfix(const QString&);

	void evaluate_expression(std::vector<std::shared_ptr<ExpressionSymbol>>, const QString&);

	bool hasVar(char);

	static unsigned int power_of_2(unsigned int);

	std::vector<std::shared_ptr<ExpressionSymbol>> change_var_to_value(std::vector<std::shared_ptr<ExpressionSymbol>>&);

	bool value_of_var(char);

	Variable calc_value(Variable, Variable, char);

	std::vector<Variable> variables{};

	std::vector<bool> fAnswer;

	QStringList labels;

	enum SymType
	{
		Var, Oper, OpenBracket, CloseBracket, Constant
	};

	static SymType symType(char);

	int operCount = 0;

	bool isCalculating = false;

	QLineEdit* expr_edit = nullptr;

	QTableWidget* table = nullptr;

	QLabel* answer_label = nullptr;

	unsigned int trues;

	std::shared_ptr<GPUComputeTable> compute_module;

	bool can_compute_gpu = true;

	bool use_gpu = true;

	int min_variables_for_gpu = 10;

	void calculate_and_set_labels(std::vector<std::shared_ptr<ExpressionSymbol>>, const QString&);

	QSettings settings_;

 signals:

	void expr_error(QString error);
};

#endif //CALCULATOR_H_
