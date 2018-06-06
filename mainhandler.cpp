/*
  Copyright (c) 2012, 2013 Vinícius dos Santos Oliveira

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

#include "mainhandler.h"
#include "usercontroller.h"
#include "controller.h"
#include "themecontroller.h"
#include "portcontroller.h"

#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>
#include <QtCore/QUrl>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QDir>
#include <QJsonParseError>
#include <QJsonObject>
#include <QCryptographicHash>

#include <Tufao/HttpFileServer>
#include <iostream>
#include "globaldata.h"
#include "typedef.h"
#include "filecontroller.h"

using namespace std;

MainHandler::MainHandler(QObject *parent) :
    QObject(parent)
{
    connect(this, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
}

bool MainHandler::handleRequest(Tufao::HttpServerRequest &request,
                                Tufao::HttpServerResponse &response)
{
    Tufao::SimpleSessionStore &store(Tufao::SimpleSessionStore::defaultInstance());
    //qDebug() << "refreshInterval()" << store.refreshInterval();
    const QByteArray method(request.method());
    QString path = request.url().path();
    if (method == "GET"/* && method != "HEAD"*/) {
        if (path == "/"){
            //                        Tufao::SimpleSessionStore &store(Tufao::SimpleSessionStore::defaultInstance());

            //                        store.setProperty(request, response, "isLogin", "no");
            //qDebug() << store.property(request, response, "isLogin");
            //handler(*request, *response);
            Controller controller(this);
            controller.sendRedirect(response, "/web");
            return true;
        }

        if (path == "/web") {
            if(store.hasSession(request)){
                QString loginId = store.property(request, response, "loginId").toString();
                if(!loginId.isEmpty()){
                    QUrl url(request.url());
                    url.setPath("/home.html");
                    request.setUrl(url);
                    return false;
                }
            }
            Controller controller(this);
            controller.sendRedirect(response, "/web/login");
            return true;
        }
        else if (path == "/web/login") {
            QUrl url(request.url());
            url.setPath("/login.html");
            request.setUrl(url);
        }
        else if (path == "/web/logout") {
            store.removeProperty(request, response, "loginId");
            store.removeSession(request, response);
            QUrl url(request.url());
            url.setPath("/login.html");
            request.setUrl(url);
        }
    }
    else if(method == "POST"){
        if (path == "/web/fileList") {
            FileController fileController(this);
            fileController.fileList(request, response);
            return true;
        }

        if (path == "/web/login") {
            UserController userController(this);
            connect(&userController, SIGNAL(message(QString)), this, SIGNAL(message(QString)));

            //            QString pwd="aa";
            //            QString md5;
            //            QByteArray ba,bb;
            //            QCryptographicHash md(QCryptographicHash::Md5);
            //            ba.append(pwd);
            //            md.addData(ba);
            //            bb = md.result();
            //            md5.append(bb.toHex());
            //            qDebug() << md5;
            userController.login(request, response);
            return true;
        }
        else{
            if(!store.hasSession(request) || store.property(request, response, "loginId").toString().isEmpty()){
                Controller controller(this);
                controller.sendRedirectPost(response, "/web/login");
                return true;
            }
        }

        if (path == "/web/getLogin") {
            UserController userController(this);
            connect(&userController, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
            userController.getLogin(request, response);
            return true;
        }
        else if (path == "/web/getTheme") {
            if(store.hasSession(request)){
                QString loginId = store.property(request, response, "loginId").toString();
                if(!loginId.isEmpty()){
                    ThemeController themeController(this);
                    connect(&themeController, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
                    themeController.getTheme(request, response, loginId);
                    return true;
                }
                else{
                    Controller controller(this);
                    controller.sendFail(response);
                    return true;
                }
            }
        }
        else if (path == "/web/getPort") {
            if(store.hasSession(request)){
                QString loginId = store.property(request, response, "loginId").toString();
                if(!loginId.isEmpty()){
                    PortController portController(this);
                    connect(&portController, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
                    portController.getPort(request, response, loginId);
                    return true;
                }
                else{
                    Controller controller(this);
                    controller.sendFail(response);
                    return true;
                }
            }
            return false;
        }
        else if (path == "/web/swithPortState") {

            QJsonParseError json_error;
            QString portId;

            //QTJSON所有的元素

            QByteArray body = request.readBody();

            QJsonDocument parse_doucment = QJsonDocument::fromJson(body, &json_error);
            //检查json是否有错误
            if (json_error.error == QJsonParseError::NoError)
            {
                if (parse_doucment.isObject())
                {
                    //开始解析json对象
                    QJsonObject obj = parse_doucment.object();
                    if (obj.contains("portId"))
                    {
                        QJsonValue value = obj.take("portId");
                        if (value.isString())
                        {
                            portId = value.toVariant().toString();
                        }
                    }
                }
            }

            qDebug() << "portId:" << portId;
            GlobalData &globalData(GlobalData::defaultInstance());
            Port port = globalData.getPort(portId);
            qDebug() << "port.ip:" << port.ip;

            Controller controller(this);
            if(!port.ip.isEmpty()){
                emit switchPortState(portId, !port.stata, SEND_TYPE_ACTIVE);
                controller.sendSuccess(response);
                return true;
            }
            else{
                emit message(QString("切换端口的IP为空"));
                controller.sendFail(response);
                return true;
            }
        }
        return false;
    }
    return false;
}

void MainHandler::onMessage(QString msg)
{
    qDebug() << msg << endl;
}
