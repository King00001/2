/*
  Copyright (c) 2012 Vinícius dos Santos Oliveira

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  */

#include "webtcpserver.h"
#include "webworker.h"

#include <QThread>
#include <Tufao/HttpServer>
#include <Tufao/HttpsServer>
#include "databaseManager.h"
#include <iostream>
#include <QDebug>
#include "type.h"

using namespace std;
using namespace Tufao;

WebTcpServer::WebTcpServer(QObject *parent) :
    QTcpServer(parent),
    i(0)
{
//    DatabaseManager dbManager(parent);
//    ReturnInfo returninfo = dbManager.connect();
//    qDebug() << returninfo.message;
    connect(this, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));

}

void WebTcpServer::run(int threadsNumber, int port, Factory factory)
{

    Tufao::SimpleSessionStore &store(Tufao::SimpleSessionStore::defaultInstance());
    store.setRefreshInterval(2000000000);

    workers.reserve(threadsNumber);
    for (int i = 0;i != threadsNumber;++i) {

        WebWorker *worker = new WebWorker;
        QThread *workerThread = new QThread(this);

        connect(worker, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));

        worker->moveToThread(workerThread);
        workerThread->start();

        worker->setFactory(factory);

        connect(worker, &WebWorker::newMessage,
                this, &WebTcpServer::newMessage);

        connect(this, &WebTcpServer::newMessage,
                worker, &WebWorker::sendMessage);

        connect(worker, &WebWorker::switchPortState, this, &WebTcpServer::switchPortState);

        workers.push_back(worker);
    }
    listen(QHostAddress::Any, port);
}

void WebTcpServer::incomingConnection(qintptr handle)
{
    workers[(++i) % workers.size()]->addConnection(handle);
}

void WebTcpServer::onMessage(QString msg)
{
    qDebug() << msg << endl;
}
