#ifndef WEBWORKER_H
#define WEBWORKER_H

#include <QObject>
#include <QMutex>

#include "webtcpserver.h"
#include "mainhandler.h"

class WebWorker : public QObject
{
    Q_OBJECT
public:
    WebWorker(QObject *parent = 0);

    void setFactory(WebTcpServer::Factory factory);
    void addConnection(qintptr socketDescriptor);

    void sendRedirect(Tufao::HttpServerResponse &response, const QString &href);
signals:
    void newConnection(qintptr socketDescriptor);
    void initReady();

    void newMessage(QByteArray msg);
    void sendMessage(QByteArray msg);

    void message(QString msg);

    void switchPortState(QString id, bool stata, uchar sendType);
private slots:
    void init();
    void onNewConnection(qintptr socketDescriptor);
    void onRequestReady();
    void onUpgrade();

    void onWebSocketDisconnected(QString id);

public slots:

private:
    WebTcpServer::Factory factory;
    QMutex factoryMutex;
    WebTcpServer::Handler handler;


    MainHandler m_mainHandler;
};

#endif // WEBWORKER_H
