#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include "webtcpserver.h"
#include "devicetcpserver.h"

class Service : public QObject
{
    Q_OBJECT
public:
    explicit Service(QObject *parent = nullptr);
    WebTcpServer m_webTcpServer;

private:

    DeviceTcpServer m_deviceTcpServer;

signals:
    void message(const QString &msg);

public slots:
    void onMessage(const QString &msg);
};

#endif // SERVICE_H
