
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    ui->setupUi(this);

    const auto infos = QSerialPortInfo::availablePorts();
    // insert port to read by combo
    for(const QSerialPortInfo &info : infos)
        ui->cbPortCommunication->addItem(info.portName());

    ui->cbPortCommunication->setFocus();
    connect(ui->btnGearUp, &QPushButton::clicked, this, &MainWindow::gearUPTransaction);
    connect(ui->btnGearDown, &QPushButton::clicked, this, &MainWindow::gearDownTransaction);
    connect(&m_thread, &SenderThread::response, this, &MainWindow::showResponse);
    connect(&m_thread, &SenderThread::error, this, &MainWindow::processError);
    connect(&m_thread, &SenderThread::timeout, this, &MainWindow::processTimeout);

    m_thread.executeTransaction(ui->cbPortCommunication->currentText(), waitingToResponse, "GEAR_DOWN");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// when click here landing gear up
void MainWindow::gearUPTransaction()
{
    ui->lblStatus->setText("Landing Gear Up!");
    m_thread.executeTransaction(ui->cbPortCommunication->currentText(), waitingToResponse, "GEAR_UP");

}

void MainWindow::gearDownTransaction()
{
    ui->lblStatus->setText("Landing Gear Down!");
    m_thread.executeTransaction(ui->cbPortCommunication->currentText(), waitingToResponse, "GEAR_DOWN");
}

void MainWindow::showResponse(const QString &s)
{
    ui->lblStatus->setText(tr("Traffic, transaction #%1:"
                               "\n\r-request: %2"
                               "\n\r-response: %3")
                                .arg(++m_transactionCount)
                                .arg("none")
                                .arg(s));
}

void MainWindow::processError(const QString &s)
{
   // setControlsEnabled(true);
    ui->lblStatus->setText(tr("Status: Not running, %1.").arg(s));
    ui->lblStatus->setText(tr("No traffic."));
}

void MainWindow::processTimeout(const QString &s)
{
    //setControlsEnabled(true);
    ui->lblStatus->setText(tr("Status: Running, %1.").arg(s));
    ui->lblStatus->setText(tr("No traffic."));
}
