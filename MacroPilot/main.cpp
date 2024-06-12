#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QtConcurrent/QtConcurrent>


int main(int argc, char *argv[])
{
    qDebug() << "STARTING APPLICATION!";
    /*
    APPLICATION OBJECT + APPLICATION SPECS
    */
    QApplication a(argc, argv);
    QIcon icon(":/img/img/favicon.ico");
    a.setWindowIcon(icon);

    /*
    TRANSLATION
    */
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "MacroPilot_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    /*
    WINDOW OBJECT + WINDOW SPECS
    */
    MainWindow w;
    w.setWindowTitle("MacroPilot");
    w.setFixedSize(485, 330);  // Width height, pixels
    w.setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    w.show();  // Show window

    return a.exec();  // Execute application event loop
}

