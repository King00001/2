#include "globaldata.h"
#include <QReadWriteLock>
#include "ndbpoolhelper.h"
#include "tcpsocketmanager.h"
#include "type.h"

GlobalData::GlobalData(QObject *parent) : QObject(parent), m_porReadWriteLock(QReadWriteLock::Recursive), property("../config/config"), m_portReadWriteLock(QReadWriteLock::Recursive), m_tcpSocketManagerMapReadWriteLock(QReadWriteLock::Recursive), m_webWorkerManagerReadWriteLock(QReadWriteLock::Recursive), m_themeMapReadWriteLock(QReadWriteLock::Recursive), m_userTemeMapReadWriteLock(QReadWriteLock::Recursive), m_portUserMapReadWriteLock(QReadWriteLock::Recursive)
{
}

GlobalData &GlobalData::defaultInstance()
{
    static GlobalData instance;
    return instance;
}

void GlobalData::setPort(const Port &port)
{
    {
        QWriteLocker locker(&m_portReadWriteLock);
        PortPtrMap::iterator posPortPtrMap = m_portPtrMap.find(port.id);
        if(posPortPtrMap != m_portPtrMap.end()){
            *(posPortPtrMap.value()) = port;
        }else{
            m_portQueMapMap[port.themeId][port.group].push_back(port);
            m_portPtrMap.insert(port.id, &(*m_portQueMapMap[port.themeId][port.group].rbegin()));
        }
    }

}
void GlobalData::setPort(const PortQueMapMap& portQueMapMap)
{
    {
        QWriteLocker locker(&m_portReadWriteLock);
        m_portQueMapMap = portQueMapMap;
        m_portPtrMap.clear();

        for(PortQueMapMap::iterator itemPortQueMapMap = m_portQueMapMap.begin(); itemPortQueMapMap != m_portQueMapMap.end(); ++itemPortQueMapMap){
            for(PortQueMap::iterator itemPortQueMap = itemPortQueMapMap.value().begin(); itemPortQueMap != itemPortQueMapMap.value().end(); ++itemPortQueMap){
                for(PortQue::iterator itemPortQue = itemPortQueMap.value().begin(); itemPortQue != itemPortQueMap.value().end(); ++itemPortQue){
                    Port &port(*itemPortQue);
                    m_portPtrMap.insert(port.id, &port);
                }
            }
        }
    }
    {
        loadPortUserMap();
    }
}

PortQueMap GlobalData::getPortQueMap(const QString &themeId)
{
    QReadLocker locker(&m_portReadWriteLock);
    PortQueMapMap::const_iterator itemPortQueMapMap = m_portQueMapMap.find(themeId);
    if(itemPortQueMapMap != m_portQueMapMap.end())
        return itemPortQueMapMap.value();

    return PortQueMap();
}

Port GlobalData::getPort(const QString &id)
{
    QReadLocker locker(&m_portReadWriteLock);
    PortPtrMap::ConstIterator pos = m_portPtrMap.find(id);
    if(pos != m_portPtrMap.end()){
        return *(pos.value());
    }
    return Port();
}
Port GlobalData::getPort(const QString &ip, int row, int col, int type)
{
    QReadLocker locker(&m_portReadWriteLock);
    for(PortQueMapMap::const_iterator posPortQueMapMap = m_portQueMapMap.begin(); posPortQueMapMap != m_portQueMapMap.end(); ++posPortQueMapMap){
        const PortQueMap &portQueMap(posPortQueMapMap.value());
        for(PortQueMap::const_iterator posPortQueMap = portQueMap.begin(); posPortQueMap != portQueMap.end(); ++posPortQueMap){
            for(PortQue::const_iterator posPortQue = posPortQueMap.value().begin(); posPortQue != posPortQueMap.value().end(); ++posPortQue){
                const Port &port(*posPortQue);
                if(port.ip == ip && port.row == row && port.col == col && port.type == type){
                    return port;
                }
            }
        }
    }
    return Port();
}
TcpSocketManager *GlobalData::getTcpSocketManager(const QString &id)
{
    QReadLocker locker(&m_tcpSocketManagerMapReadWriteLock);
    TcpSocketManagerMap::iterator pos = m_tcpSocketManagerMap.find(id);
    if(m_tcpSocketManagerMap.end() != pos){
        return pos.value();
    }
    return NULL;
}

void GlobalData::addTcpSocketManager(const QString &id, TcpSocketManager *tcpSocketManager)
{
    QWriteLocker locker(&m_tcpSocketManagerMapReadWriteLock);
//    TcpSocketManagerMap::iterator pos = m_tcpSocketManagerMap.find(id);
//    if(pos != m_tcpSocketManagerMap.end() && pos.value()->getTcpSocket() != tcpSocketManager->getTcpSocket()){
//        removeTcpSocketManager(id);
//    }
//    m_tcpSocketManagerMap[id] = (tcpSocketManager);
    removeTcpSocketManager(id);
    m_tcpSocketManagerMap[id] = (tcpSocketManager);
}

void GlobalData::removeTcpSocketManager(const QString &id)
{
    QWriteLocker locker(&m_tcpSocketManagerMapReadWriteLock);
    TcpSocketManagerMap::iterator pos = m_tcpSocketManagerMap.find(id);
    if(m_tcpSocketManagerMap.end() != pos){
        pos.value()->deleteLater();
        m_tcpSocketManagerMap.erase(pos);
    }

}

void GlobalData::addWebSocketManager(const QString &id, WebSocketManager *webSocketManager)
{
    QWriteLocker locker(&m_webWorkerManagerReadWriteLock);
//    WebSocketManagerMap::iterator pos = m_webWorkerManagerMap.find(id);
//    if(pos != m_webWorkerManagerMap.end() && pos.value() != webSocketManager){
//        removeWebSocketManager(id);
//    }
    removeWebSocketManager(id);
    m_webWorkerManagerMap[id] = (webSocketManager);
}

void GlobalData::removeWebSocketManager(const QString &id)
{
    QWriteLocker locker(&m_tcpSocketManagerMapReadWriteLock);
    WebSocketManagerMap::iterator pos = m_webWorkerManagerMap.find(id);
    if(m_webWorkerManagerMap.end() != pos){
        pos.value()->deleteLater();
        m_webWorkerManagerMap.erase(pos);
    }
}

//WebSocketManager *GlobalData::getWebSocketManager(const QString &id)
//{
//    QReadLocker locker(&m_tcpSocketManagerMapReadWriteLock);

//    WebSocketManagerMap::iterator pos = m_webWorkerManagerMap.find(id);
//    if(m_webWorkerManagerMap.end() != pos){
//        return pos.value();
//    }
//    return NULL;
//}

WebSocketManagerPtrQue GlobalData::getWebSocketManagerByLoginId(const QString &loginId)
{
    WebSocketManagerPtrQue webSocketManagerPtrs;
    for(WebSocketManagerMap::const_iterator item = m_webWorkerManagerMap.begin(); item != m_webWorkerManagerMap.end(); ++item){
        WebSocketManager *webSocketManagerPtr = item.value();
        if(webSocketManagerPtr && webSocketManagerPtr->getloginId() == loginId){
            webSocketManagerPtrs.push_back(webSocketManagerPtr);
        }
    }
    return webSocketManagerPtrs;
}

Theme GlobalData::getTheme(const QString &id)
{
    QReadLocker locker(&m_themeMapReadWriteLock);
    ThemeMap::ConstIterator pos = m_themeMap.find(id);
    if(pos != m_themeMap.end()){
        return pos.value();
    }
    return Theme();
}

void GlobalData::setTheme(const ThemeMap &themeMap)
{
    QWriteLocker locker(&m_themeMapReadWriteLock);
    m_themeMap = themeMap;
}

void GlobalData::setUserThemeId(const QStringListMap &userTemeMap)
{
    {
        QWriteLocker locker(&m_userTemeMapReadWriteLock);
        m_userTemeMap = userTemeMap;
    }
    {
        loadPortUserMap();
    }
}

QStringList GlobalData::getUserThemeId(const QString &userId)
{
    QReadLocker locker(&m_userTemeMapReadWriteLock);
    QStringListMap::const_iterator pos = m_userTemeMap.find(userId);
    if(pos != m_userTemeMap.end()){
        return pos.value();
    }
    return QStringList();
}

void GlobalData::loadPortUserMap()
{
    QWriteLocker locker(&m_portUserMapReadWriteLock);
    QReadLocker locker2(&m_userTemeMapReadWriteLock);
    QReadLocker locker3(&m_portReadWriteLock);
    for(QStringListMap::const_iterator posQStringListMap = m_userTemeMap.begin(); posQStringListMap != m_userTemeMap.end(); ++posQStringListMap){//用户包含主提
        foreach (QString temeId, posQStringListMap.value()) {
            PortQueMapMap::const_iterator posPortQueMapMap = m_portQueMapMap.find(temeId);
            if(posPortQueMapMap != m_portQueMapMap.end()){
                for(PortQueMap::const_iterator posPortQueMap = posPortQueMapMap.value().begin(); posPortQueMap != posPortQueMapMap.value().end(); ++posPortQueMap){//主题下面的分组
                    for(PortQue::const_iterator posPortQue = posPortQueMap.value().begin(); posPortQue != posPortQueMap.value().end(); ++posPortQue){//分组下面的端口
                        const Port &port(*posPortQue);
                        m_portUserMap[port.id] << posQStringListMap.key();
                    }
                }
            }
        }
    }

}

QStringList GlobalData::getPortUser(const QString &portId)
{
    QReadLocker locker(&m_portUserMapReadWriteLock);
    QStringListMap::const_iterator posPortUserMap = m_portUserMap.find(portId);
    if(posPortUserMap != m_portUserMap.end())
        return posPortUserMap.value();

    return QStringList();
}

void GlobalData::setPortAction(const PortActionMap &portActionMap)
{
    QWriteLocker locker(&m_portActionMapReadWriteLock);
    m_portActionMap = portActionMap;
    m_portActionPtrQueMap.clear();
    for(PortActionMap::iterator posPortActionMap = m_portActionMap.begin(); posPortActionMap != m_portActionMap.end(); ++posPortActionMap){
        m_portActionPtrQueMap[posPortActionMap.value().sourceId].push_back(&posPortActionMap.value());
    }
}

PortActionQue GlobalData::getPortActionBySourceId(const QString &sourceId)
{
    PortActionQue portActionQue;
    QReadLocker locker(&m_portActionMapReadWriteLock);
    if(m_portActionPtrQueMap.find(sourceId) != m_portActionPtrQueMap.end()){
        for(PortActionPtrQue::iterator posPortActionPtrQue = m_portActionPtrQueMap[sourceId].begin(); posPortActionPtrQue != m_portActionPtrQueMap[sourceId].end(); ++posPortActionPtrQue){
            portActionQue.push_back(*(*posPortActionPtrQue));
        }
    }
    return portActionQue;
}

//Property::State GlobalData::loadProperty(const string &path)
//{
//    QWriteLocker locker(&m_propertyReadWriteLock);
//    m_property.load(path);
//}

void GlobalData::setProperty(const string &key, const string &value)
{
    QWriteLocker locker(&m_propertyReadWriteLock);
    m_property.setProperty(key, value);
}

void GlobalData::setProperty(const PropertyType &property)
{
    QWriteLocker locker(&m_propertyReadWriteLock);
    m_property.setProperty(property);
}

PropertyType GlobalData::getProperty()
{
    QWriteLocker locker(&m_propertyReadWriteLock);
    return m_property.getProperty();
}

string GlobalData::getProperty(const string &key)
{
    QWriteLocker locker(&m_propertyReadWriteLock);
    return m_property.getProperty(key);
}

string GlobalData::getPropertyWithDefault(const string &key, const string &defaultStr)
{
    QWriteLocker locker(&m_propertyReadWriteLock);
    return m_property.getPropertyWithDefault(key, defaultStr);
}
