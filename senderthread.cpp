
#include "senderthread.h"

#include <QSerialPort>
#include <QTime>


SenderThread::SenderThread(QObject *parent)
    : QThread{parent}
{

}
SenderThread::~SenderThread()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}

void SenderThread::executeTransaction(const QString &portConnection, int waitTimeout, const QString &solicitation)
{
    const QMutexLocker locker(&m_mutex);
    m_portName = portConnection;
    m_waitTimeout = waitTimeout;
    m_solicitation = solicitation;

    if(!isRunning())
        start();
    else
        m_cond.wakeOne();
}

void SenderThread::run()
{
    bool currentPortSelectedChanged = false;

    m_mutex.lock();

    QString currentPortSelected;
    if(currentPortSelected != m_portName) {
        currentPortSelected = m_portName;
        currentPortSelectedChanged = true;
    }

    int currentWaitTimeout = m_waitTimeout;
    QString request = m_solicitation;
    m_mutex.unlock();

    QSerialPort serial;

    if(currentPortSelected.isEmpty()) {
        emit error(tr("port not found"));
        return;
    }

    while(!m_quit) {
        if (currentPortSelectedChanged) {
            serial.close();
            serial.setPortName(currentPortSelected);

            if(!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Can't open %1, error code %2")
                               .arg(m_portName).arg(serial.error()));
                return;

            }
        }

        // writting solicitation
        const QByteArray requestData = request.toUtf8();
        serial.write(requestData);
        if(serial.waitForBytesWritten(m_waitTimeout))
        {
            //read response
            if(serial.waitForReadyRead(currentWaitTimeout)) {
                QByteArray responseData = serial.readAll();
                while (serial.waitForReadyRead(10))
                    responseData += serial.readAll();

                const QString response = QString::fromUtf8(responseData);

                emit this->response(response);
            }
            else
            {
                emit timeout(tr("Wait read response timeout &1")
                                 .arg(QTime::currentTime().toString()));
            }
        }
        else
        {
            emit timeout(tr("Wait write request timeout %1")
                             .arg(QTime::currentTime().toString()));
        }
        m_mutex.lock();
        m_cond.wait(&m_mutex);
        if(currentPortSelected != m_portName) {
            currentPortSelected = m_portName;
            currentPortSelectedChanged = true;
        } else {
            currentPortSelectedChanged = false;
        }

        currentWaitTimeout = m_waitTimeout;
        request = m_solicitation;
        m_mutex.unlock();
    }
}
