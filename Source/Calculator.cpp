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
#include <exception>

void Calculator::eval_button_clicked()
{
	QString string = expr_edit->text();
	std::ostringstream answer;
	std::vector<std::shared_ptr<ExpressionSymbol>> expression;

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
	try
	{
		expression = expr_to_postfix(string);
	} catch (std::runtime_error &e)
	{
		emit expr_error(e.what());
		return;
	}
	if (variables.size() > 19)
	{
		emit expr_error("Too many variables (max=19)");
		return;
	}

	std::sort(variables.begin(), variables.end());

	settings_.beginGroup("gpu");
	use_gpu = settings_.value("use", true).toBool();
	min_variables_for_gpu = settings_.value("vars", 10).toInt();
	settings_.endGroup();

	qDebug() << "Expr size: " << expression.size();

	if (variables.size() >= min_variables_for_gpu && can_compute_gpu && use_gpu)
	{
		compute_module->run(fAnswer, variables, expression, operCount, trues);
		qDebug() << "GPU compute module used";
	}
	else
	{
		evaluate_expression(expression, string);
		qDebug() << "CPU compute module used";
	}
	unsigned int vars_2 = power_of_2(variables.size());

	table->horizontalScrollBar()->setValue(0);
	table->verticalScrollBar()->setValue(0);
	table->setRowCount(vars_2);
	table->setColumnCount(variables.size() + operCount);

	if (table->rowCount() != vars_2 || table->columnCount() != variables.size() + operCount)
	{
		emit expr_error("Not enough memory");
		return;
	}

	calculate_and_set_labels(expression, string);
	for (int i = 0; i < table->rowCount(); ++i)
		for (int j = 0; j < table->columnCount(); ++j)
		{
			table->setItem(i, j, new QTableWidgetItem(QString::number(fAnswer[i * (variables.size() + operCount) + j])));
		}

	qDebug() << "Trues: " << trues;
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
		bool variant = ((double)trues / (double)(vars_2 - 1)) >= 0.5;
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
		for (int i = 0; i < vars_2; ++i)
		{
			if (fAnswer[(i + 1) * (variables.size() + operCount) - 1] != variant)
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
					answer << fAnswer[i * (variables.size() + operCount) + j];
				}
				answer << ")";
				isFirst = false;
			}
		}
	}
	answer_label->setText(QString::fromStdString(answer.str()));
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
		else if (qSym == ',')
			if  (brackets != 0)
				return false;
		if (brackets < 0)
			return false;
	}
	return brackets == 0;
}

std::vector<std::shared_ptr<ExpressionSymbol>> Calculator::expr_to_postfix(const QString &string)
{
	variables.clear();
	operCount = 0;

	std::vector<std::shared_ptr<ExpressionSymbol>> answer;
	std::stack<Operation> operationStack;

	int i = 0;
	for (QChar qSym : string)
	{
		char sym = qSym.toLatin1();
		switch (symType(sym))
		{
			case Var:
			{
				answer.push_back(std::make_shared<Variable>(sym, i));
				if (!hasVar(sym))
					variables.emplace_back(sym);
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
						answer.push_back(std::make_shared<Operation>(operationStack.top()));
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
					answer.push_back(std::make_shared<Operation>(operationStack.top()));
					operationStack.pop();
				}
				operationStack.pop();
				break;
			}
			case Constant:
			{
				answer.push_back(std::make_shared<Variable>(sym, i));
				answer[answer.size() - 1]->value = (sym == '1');
				break;
			}
			case Separator:
			{
				while (!operationStack.empty())
				{
					answer.push_back(std::make_shared<Operation>(operationStack.top()));
					operationStack.pop();
				}
				break;
			}
		}
		i++;
	}

	while (!operationStack.empty())
	{
		answer.push_back(std::make_shared<Operation>(operationStack.top()));
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
	else if (symbol == ' ')
		return Calculator::SymType::Space;
	else if (symbol == ',')
		return Calculator::SymType::Separator;
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
		}else if (sym != '-' && sym != ' ')
		{
			isOperator = false;
			isMinus = false;
		} else if (sym == '-')
		{
			if (isMinus)
				return false;
			isMinus = true;
		} else if (sym == ',')
		{
			if (isMinus || isOperator)
				return false;
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
			case Var:
			{
				if (hasFirstOperand)
					return false;
				hasFirstOperand = true;
				isOper = false;
				break;
			}
			case Constant:
			{
				if (hasFirstOperand)
					return false;
				hasFirstOperand = true;
				isOper = false;
				break;
			}
			case Separator:
			{
				if (!hasFirstOperand || isOper)
					return false;
				hasFirstOperand = false;
				break;
			}
		}
	}
	return true;
}

void Calculator::evaluate_expression(std::vector<std::shared_ptr<ExpressionSymbol>> expression, const QString &string)
{
	trues = 0;
	fAnswer.clear();
	unsigned int vars_2 = power_of_2(variables.size());
	fAnswer.reserve(vars_2 * (variables.size() + operCount));
	for (unsigned int i = vars_2; i > 0; i--)
	{
		int opers = 0;
		for (int j = 0; j < variables.size(); j++)
		{
			variables[j].value = (1u << variables.size() - (j + 1)) & (i - 1);
			fAnswer.push_back(variables[j].value);
//			qDebug() << QString(variables[j].getSymbol()) << QString::number(fAnswer[fAnswer.size() - 1]);
		}
		auto var_expression = change_var_to_value(expression);
		std::stack<std::shared_ptr<ExpressionSymbol>> exprStack;
		for (std::shared_ptr<ExpressionSymbol>& var_expr : var_expression)
		{
			if (var_expr->getType() == ExpressionSymbol::Type::Var)
			{
				exprStack.push(var_expr);
			} else
			{
				Variable result('A');
				if (var_expr->getSymbol() == '-')
				{
					result = *static_cast<Variable *>(&*exprStack.top());
					result.positionOfStart--;
					exprStack.pop();
					result.value = !result.value;
				} else
				{
					auto second_val = exprStack.top();
					exprStack.pop();
					auto first_val = exprStack.top();
					exprStack.pop();
					result = calc_value(*static_cast<Variable *>(&*first_val), *static_cast<Variable *>(&*second_val),
										var_expr->getSymbol());
				}
				exprStack.push(std::make_shared<Variable>(result));
				fAnswer.push_back(result.value);
				opers++;
			}
		}
		if (fAnswer[(vars_2 - i + 1) * (variables.size() + operCount) - 1] == true)
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

std::vector<std::shared_ptr<ExpressionSymbol>> Calculator::change_var_to_value(std::vector<std::shared_ptr<ExpressionSymbol>> &expression)
{
	for (std::shared_ptr<ExpressionSymbol> &expr : expression)
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
		for (int i = 0, vars_2 = power_of_2(variables.size()); i < vars_2; ++i)
		{
			outData << i + 1 << ';';
			for (int j = 0, max_c = variables.size() + operCount; j < max_c; ++j)
			{
				outData << fAnswer[i * max_c + j] << ';';
			}
			outData << std::endl;
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
		for (int i = 0, vars_2 = power_of_2(variables.size()); i < vars_2; ++i)
		{
			int j = 2;
			outData.write(i + 2, 1, i + 1);
			for (int j = 0, max_c = variables.size() + operCount; j < max_c; ++j)
			{
				outData.write(i + 2, j + 2, static_cast<int>(fAnswer[i * max_c + j]));
			}
		}
		outData.saveAs(path);
	}
}

Calculator::Calculator(QLineEdit *expr_ed, QTableWidget *tab, QLabel *answer_lab)
	: expr_edit(expr_ed), table(tab), answer_label(answer_lab), settings_("settings.ini", QSettings::IniFormat)
	{
	try
	{
		compute_module = std::make_shared<GPUComputeTable>();
	}
	catch (...)
	{
		can_compute_gpu = false;
	}
	qDebug() << "Can compute: " << can_compute_gpu;
}

void Calculator::calculate_and_set_labels(std::vector<std::shared_ptr<ExpressionSymbol>> expression, const QString &string)
{
	labels.clear();
	for (int j = 0; j < variables.size(); j++)
	{
		variables[j].value = (1u << variables.size() - (j + 1)) & (1 - 1);
		labels << QString(variables[j].getSymbol());
	}
	auto var_expression = change_var_to_value(expression);
	std::stack<std::shared_ptr<ExpressionSymbol>> exprStack;
	for (std::shared_ptr<ExpressionSymbol> &var_expr : var_expression)
	{
		if (var_expr->getType() == ExpressionSymbol::Type::Var)
		{
			exprStack.push(var_expr);
		} else
		{
			Variable result('A');
			if (var_expr->getSymbol() == '-')
			{
				result = *static_cast<Variable *>(&*exprStack.top());
				result.positionOfStart--;
				exprStack.pop();
				result.value = !result.value;
			} else
			{
				auto second_val = exprStack.top();
				exprStack.pop();
				auto first_val = exprStack.top();
				exprStack.pop();
				result = calc_value(*static_cast<Variable *>(&*first_val), *static_cast<Variable *>(&*second_val),
									var_expr->getSymbol());
			}
			while (result.positionOfStart > 0 && result.positionOfEnd < string.length() - 1)
			{
				if (string[result.positionOfStart - 1].toLatin1() == ' ')
				{
					result.positionOfStart--;
				} else if(string[result.positionOfEnd + 1].toLatin1() == ' ')
				{
					result.positionOfEnd++;
				} else if (string[result.positionOfStart - 1].toLatin1() == '('
					&& string[result.positionOfEnd + 1].toLatin1() == ')')
				{
					result.positionOfStart--;
					result.positionOfEnd++;
				} else
					break;
			}
			labels << string.mid(result.positionOfStart, result.positionOfEnd - result.positionOfStart + 1);
			exprStack.push(std::make_shared<Variable>(result));
		}
	}
	table->setHorizontalHeaderLabels(labels);
}
