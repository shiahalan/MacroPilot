#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>
#include <windows.h>
#include <unistd.h>
#include <QThread>
#include <QMessageBox>
#include <QKeyEvent>
#include <QApplication>


bool autoClickerRunning = false;  // Enable or disable autoclicker while loop
int hotKeyId = 101;  // Arbitrary hotKeyId for registering hotKeys
QMessageBox *messageBox = nullptr;  // Global message box

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QKeySequence defaultKeySequence("F9");  // Setup default hotkey as F9
    ui->keySequenceEdit->setKeySequence(defaultKeySequence);

    // RegisterHotKey((HWND)MainWindow::winId(), 101, 0, VK_F9);
    registerHotKey(101, 0, VK_F9);

    mainTimer = new QTimer;  // Timer for autoclicker intervals
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(autoClickerRun()));  // Each timeout call autoClickerRun() function

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
    qDebug() << "Hotkey being changed to: " << keySequence;
    unregisterHotKey(hotKeyId);

    int keyCode = keySequence[0];  // When single character, first index is character
    int modifiers = 0;  // Modifier for registering hotkeys: ctrl alt etc.

    if (!keySequence.isEmpty()) {

        if (keySequence.toString(QKeySequence::NativeText).contains("Ctrl") ||
            keySequence.toString(QKeySequence::NativeText).contains("Alt") ||
            keySequence.toString(QKeySequence::NativeText).contains("Shift") ||
            keySequence.toString(QKeySequence::NativeText).contains("Meta")) {

            qDebug() << "Has a modifier!";

            if (keySequence.toString(QKeySequence::NativeText).contains("F1") ||
                keySequence.toString(QKeySequence::NativeText).contains("F2") ||
                keySequence.toString(QKeySequence::NativeText).contains("F3") ||
                keySequence.toString(QKeySequence::NativeText).contains("F4") ||
                keySequence.toString(QKeySequence::NativeText).contains("F5") ||
                keySequence.toString(QKeySequence::NativeText).contains("F6") ||
                keySequence.toString(QKeySequence::NativeText).contains("F7") ||
                keySequence.toString(QKeySequence::NativeText).contains("F8") ||
                keySequence.toString(QKeySequence::NativeText).contains("F9") ||
                keySequence.toString(QKeySequence::NativeText).contains("F10") ||
                keySequence.toString(QKeySequence::NativeText).contains("F11") ||
                keySequence.toString(QKeySequence::NativeText).contains("F12")) {

                if (keySequence.toString(QKeySequence::NativeText).contains("F1"))
                    keyCode = Qt::Key_F1;
                if (keySequence.toString(QKeySequence::NativeText).contains("F2"))
                    keyCode = Qt::Key_F2;
                if (keySequence.toString(QKeySequence::NativeText).contains("F3"))
                    keyCode = Qt::Key_F3;
                if (keySequence.toString(QKeySequence::NativeText).contains("F4"))
                    keyCode = Qt::Key_F4;
                if (keySequence.toString(QKeySequence::NativeText).contains("F5"))
                    keyCode = Qt::Key_F5;
                if (keySequence.toString(QKeySequence::NativeText).contains("F6"))
                    keyCode = Qt::Key_F6;
                if (keySequence.toString(QKeySequence::NativeText).contains("F7"))
                    keyCode = Qt::Key_F7;
                if (keySequence.toString(QKeySequence::NativeText).contains("F8"))
                    keyCode = Qt::Key_F8;
                if (keySequence.toString(QKeySequence::NativeText).contains("F9"))
                    keyCode = Qt::Key_F9;
                if (keySequence.toString(QKeySequence::NativeText).contains("F10"))
                    keyCode = Qt::Key_F10;
                if (keySequence.toString(QKeySequence::NativeText).contains("F11"))
                    keyCode = Qt::Key_F11;
                if (keySequence.toString(QKeySequence::NativeText).contains("F12"))
                    keyCode = Qt::Key_F12;
            } else keyCode = keySequence.toString(QKeySequence::NativeText).toStdString().back();

            if (keySequence.toString(QKeySequence::NativeText).contains("Ctrl"))
                modifiers |= MOD_CONTROL;
            if (keySequence.toString(QKeySequence::NativeText).contains("Alt"))
                modifiers |= MOD_ALT;
            if (keySequence.toString(QKeySequence::NativeText).contains("Shift"))
                modifiers |= MOD_SHIFT;
            if (keySequence.toString(QKeySequence::NativeText).contains("Meta"))
                modifiers |= MOD_WIN;
        }

        // Check for special cases like F1-F12
        switch(keyCode) {

        case Qt::Key_F1:
            keyCode = VK_F1;
            break;
        case Qt::Key_F2:
            keyCode = VK_F2;
            break;
        case Qt::Key_F3:
            keyCode = VK_F3;
            break;
        case Qt::Key_F4:
            keyCode = VK_F4;
            break;
        case Qt::Key_F5:
            keyCode = VK_F5;
            break;
        case Qt::Key_F6:
            keyCode = VK_F6;
            break;
        case Qt::Key_F7:
            keyCode = VK_F7;
            break;
        case Qt::Key_F8:
            keyCode = VK_F8;
            break;
        case Qt::Key_F9:
            keyCode = VK_F9;
            break;
        case Qt::Key_F10:
            keyCode = VK_F10;
            break;
        case Qt::Key_F11:
            keyCode = VK_F11;
            break;
        case Qt::Key_F12:
            keyCode = VK_F12;
            break;
        default:
            qDebug() << "Not a registered special key case";
            break;
        }

        if (registerHotKey(hotKeyId, modifiers, keyCode)) {
            qDebug() << "Hotkey has been changed to: " << keySequence;
        } else {
            qDebug() << "Failed to change hotkey";
        }
    }
}

void MainWindow::on_keySequenceEdit_editingFinished()
{
    qDebug() << "Hotkey change successful!";
    ui->keySequenceEdit->clearFocus();  // Unselect the keySequenceEdit so user doesn't accidentally change it after hotkey recorded
}


/*
AUTOCLICKER RUN
*/
void MainWindow::autoClickerRun() {
    if (ui->checkBox_2->isChecked()) {
        if (ui->comboBox->currentText() == "Left") {
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        } else if (ui->comboBox->currentText() == "Right") {
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
        }

        return;
    }

    if (ui->comboBox->currentText() == "Left") {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

        if (ui->comboBox_2->currentText() == "Double") {
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
    } else if (ui->comboBox->currentText() == "Right") {
        mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
        mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);

        if (ui->comboBox_2->currentText() == "Double") {
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
        }
    }

    int numClicks;

    if (ui->radioButton_2->isChecked()) numClicks = ui->spinBox->value();
    else numClicks = -1;

    if (numClicks != -1)  {
        numClicks--;
        ui->spinBox->setValue(numClicks);
    }
    if (numClicks == 0) on_pushButton_2_clicked();
}


/*
AUTOCLICKER START AND STOP BUTTONS
*/
bool isLeftMouseButtonDown() {
    return GetAsyncKeyState(VK_LBUTTON) & 0x8000;
}

bool isRightMouseButtonDown() {
    return GetAsyncKeyState(VK_RBUTTON) & 0x8000;
}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "Start autoclicker button pressed";
    ui->pushButton->setChecked(true);
    ui->pushButton_2->setChecked(false);

    if (!autoClickerRunning) {
        qDebug() << "Autoclicker running";
        autoClickerRunning = true;

        autoClickerRun();  // Run once before starting timer since timer will wait before first click

        if (ui->spinBox_2->value() == 0 &&
            ui->spinBox_3->value() == 0 &&
            ui->spinBox_4->value() == 0 &&
            ui->spinBox_5->value() == 0)  {
            mainTimer->setInterval(1);
        } else {
            std::uint64_t hours = ui->spinBox_2->value() * 60ULL * 60ULL * 1000ULL;
            std::uint64_t minutes = ui->spinBox_3->value() * 60ULL * 1000ULL;
            std::uint64_t seconds = ui->spinBox_4->value() * 1000ULL;
            std::uint64_t ms = ui->spinBox_5->value();

            unsigned long long int interval =  hours + minutes + seconds + ms;
            qDebug() << "Interval set to: " << std::to_string(interval) << "milliseconds!";
            mainTimer->setInterval(interval);
        }

        mainTimer->start();

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
    if (isLeftMouseButtonDown()) mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    if (isRightMouseButtonDown()) mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
    mainTimer->stop();
}


/*
AUTOCLICKER HOTKEY START AND STOP
*/

bool MainWindow::registerHotKey(int id, int modifiers, int key) {  // Register new hotkeys
    if (!RegisterHotKey((HWND)MainWindow::winId(), id, modifiers, key)) {
        qDebug() << "Failed to register hotkey with ID:" << id;
        return false;
    }
    qDebug() << "Registered hotkey with ID:" << id;
    return true;
}

bool MainWindow::unregisterHotKey(int id) {  // Unregister previous hotkey
    if (!UnregisterHotKey((HWND)MainWindow::winId(), id)) {
        qDebug() << "Failed to unregister hotkey with ID:" << id;
        return false;
    }
    qDebug() << "Unregistered hotkey with ID:" << id;
    return true;
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result){  // Detection of native windows Events like key pressing
    Q_UNUSED(eventType);
    Q_UNUSED(result);

    MSG *msg = reinterpret_cast<MSG*>(message);
    // qDebug() << "Native Event Message Received: " << msg->message << "wParam:" << msg->wParam;
    // qDebug() << ui->keySequenceEdit->keySequence();
    if (msg->message == WM_HOTKEY){
        qDebug() << "Hotkey was pressed";
        if (msg->wParam == 101){
            if (!autoClickerRunning) {
                on_pushButton_clicked();
            } else {
                on_pushButton_2_clicked();
            }
            return true; // Indicate that the message has been handled
        }
    }

    return false;  // Not handled
}

/*
AUTOCLICKER SECION END ===============================================================
*/



