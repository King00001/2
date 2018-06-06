#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QObject>
#include <QReadWriteLock>
#include "property.h"
#include "type.h"
#include <QReadWriteLock>
#include "websocketmanager.h"

using namespace std;

using namespace Tufao;

class GlobalData : public QObject
{
    Q_OBJECT
public:

    static GlobalData &defaultInstance();

    Property property;
    PortQueMapMap getPort(){
        return m_portQueMapMap;
    }
    void setPort(const Port &port);
    void setPort(const PortQueMapMap& portQueMapMap);
    PortQueMap getPortQueMap(const QString &themeId);
    Port getPort(const QString &id);
    Port getPort(const QString &ip, int row, int col, int type);

    TcpSocketManager *getTcpSocketManager(const QString &id);
    void addTcpSocketManager(const QString &id, TcpSocketManager *tcpSocketManager);
    void removeTcpSocketManager(const QString &id);

    void addWebSocketManager(const QString &id, WebSocketManager *webSocketManager);
    void removeWebSocketManager(const QString &id);
    //WebSocketManager *getWebSocketManager(const QString &id);
    WebSocketManagerPtrQue getWebSocketManagerByLoginId(const QString &loginId);

    Theme getTheme(const QString &id);
    void setTheme(const ThemeMap &themeMap);

    void setUserThemeId(const QStringListMap &userTemeMap);
    QStringList getUserThemeId(const QString &userId);

    void loadPortUserMap();
    QStringList getPortUser(const QString &portId);

    void setPortAction(const PortActionMap &portActionMap);
    PortActionQue getPortActionBySourceId(const QString &sourceId);

    //Property::State loadProperty(const string &path);
    void setProperty(const string &key, const string &value);
    void setProperty(const PropertyType &property);
    PropertyType getProperty();
    string getProperty(const string &key);
    string getPropertyWithDefault(const string &key, const string &defaultStr);
signals:

public slots:

private:
    explicit GlobalData(QObject *parent = nullptr);
    QReadWriteLock m_porReadWriteLock;

    PortQueMapMap m_portQueMapMap;
    PortPtrMap m_portPtrMap;
    QReadWriteLock m_portReadWriteLock;

    TcpSocketManagerMap m_tcpSocketManagerMap;
    QReadWriteLock m_tcpSocketManagerMapReadWriteLock;

    WebSocketManagerMap m_webWorkerManagerMap;
    QReadWriteLock m_webWorkerManagerReadWriteLock;

    ThemeMap m_themeMap;
    QReadWriteLock m_themeMapReadWriteLock;

    QStringListMap m_userTemeMap;
    QReadWriteLock m_userTemeMapReadWriteLock;

    QStringListMap m_portUserMap;
    QReadWriteLock m_portUserMapReadWriteLock;

    PortActionPtrQueMap m_portActionPtrQueMap;
    PortActionMap m_portActionMap;
    QReadWriteLock m_portActionMapReadWriteLock;

    Property m_property;
    QReadWriteLock m_propertyReadWriteLock;
};

#endif // GLOBALDATA_H
