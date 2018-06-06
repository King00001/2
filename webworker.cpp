#include "webworker.h"

#include <QtNetwork/QTcpSocket>

#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>
#include <Tufao/WebSocket>
#include <QUrl>
#include <QThread>
#include <Tufao/SimpleSessionStore>
#include "type.h"
#include "globaldata.h"
#include "websocketmanager.h"

using namespace Tufao;

WebWorker::WebWorker(QObject *parent) : QObject(parent), m_mainHandler(this)
{
    connect(this, SIGNAL(initReady()), this, SLOT(init()),
            Qt::QueuedConnection);
    connect(this, &WebWorker::newConnection/*SIGNAL(newConnection(qintptr))*/,
            this, &WebWorker::onNewConnection/*SLOT(onNewConnection(qintptr))*/,
            Qt::QueuedConnection);
    connect(&m_mainHandler, &MainHandler::switchPortState, this, &WebWorker::switchPortState);
}

void WebWorker::setFactory(WebTcpServer::Factory factory)
{
    factoryMutex.lock();
    this->factory = factory;
    factoryMutex.unlock();
    emit initReady();
}

void WebWorker::addConnection(qintptr socketDescriptor)
{
    emit newConnection(socketDescriptor);
}

void WebWorker::init()
{
    factoryMutex.lock();
    handler = factory();
    factoryMutex.unlock();
}

void WebWorker::onNewConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return;
    }

    HttpServerRequest *handle = new HttpServerRequest(*socket, this);

    connect(handle, SIGNAL(ready()), this, SLOT(onRequestReady()));
    connect(handle, SIGNAL(upgrade()), this, SLOT(onUpgrade()));
    connect(socket, SIGNAL(disconnected()), handle, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void WebWorker::onRequestReady()
{
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());

    QAbstractSocket &socket = request->socket();
    HttpServerResponse *response
            = new HttpServerResponse(socket, request->responseOptions(), this);

    connect(&socket, SIGNAL(disconnected()), response, SLOT(deleteLater()));
    connect(response, SIGNAL(finished()), response, SLOT(deleteLater()));
    connect(response, &HttpServerResponse::finished, request, &HttpServerRequest::resume);

    if (request->headers().contains("Expect", "100-continue"))
        response->writeContinue();

    if(!m_mainHandler.handleRequest(*request, *response)){
        handler(*request, *response);
    }
}

void WebWorker::onUpgrade()
{
   // qDebug() << "onUpgrade";
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());

    QAbstractSocket &socket = request->socket();
    HttpServerResponse *response
            = new HttpServerResponse(socket, request->responseOptions(), this);

    if (request->url().path() == "/ws") {
        Tufao::SimpleSessionStore &store(Tufao::SimpleSessionStore::defaultInstance());
        if(store.hasSession(*request)){
            QString loginId = store.property(*request, *response, "loginId").toString();
            if(!loginId.isEmpty()){
                Tufao::WebSocket *socket = new Tufao::WebSocket(this);
                socket->startServerHandshake(*request, request->readBody());
                socket->setMessagesType(Tufao::WebSocketMessageType::TEXT_MESSAGE);

//                connect(socket, &Tufao::AbstractMessageSocket::disconnected,
//                        socket, &QObject::deleteLater);

//                connect(socket, &Tufao::AbstractMessageSocket::newMessage,
//                        this, &WebWorker::newMessage);
//                connect(this, &WebWorker::sendMessage,
//                        socket, &Tufao::AbstractMessageSocket::sendMessage);

                WebSocketManager * webSocketManager = new WebSocketManager(loginId, socket,this);
                connect(webSocketManager, &WebSocketManager::disconnected, this, &WebWorker::onWebSocketDisconnected);
                GlobalData &globalData(GlobalData::defaultInstance());
                globalData.addWebSocketManager(webSocketManager->getId(), webSocketManager);
                //emit webSocketManager->sendMessage("message test");
            }
        }
    }

#if 0
    if (request->url().path() == "/ws") {
//        Tufao::SimpleSessionStore &store(Tufao::SimpleSessionStore::defaultInstance());
//        if(store.hasSession(*request)){
//            QString loginId = store.property(*request, *response, "loginId").toString();
//            if(!loginId.isEmpty()){
                Tufao::WebSocket *socket = new Tufao::WebSocket(this);
                socket->startServerHandshake(*request, request->readBody());
                socket->setMessagesType(Tufao::WebSocketMessageType::TEXT_MESSAGE);

//                connect(socket, &Tufao::AbstractMessageSocket::disconnected,
//                        socket, &QObject::deleteLater);

//                connect(socket, &Tufao::AbstractMessageSocket::newMessage,
//                        this, &WebWorker::newMessage);
//                connect(this, &WebWorker::sendMessage,
//                        socket, &Tufao::AbstractMessageSocket::sendMessage);

                WebSocketManager * webSocketManager = new WebSocketManager("loginId", socket,this);
                connect(webSocketManager, &WebSocketManager::disconnected, this, &WebWorker::onWebSocketDisconnected);
                GlobalData &globalData(GlobalData::defaultInstance());
                globalData.addWebSocketManager(webSocketManager->getId(), webSocketManager);
                emit webSocketManager->sendMessage("message test");
//            }
//        }
    }
#endif
    //    priv->upgradeHandler(*request, request->readBody());
    delete request;
    delete response;

    //    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());
    //    delete request;
}

void WebWorker::onWebSocketDisconnected(QString id)
{
    GlobalData &globalData(GlobalData::defaultInstance());
    globalData.removeWebSocketManager(id);

}
