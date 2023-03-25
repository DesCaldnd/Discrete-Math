//
// Created by ivanl on 14.03.2023.
//

#include "../Headers/Calculator.h"
#include <QRegularExpressionValidator>
#include <QDebug>
#include <stack>
#include <iostream>
#include <fstream>
#include <QFileInfo>
#include "../Headers/Operation.h"
#include "xlsxdocument.h"
#include <QScrollBar>

void Calculator::eval_button_clicked()
{
	QString string = expr_edit->text();
	std::ostringstream answer;
	bool isAllTrue = true, isAllFalse = true;
	std::vector<ExpressionSymbol *> expression;

	table->verticalScrollBar()->setValue(0);
	table->horizontalScrollBar()->setValue(0);

	if (string.isEmpty())
	{
		emit expr_error("Expression is empty");
		return;
	}
	if (!check_string_for_brackets(string))
	{
		emit expr_error("Brackets error");
		return;
	}
	if (!check_string_for_operators(string))
	{
		emit expr_error("More than one operator in a row");
		return;
	}
	if (!check_string_for_end(string))
	{
		emit expr_error("This expression is not completed");
		return;
	}
	expression = expr_to_postfix(string);
	if (variables.size() > 19)
	{
		emit expr_error("Too many variables (max=19)");
		return;
	}

	std::sort(variables.begin(), variables.end());
	table->clear();

	bool* result_table = compute.run(variables, expression, operCount, trues);
	unsigned int vars_2 = power_of_2(variables.size());
	table->setRowCount(vars_2);
	table->setColumnCount(variables.size() + operCount);
	for (int i = 0; i < table->rowCount(); ++i)
		for (int j = 0; j < table->columnCount(); ++j)
		{
			table->setItem(i, j, new QTableWidgetItem(QString::number(result_table[i * (variables.size() + operCount) + j])));
		}

//	evaluate_expression(expression, string);
	table->resizeColumnsToContents();

	if (trues == vars_2)
	{
		answer << "This expression is absolute true";
	} else if (trues == 0)
	{
		answer << "This expression is absolute false";
	} else
	{
		answer << "This expression is ";
		bool variant = ((double)trues / (double)(fAnswer.size() - 1)) >= 0.5;
		if (!variant)
		{
			answer << "true";
		} else
		{
			answer << "false";
		}
		answer << " at (";
		bool isFirst = true;
		for (Variable data : variables)
		{
			if (!isFirst)
			{
				answer << ", ";
			}
			answer << data.getSymbol();
			isFirst = false;
		}
		answer << "):";
		isFirst = true;
		for (std::vector<bool> vector_data : fAnswer)
		{
			if (vector_data[vector_data.size() - 1] != variant)
			{
				if (!isFirst)
				{
					answer << ',';
				}
				answer << " (";
				for (int j = 0; j < variables.size(); j++)
				{
					if (j != 0)
						answer << ", ";
					answer << vector_data[j];
				}
				if (vector_data.size() == variables.size())
					answer << "0";
				answer << ")";
				isFirst = false;
			}
		}
	}
	answer_label->setText(QString::fromStdString(answer.str()));
	delete[] result_table;
}

bool Calculator::check_string_for_brackets(const QString &string)
{
	int brackets = 0;
	for (QChar qSym : string)
	{
		if (qSym == '(')
			brackets++;
		else if (qSym == ')')
			brackets--;
		if (brackets < 0)
			return false;
	}
	return brackets == 0;
}

std::vector<ExpressionSymbol *> Calculator::expr_to_postfix(const QString &string)
{
	variables.clear();
	operCount = 0;

	std::vector<ExpressionSymbol *> answer;
	std::stack<Operation> operationStack;

	int i = 0;
	for (QChar qSym : string)
	{
		char sym = qSym.toLatin1();
		switch (symType(sym))
		{
			case Var:
			{
				answer.push_back(new Variable(sym, i));
				if (!hasVar(sym))
					variables.push_back(Variable{sym});
				break;
			}
			case Oper:
			{
				operCount++;
				int order = Operation::order(sym);
				while (!operationStack.empty())
				{
					if (operationStack.top().getOrder() > order)
					{
						answer.push_back(new Operation(operationStack.top()));
						operationStack.pop();
					} else
						break;
				}
				operationStack.push(Operation{sym, i});
				break;
			}
			case OpenBracket:
			{
				operationStack.push(Operation{sym});
				break;
			}
			case CloseBracket:
			{
				while (operationStack.top().getSymbol() != '(')
				{
					answer.push_back(new Operation(operationStack.top()));
					operationStack.pop();
				}
				operationStack.pop();
				break;
			}
			case Constant:
			{
				answer.push_back(new Variable(sym, i));
				answer[answer.size() - 1]->value = (sym == '1');
				break;
			}
		}
		i++;
	}

	while (!operationStack.empty())
	{
		answer.push_back(new Operation(operationStack.top()));
		operationStack.pop();
	}

	return answer;
}

Calculator::SymType Calculator::symType(char symbol)
{
	if ((symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z'))
		return Calculator::SymType::Var;
	else if (symbol == '(')
		return Calculator::SymType::OpenBracket;
	else if (symbol == ')')
		return Calculator::SymType::CloseBracket;
	else if (symbol == '1' || symbol == '0')
		return Calculator::SymType::Constant;
	return Calculator::SymType::Oper;
}

bool Calculator::hasVar(char var)
{
	for (Variable data : variables)
	{
		if (data.getSymbol() == var)
			return true;
	}
	return false;
}

bool Calculator::check_string_for_operators(const QString &string)
{
	bool isOperator = false, isMinus = false;
	for (QChar qSym : string)
	{
		char sym = qSym.toLatin1();
		if (sym == '&' || sym == '|' || sym == '/' || sym == '+' || sym == '<' || sym == '~')
		{
			if (isOperator)
				return false;
			isOperator = true;
		} else if (sym != '-')
		{
			isOperator = false;
			isMinus = false;
		} else
		{
			if (isMinus)
				return false;
			isMinus = true;
		}
	}
	return !(isMinus || isOperator);
}

bool Calculator::check_string_for_end(const QString &string)
{
	if (string.size() == 1 && (string[0] == '1' || string[0] == '0'))
		return false;
	bool hasFirstOperand = false, isOper = false;
	for (QChar qSym : string)
	{
		char sym = qSym.toLatin1();
		switch (symType(sym))
		{
			case Oper:
			{
				if (sym != '-')
				{
					if (!hasFirstOperand)
					{
						return false;
					} else
					{
						hasFirstOperand = false;
					}
				}
				isOper = true;
				break;
			}
			case OpenBracket:
			{
				isOper = false;
				hasFirstOperand = false;
				break;
			}
			case CloseBracket:
			{
				if (isOper)
				{
					return false;
				}
				hasFirstOperand = true;
				break;
			}
			default:
			{
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

void Calculator::evaluate_expression(std::vector<ExpressionSymbol *> expression, const QString &string)
{
	trues = 0;
	fAnswer.clear();
	unsigned int vars_2 = power_of_2(variables.size());
	fAnswer.reserve(vars_2);
	labels.clear();
	bool isFirst = true;
	for (unsigned int i = vars_2; i > 0; i--)
	{
		fAnswer.push_back(std::vector<bool>());
		fAnswer[vars_2 - i].reserve(variables.size() + operCount);
		int opers = 0;
		for (int j = 0; j < variables.size(); j++)
		{
			variables[j].value = (1u << variables.size() - (j + 1)) & (i - 1);
			if (isFirst)
				labels << QString(variables[j].getSymbol());
			table->setItem(vars_2 - i, j, new QTableWidgetItem(QString::number(variables[j].value)));
			fAnswer[vars_2 - i].push_back(variables[j].value);
		}
		auto var_expression = change_var_to_value(expression);
		std::stack<ExpressionSymbol *> exprStack;
		for (ExpressionSymbol *var_expr : var_expression)
		{
			if (var_expr->getType() == ExpressionSymbol::Type::Var)
			{
				exprStack.push(var_expr);
			} else
			{
				Variable result('A');
				if (var_expr->getSymbol() == '-')
				{
					result = *static_cast<Variable *>(exprStack.top());
					result.positionOfStart--;
					exprStack.pop();
					result.value = !result.value;
				} else
				{
					auto second_val = exprStack.top();
					exprStack.pop();
					auto first_val = exprStack.top();
					exprStack.pop();
					result = calc_value(*static_cast<Variable *>(first_val), *static_cast<Variable *>(second_val),
										var_expr->getSymbol());
				}
				table->setItem(vars_2 - i, variables.size() + opers,
							   new QTableWidgetItem(QString::number(result.value)));
				if (isFirst)
				{
					while (result.positionOfStart > 0 && result.positionOfEnd < string.length() - 1)
					{
						if (string[result.positionOfStart - 1].toLatin1() == '('
							&& string[result.positionOfEnd + 1].toLatin1() == ')')
						{
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
		if (isFirst)
		{
			table->setHorizontalHeaderLabels(labels);
			isFirst = false;
		} else if (fAnswer[vars_2 - i][fAnswer[vars_2 - i].size() - 1] == true)
		{
			trues++;
		}
	}
}

unsigned int Calculator::power_of_2(unsigned int pow)
{
	unsigned long long result = 1;
	for (int i = 0; i < pow; i++)
	{
		result *= 2u;
	}
	return result;
}

std::vector<ExpressionSymbol *> Calculator::change_var_to_value(std::vector<ExpressionSymbol *> &expression)
{
	for (ExpressionSymbol *expr : expression)
	{
		if (symType(expr->getSymbol()) == Calculator::SymType::Var)
		{
			expr->value = value_of_var(expr->getSymbol());
		}
	}
	return expression;
}

bool Calculator::value_of_var(char var)
{
	for (Variable data : variables)
	{
		if (var == data.getSymbol())
			return data.value;
	}
	return false;
}

Variable Calculator::calc_value(Variable first_var, Variable second_var, char oper)
{
	Variable result{'1', first_var.positionOfStart};
	result.positionOfEnd = second_var.positionOfEnd;
	bool f = first_var.value, s = second_var.value;
	switch (oper)
	{
		case '&':
		{
			f = f && s;
			break;
		}
		case '|':
		{
			f = f || s;
			break;
		}
		case '<':
		{
			f = f <= s;
			break;
		}
		case '~':
		{
			f = f == s;
			break;
		}
		case '/':
		{
			f = f && !s;
			break;
		}
		case '+':
		{
			f = f != s;
			break;
		}
	}
	result.value = f;
	return result;
}

void Calculator::action_file(QString path)
{
	if (fAnswer.empty())
		emit expr_error("Table is empty!");
	QFileInfo info(path);

	if (info.suffix() == "csv")
	{
		std::ofstream outData(path.toStdString(), std::ios::out | std::ios::trunc);
		if (!outData.is_open())
		{
			emit expr_error("Error during file open");

			return;
		}
		outData << "Row index;";
		for (QString data : labels)
		{
			outData << data.toStdString() << ';';
		}
		outData << std::endl;
		int i = 1;
		for (std::vector<bool> iter : fAnswer)
		{
			outData << i << ';';
			for (bool data : iter)
			{
				outData << data << ';';
			}
			outData << std::endl;
			i++;
		}
		outData.close();
	} else
	{
		QXlsx::Document outData;
		QVariant writeValue = QString("Row index");
		outData.write(1, 1, writeValue);

		int i = 2;
		for (QString data : labels)
		{
			outData.write(1, i, data);
			i++;
		}
		i = 2;
		for (std::vector<bool> iter : fAnswer)
		{
			int j = 2;
			outData.write(i, 1, i - 1);
			for (bool data : iter)
			{
				outData.write(i, j, static_cast<int>(data));
				j++;
			}
			i++;
		}
		outData.saveAs(path);
	}
}

Calculator::Calculator(QLineEdit *expr_ed, QTableWidget *tab, QLabel *answer_lab)
	: expr_edit(expr_ed), table(tab), answer_label(answer_lab) {}
