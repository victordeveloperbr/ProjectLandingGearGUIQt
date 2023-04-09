
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "senderthread.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
   explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int waitingToResponse {1000};

   private slots:
    void gearUPTransaction();
    void gearDownTransaction();
    void showResponse(const QString &s);
    void processError(const QString &s);
    void processTimeout(const QString &s);

private:
    int m_transactionCount = 0;
    Ui::MainWindow *ui;

    SenderThread m_thread;
};

#endif // MAINWINDOW_H
