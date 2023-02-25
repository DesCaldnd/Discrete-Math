//
// Created by ivanl on 25.02.2023.
//

#ifndef DISCRETEMATH_DEVELOPER_H
#define DISCRETEMATH_DEVELOPER_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class Developer; }
QT_END_NAMESPACE

class Developer : public QWidget {
Q_OBJECT

public:
    explicit Developer(QWidget *parent = nullptr);

    ~Developer() override;

private:
    Ui::Developer *ui;
};


#endif //DISCRETEMATH_DEVELOPER_H
