#include "service.h"

#include <Tufao/HttpFileServer>
#include <Tufao/HttpServerRequestRouter>
#include <Tufao/Headers>
#include <QDebug>

#include "mainhandler.h"

#include "ndbpool.h"
#include "globaldata.h"
#include "ndbpoolhelper.h"
#include "portcontroller.h"
#include "themecontroller.h"
#include <QCoreApplication>

Service::Service(QObject *parent) : QObject(parent), m_webTcpServer(this), m_deviceTcpServer(this)
{

    connect(this, &Service::message, this, &Service::onMessage);

    NDBPool::instance().setDebug(false);
    GlobalData &globalData(GlobalData::defaultInstance());
    {

        {
            Property property("../config/config");
            globalData.setProperty(property.getProperty());
        }

        PortQueMapMap portQueMapMap;
        PortController portController(this);
        ReturnInfo returnInfo = portController.getPort(portQueMapMap);
        emit message(returnInfo.message);
        emit message(QString::number(portQueMapMap.size()));
        if(returnInfo.state == Success){
            globalData.setPort(portQueMapMap);
        }

        PortActionMap portActionMap;
        returnInfo = portController.getPortAction(portActionMap);
        emit message(returnInfo.message);
        emit message(QString::number(portActionMap.size()));
        if(returnInfo.state == Success){
            globalData.setPortAction(portActionMap);
        }

        ThemeMap themeMap;
        ThemeController themeController(this);
        returnInfo = themeController.getTheme(themeMap);
        emit message(returnInfo.message);
        emit message(QString::number(themeMap.size()));
        if(returnInfo.state == Success){
            globalData.setTheme(themeMap);
        }

        QStringListMap userTemeIdsMap;
        returnInfo = themeController.getUserTemeIdsMap(userTemeIdsMap);
        emit message(returnInfo.message);
        emit message(QString::number(userTemeIdsMap.size()));
        if(returnInfo.state == Success){
            globalData.setUserThemeId(userTemeIdsMap);
        }

    }

    m_deviceTcpServer.run(QString::fromStdString(globalData.getPropertyWithDefault("Server.Device.Thread.Count", "4")).toInt()
                          , QString::fromStdString(globalData.getPropertyWithDefault("Server.NetWork.Device.Tcp.Port", "6888")).toInt());

    m_webTcpServer.run(QString::fromStdString(globalData.getPropertyWithDefault("Server.Web.Thread.Count", "4")).toInt()
                       , QString::fromStdString(globalData.getPropertyWithDefault("Server.NetWork.Web.Port", "80")).toInt()
                       , []() {
        QSharedPointer<Tufao::HttpServerRequestRouter> router;
        return [router](Tufao::HttpServerRequest &request,
                Tufao::HttpServerResponse &response) mutable {
            //MainHandler mainHandler;

            if (!router) {
                router.reset(new Tufao::HttpServerRequestRouter);
                //                router->map({QRegularExpression{""},
                //                             mainHandler});
                router->map({QRegularExpression{""},
                             Tufao::HttpFileServer::handler(QString::fromStdString(GlobalData::defaultInstance().getProperty("Server.Web.Root")))});

            }

            if (!router->handleRequest(request, response)) {
                response.writeHead(Tufao::HttpResponseStatus::NOT_FOUND);
                response.headers().insert("Content-Type", "text/plain");
                response.end("Not found\n");
            }

            return true;
        };
    });


    connect(&m_webTcpServer, &WebTcpServer::switchPortState, &m_deviceTcpServer, &DeviceTcpServer::onSwitchPortState);
}

void Service::onMessage(const QString &msg)
{
    qDebug() << msg;
}
