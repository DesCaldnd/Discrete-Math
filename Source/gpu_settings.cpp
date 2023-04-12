//
// Created by ivanl on 03.04.2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_GPU_Settings.h" resolved

#include "../Headers/gpu_settings.h"
#include "ui_GPU_Settings.h"
#include <QDebug>
#include <QTranslator>

GPU_Settings::GPU_Settings(QWidget *parent) :
	QWidget(parent), ui(new Ui::GPU_Settings), settings_("settings.ini", QSettings::IniFormat)
{
	ui->setupUi(this);

	connect(ui->checkBox, &QCheckBox::stateChanged, this, &GPU_Settings::checkbox_changed);
	connect(ui->spinBox, &QSpinBox::valueChanged, this, &GPU_Settings::vars_changed);
	connect(ui->pushButton, &QPushButton::clicked, this, &GPU_Settings::button_pushed);
    connect(ui->language_box, &QComboBox::currentIndexChanged, this, &GPU_Settings::language_changed);

	settings_.beginGroup("gpu");
	ui->checkBox->setCheckState(settings_.value("use", true).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
	ui->spinBox->setValue(settings_.value("vars", 10).toInt());
    language_settings = settings_.value("lang", "en").toString();
    language = language_settings;
	settings_.endGroup();
}

GPU_Settings::~GPU_Settings()
{
	delete ui;
}

void GPU_Settings::checkbox_changed(int v)
{
	settings_.beginGroup("gpu");
	settings_.setValue("use", v);
	settings_.endGroup();
	qDebug() << "Use GPU: " << v;
}

void GPU_Settings::vars_changed(int v)
{
	settings_.beginGroup("gpu");
	settings_.setValue("vars", v);
	settings_.endGroup();
	qDebug() << "Min vars for GPU: " << v;
}

void GPU_Settings::button_pushed()
{
    if (language_settings != language)
    {
        QTranslator translator;
        translator.load(QString("lev-") + language);
        QApplication::instance()->installTranslator(&translator);
    }
	hide();
}

void GPU_Settings::language_changed(int index) {
    QString lang;
    switch (index)
    {
        case 0:
        {
            lang = "en";
            break;
        }
        case 1:
        {
            lang = "ru";
            break;
        }
    }
    language = lang;
}
