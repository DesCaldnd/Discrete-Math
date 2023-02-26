//
// Created by ivanl on 25.02.2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Developer.h" resolved

#include "../Headers/developer.h"
#include "ui_Developer.h"
#include <QPalette>


Developer::Developer(QWidget *parent) :
        QWidget(parent), ui(new Ui::Developer) {
    ui->setupUi(this);
}

Developer::~Developer() {
    delete ui;
}
