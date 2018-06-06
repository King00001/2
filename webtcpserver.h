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

#ifndef WEBTCPSERVER_H
#define WEBTCPSERVER_H

#include <functional>

#include <QTcpServer>
#include <QVector>

class WebWorker;

namespace Tufao {
class HttpServerRequest;
class HttpServerResponse;
} // namespace Tufao

class WebTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    typedef std::function<void(Tufao::HttpServerRequest&,
                               Tufao::HttpServerResponse&)> Handler;
    typedef std::function<Handler()> Factory;

    explicit WebTcpServer(QObject *parent = 0);

    void run(int threadsNumber, int port, Factory factory);

signals:
    void initReady();
    void newConnection(int socketDescriptor);

    void newMessage(QByteArray msg);

    void message(QString msg);


    void switchPortState(QString id, bool stata, uchar sendType);
protected:
    void incomingConnection(qintptr handle);

private:
    QVector<WebWorker*> workers;
    int i;
private slots:
    void onMessage(QString msg);
};

#endif // WEBTCPSERVER_H
