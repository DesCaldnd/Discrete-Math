//
// Created by Des Caldnd on 2/19/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "../Headers/mainwindow.h"
#include "ui_MainWindow.h"
#include <QRegularExpressionValidator>
#include <QPushButton>
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setMinimumSize(500, 250);

//	thread = new QThread();

	calculator = std::make_unique<Calculator>(ui->expr_edit, ui->table, ui->answer_label);

	calculator->moveToThread(&thread);

	thread.start();

	errorMessageBox.setWindowTitle("Error");

	auto *validator = new QRegularExpressionValidator{QRegularExpression{"[A-Za-z10()\\-&|<~+\\/ ]*"}, this};
	ui->expr_edit->setValidator(validator);

	connect(ui->eval_button, &QPushButton::clicked, &*calculator, &Calculator::eval_button_clicked);
	connect(ui->expr_edit, &QLineEdit::returnPressed, &*calculator, &Calculator::eval_button_clicked);

	connect(&*calculator, &Calculator::expr_error, this, &MainWindow::show_error);

	connect(ui->actionClear_expression, &QAction::triggered, this, &MainWindow::action_clear_expression);
	connect(ui->actionNegative, &QAction::triggered, [&]() { action_insert('-'); });
	connect(ui->actionConjunction, &QAction::triggered, [&]() { action_insert('&'); });
	connect(ui->actionDisjunction, &QAction::triggered, [&]() { action_insert('|'); });
	connect(ui->actionImplication, &QAction::triggered, [&]() { action_insert('<'); });
	connect(ui->actionEquivalent, &QAction::triggered, [&]() { action_insert('~'); });
	connect(ui->actionSymmetric_Complement, &QAction::triggered, [&]() { action_insert('+'); });
	connect(ui->actionRealative_Complement, &QAction::triggered, [&]() { action_insert('/'); });
	connect(ui->actionOpen_Bracket, &QAction::triggered, [&]() { action_insert('('); });
	connect(ui->actionClose_Bracket, &QAction::triggered, [&]() { action_insert(')'); });
	connect(ui->actionA, &QAction::triggered, [&]() { action_insert('A'); });
	connect(ui->actionB, &QAction::triggered, [&]() { action_insert('B'); });
	connect(ui->actionC, &QAction::triggered, [&]() { action_insert('C'); });
	connect(ui->actionBackspace, &QAction::triggered, this, &MainWindow::action_backspace);
	connect(ui->actionDeveloper, &QAction::triggered, this, &MainWindow::action_developer);
	connect(ui->actionSave_Table, &QAction::triggered, this, &MainWindow::action_save);
	connect(this, &MainWindow::save_signal, &*calculator, &Calculator::action_file);
	connect(ui->actionGPU_settings, &QAction::triggered, this, &MainWindow::action_settings);

	ui->statusbar->hide();

}

MainWindow::~MainWindow()
{
	delete ui;
	thread.exit();
}

void MainWindow::action_clear_expression()
{
	ui->expr_edit->setText("");
}

void MainWindow::action_insert(char sym)
{
	int pos = ui->expr_edit->cursorPosition();
	ui->expr_edit->setText(ui->expr_edit->text().insert(pos, sym));
	ui->expr_edit->setCursorPosition(pos + 1);
}

void MainWindow::action_backspace()
{
	int pos = ui->expr_edit->cursorPosition();
	if (pos == 0)
		return;
	QString result, string = ui->expr_edit->text();
	result.append(string.left(pos - 1));
	result.append(string.right(string.length() - pos));
	ui->expr_edit->setText(result);
	ui->expr_edit->setCursorPosition(pos - 1);
}

void MainWindow::action_developer()
{
	dev.show();
}

void MainWindow::show_error(QString text)
{
	errorMessageBox.setText(text);
	errorMessageBox.exec();
}

void MainWindow::action_save()
{
	QString path = QFileDialog::getSaveFileName(nullptr, tr("Save Table"), "/downloads/untitled.xlsx",
												tr("Exel new table (*.xlsx);;Exel simple table (*.csv)"));
	emit save_signal(path);
}

void MainWindow::action_settings()
{
	settings_.show();
}
