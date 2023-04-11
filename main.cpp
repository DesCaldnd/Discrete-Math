#include <QApplication>
#include "Headers/mainwindow.h"
#include <QFile>

#define ORGANIZATION_NAME "DesCaldnd Studios, Inc."
#define ORGANIZATION_DOMAIN "https://www.example.com/"
#define APPLICATION_NAME "Logic Expression Evaluator"

int main(int argc, char *argv[]) {
	QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
	QCoreApplication::setApplicationName(APPLICATION_NAME);

    QApplication a(argc, argv);

    QFile styleFile("./style/Diffnes.qss");
    styleFile.open(QFile::ReadOnly);
    QString styleString = QLatin1String(styleFile.readAll());
    a.setStyleSheet(styleString);

    MainWindow window{nullptr};
    window.show();
    return QApplication::exec();
}
