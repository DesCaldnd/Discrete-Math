//
// Created by ivanl on 03.04.2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_GPU_Settings.h" resolved

#include "../Headers/gpu_settings.h"
#include "ui_GPU_Settings.h"

GPU_Settings::GPU_Settings(QWidget *parent) :
	QWidget(parent), ui(new Ui::GPU_Settings)
{
	ui->setupUi(this);


}

GPU_Settings::~GPU_Settings()
{
	delete ui;
}
