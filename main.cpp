#include <QApplication>
#include "Headers/mainwindow.h"
#include <QFile>
#include <QTranslator>
#include <QDebug>

#define ORGANIZATION_NAME "DesCaldnd Studios, Inc."
#define ORGANIZATION_DOMAIN "https://www.example.com/"
#define APPLICATION_NAME "Logic Expression Evaluator"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    a.setOrganizationName(ORGANIZATION_NAME);
    a.setOrganizationDomain(ORGANIZATION_DOMAIN);
    a.setApplicationName(APPLICATION_NAME);

    QTranslator translator;
    translator.load(QString("lev-") + QLocale::system().uiLanguages()[2]);
    a.installTranslator(&translator);

    QFile styleFile("./style/Diffnes.qss");
    styleFile.open(QFile::ReadOnly);
    QString styleString = QLatin1String(styleFile.readAll());
    a.setStyleSheet(styleString);

    MainWindow window{nullptr};
    window.show();
    return QApplication::exec();
}
