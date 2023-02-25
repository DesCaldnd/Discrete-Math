#include <QApplication>
#include "Headers/mainwindow.h"
#include <QFile>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QFile styleFile("../Diffnes.qss");
    styleFile.open(QFile::ReadOnly);
    QString styleString = QLatin1String(styleFile.readAll());
    a.setStyleSheet(styleString);

    MainWindow window{nullptr};
    window.show();
    return QApplication::exec();
}
