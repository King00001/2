#include "websocketmanager.h"
#include <QDebug>
#include <QUuid>

WebSocketManager::WebSocketManager(const QString &loginId, WebSocket *webSocket, QObject *parent) : QObject(parent), m_loginId(loginId), m_WebSocket(webSocket)
{
    m_id = QUuid::createUuid().toString();
    connect(this, &WebSocketManager::sendMessage,
            m_WebSocket, &Tufao::AbstractMessageSocket::sendMessage);

    connect(m_WebSocket, &WebSocket::disconnected, this, &WebSocketManager::onDisconnected);
    connect(m_WebSocket, &WebSocket::newMessage, this, &WebSocketManager::onMessage);
}

WebSocketManager::~WebSocketManager()
{
    //qDebug() << "~WebSocketManager";

    if(m_WebSocket)
        emit m_WebSocket->deleteLater();
}

void WebSocketManager::onDisconnected()
{
    emit disconnected(m_id);
}

void WebSocketManager::onMessage(const QByteArray &message)
{
    qDebug() << message;
    m_WebSocket->sendMessage(message);
}
