#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>
#include <windows.h>
#include <unistd.h>
#include <QThread>
#include <QMessageBox>
#include <QKeyEvent>
#include <QApplication>


bool autoClickerRunning = false;
bool autoClickerAntiSpamStart = false;
bool autoClickerAntiSpamStop = false;
QMessageBox *messageBox = nullptr;  // Global message box

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QKeySequence defaultKeySequence("F9");
    ui->keySequenceEdit->setKeySequence(defaultKeySequence);

    qApp->installEventFilter(this);
}


MainWindow::~MainWindow()
{
    delete ui;
}



/*
AUTOCLICKER SECION START ===============================================================
*/

/*
KEY SEQUENCE EDITOR FOR AUTOCLICKER HOTKEY
*/
void MainWindow::on_keySequenceEdit_keySequenceChanged(const QKeySequence &keySequence)
{
    ui->keySequenceEdit->setKeySequence(keySequence);
    qDebug() << "Hotkey changed to: " << keySequence;
}


void MainWindow::on_keySequenceEdit_editingFinished()
{
    qDebug() << "Hotkey change successful!";

}



/*
AUTOCLICKER RUN
*/
void autoClickerRun() {
    int interval = 100;
    autoClickerRunning = true;

    Sleep(3000);  // Wait before starting for 3 seconds
    while (autoClickerRunning) {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        Sleep(interval);
    }

}


/*
AUTOCLICKER START AND STOP BUTTONS
*/
void MainWindow::on_pushButton_clicked()
{
    qDebug() << "Start autoclicker button pressed";
    ui->pushButton->setChecked(true);
    ui->pushButton_2->setChecked(false);
    autoClickerAntiSpamStop = false;

    if (!autoClickerRunning) {
        qDebug() << "Autoclicker running";

        if (!autoClickerAntiSpamStart) {
            autoClickerAntiSpamStart = true;

            if (messageBox) {
                messageBox->close();
                delete messageBox;
            }

            messageBox = new QMessageBox(this);
            messageBox->setWindowTitle(tr("Auto Clicker Running"));
            messageBox->setText(tr("After clicking OK, the autoclicker will begin running in 3 seconds!"));
            messageBox->exec();
            QThread *thread = QThread::create(autoClickerRun);
            thread->start();
        }
    } else {
        qDebug() << "Autoclicker already running";
    }

}


void MainWindow::on_pushButton_2_clicked()
{
    qDebug() << "Stop autoclicker button pressed";
    ui->pushButton->setChecked(false);
    ui->pushButton_2->setChecked(true);
    autoClickerRunning = false;
    autoClickerAntiSpamStart = false;

    if (!autoClickerAntiSpamStop) {
        autoClickerAntiSpamStop = true;

        if (messageBox) {
            messageBox->close();
            delete messageBox;
        }

        messageBox = new QMessageBox(this);
        messageBox->setWindowTitle(tr("Auto Clicker Stopped"));
        messageBox->setText(tr("The autoclicker process has been stopped!"));
        messageBox->exec();
    }
}

/*
AUTOCLICKER HOTKEY START AND STOP
*/

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        QKeySequence userInput = ui->keySequenceEdit->keySequence();

        if (userInput == QKeySequence(keyEvent->key())) {

            if (!autoClickerRunning) {
                on_pushButton_clicked();

            } else {
                on_pushButton_2_clicked();
            }

            return true; // Use the event, do not give to another
        }
    }


    return QMainWindow::eventFilter(obj, event);  // Return the event, other event handler maybe can take it
}


/*
AUTOCLICKER SECION END ===============================================================
*/



