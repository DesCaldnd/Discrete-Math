//
// Created by ivanl on 03.04.2023.
//

#ifndef DISCRETEMATH_HEADERS_GPU_SETTINGS_H_
#define DISCRETEMATH_HEADERS_GPU_SETTINGS_H_

#include <QWidget>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class GPU_Settings; }
QT_END_NAMESPACE

class GPU_Settings : public QWidget
{
Q_OBJECT

public:
	explicit GPU_Settings(QWidget *parent = nullptr);

	~GPU_Settings() override;

private slots:

	void checkbox_changed(int v);

	void vars_changed(int v);

	void button_pushed();

    void language_changed(int);

private:
	Ui::GPU_Settings *ui;

	QSettings settings_;

    QString language;

    QString language_settings;
};

#endif //DISCRETEMATH_HEADERS_GPU_SETTINGS_H_
