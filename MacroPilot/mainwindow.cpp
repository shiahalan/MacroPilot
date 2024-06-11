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
bool autoClickerAntiSpamStop = true;
QMessageBox *messageBox = nullptr;  // Global message box

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QKeySequence defaultKeySequence("F9");
    ui->keySequenceEdit->setKeySequence(defaultKeySequence);

    // RegisterHotKey((HWND)MainWindow::winId(), 99, 0, VK_F9);
    // RegisterHotKey((HWND)MainWindow::winId(), 100, 0, VK_F8);
    // RegisterHotKey((HWND)MainWindow::winId(), 101, 0, VK_F7);

    registerHotKey(100, 0, VK_F8);
    registerHotKey(101, 0, VK_F7);

    // qApp->installEventFilter(this);

}


MainWindow::~MainWindow()
{
    delete ui;
}


Ui::MainWindow *MainWindow::getUi() const {
    return ui;
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


bool MainWindow::registerHotKey(int id, int modifiers, int key) {
    if (!RegisterHotKey((HWND)MainWindow::winId(), id, modifiers, key)) {
        qDebug() << "Failed to register hotkey with ID:" << id;
        return false;
    }
    qDebug() << "Registered hotkey with ID:" << id;
    return true;
}


bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result){
    Q_UNUSED(eventType);
    Q_UNUSED(result);

    // MSG *msg = reinterpret_cast<MSG*>(message);
    qDebug() << "Received message:"; // << msg->message << "wParam:" << msg->wParam;

    // Q_UNUSED(eventType)
    // Q_UNUSED(result)

    // MSG *msg = reinterpret_cast<MSG*>(message);
    // if (msg->message == WM_HOTKEY){
    //     qDebug() << "Something was pressed";
    //     if (msg->wParam == 99) {
    //         qDebug() << " I WAS PRESSED";
    //         return true;
    //     }
    //     // if (msg->wParam == 100){ // Hotkey ID for F8
    //     //     if (!autoClickerRunning) {
    //     //         on_pushButton_clicked();
    //     //     } else {
    //     //         on_pushButton_2_clicked();
    //     //     }
    //     //     return true; // Indicate that the message has been handled
    //     // }
    //     // if (msg->wParam == 101){ // Hotkey ID for F7
    //     //     if (!autoClickerRunning) {
    //     //         on_pushButton_clicked();
    //     //     } else {
    //     //         on_pushButton_2_clicked();
    //     //     }
    //     //     return true; // Indicate that the message has been handled
    //     // }
    // }

    return false;
}


int mapQtKeyToVirtualKey(Qt::Key key) {
    switch (key) {
    case Qt::Key_F9: return VK_F9;
    default: return -1; // Invalid key
    }
}

bool tester = false;
void MainWindow::testHotkey(const QKeySequence &sequence, MainWindow *mainWindow) {
    QList<int> virtualKeyCodes;
    for (int i = 0; i < sequence.count(); ++i) {
        int qtKeyCode = sequence[i];
        int virtualKeyCode = mapQtKeyToVirtualKey(static_cast<Qt::Key>(qtKeyCode));
        if (virtualKeyCode != -1) {
            virtualKeyCodes.append(virtualKeyCode);
        }
    }

    while (true) {
        bool allPressed = true;
        foreach (int virtualKeyCode, virtualKeyCodes) {
            if (!(GetAsyncKeyState(virtualKeyCode) & 0x8000)) {
                allPressed = false;
                break;
            }
        }
        if (allPressed) {
            if (!autoClickerRunning) {
                mainWindow->on_pushButton_clicked();

            } else {
                mainWindow->on_pushButton_2_clicked();
            }
        }

        // if (GetAsyncKeyState('X') {
        //     qDebug() << "Pressed";
        //     tester = true;
        // }
        // else if (GetAsyncKeyState('Z')) {
        //     qDebug() << "Stopped";
        //     tester = false;
        // }
        // if (tester) {
        //     qDebug() << "Clicking";
        //     Sleep(500);
        // }
    }
}


/*
AUTOCLICKER SECION END ===============================================================
*/



