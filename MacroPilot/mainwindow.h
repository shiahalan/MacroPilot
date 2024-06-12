#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

#include "ui_mainwindow.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTimer *mainTimer;


protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;


private slots:
    void on_keySequenceEdit_keySequenceChanged(const QKeySequence &keySequence);

    void on_keySequenceEdit_editingFinished();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    bool registerHotKey(int id, int modifiers, int key);

    bool unregisterHotKey(int id);

    void autoClickerRun();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
