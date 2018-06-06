#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QObject>
#include <Tufao/WebSocket>
using namespace Tufao;

class WebSocketManager : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketManager(const QString &loginId, WebSocket *webSocket, QObject *parent = nullptr);
    ~WebSocketManager();

    const QString &getId(){
        return m_id;
    }

    const QString &getloginId(){
        return m_loginId;
    }
private:
    QString m_id;
    QString m_loginId;
    WebSocket *m_WebSocket;

signals:
    void sendMessage(const QByteArray &sendArray);
    void disconnected(QString id);

public slots:
    void onMessage(const QByteArray &message);

private slots:
    void onDisconnected();
};

#endif // WEBSOCKETMANAGER_H
