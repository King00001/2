#include "databaseManager.h"
#include <QSqlError>
#include "property.h"

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{

}

DatabaseManager::DatabaseManager(const map<string, string> &databaseInfo, QObject *parent) : QObject(parent)
{
    //connect(databaseInfo);
}

ReturnInfo DatabaseManager::connect(const map<string, string> &databaseInfo, const QString& connectionName)
{
    map<string, string> databaseInfoTmp;
    for(map<string, string>::const_iterator item = databaseInfo.begin(); item != databaseInfo.end(); ++item){
        if(item->first == "Database.Type"
                ||item->first == "Database.Name"
                ||item->first == "Database.Host"
                ||item->first == "Database.Port"
                ||item->first == "Database.UserName"
                ||item->first == "Database.Password"
                ||item->first == "Database.Options"
                ||item->first == "Database.ODBC"
                )
            databaseInfoTmp.insert(*item);
    }
    ReturnInfo returninfo;
    returninfo.state = Fail;
    returninfo.message = QString("未知状态");

    QString databaseType;
    if(databaseInfoTmp["Database.ODBC"] == "1")
        databaseType = "QODBC";
    else
        databaseType = QString("Q") + QString::fromStdString(databaseInfoTmp["Database.Type"]);

    if(connectionName.isEmpty())
        db = QSqlDatabase::addDatabase(databaseType);
    else
        db = QSqlDatabase::addDatabase(databaseType, connectionName);
    db.setDatabaseName (QString::fromStdString(databaseInfoTmp["Database.Name"]));
    db.setHostName (QString::fromStdString(databaseInfoTmp["Database.Host"]));
    db.setPort(QString::fromStdString(databaseInfoTmp["Database.Port"]).toInt());
    db.setUserName (QString::fromStdString(databaseInfoTmp["Database.UserName"]));
    db.setPassword (QString::fromStdString(databaseInfoTmp["Database.Password"]));
    db.setConnectOptions(QString::fromStdString(databaseInfoTmp["Database.Options"]));

    if(!db.open ())
    {
        returninfo.state = Fail;
        returninfo.message =  QString("不能连接到数据库:") + db.lastError ().text ();
    }
    else{
        returninfo.state = Success;
        returninfo.message = QString("数据库连接成功");
    }

    return returninfo;
}

ReturnInfo DatabaseManager::connect(const QString& connectionName)
{
    Property property;
    Property::State state = property.load("../config/config");
    if(state != Property::Success){
        emit message("File Error");
    }
    return connect(property.getProperty(), connectionName);
}
