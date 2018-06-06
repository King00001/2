#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QObject>
#include "type.h"
#include <QSqlDatabase>
#include <map>
using namespace std;

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    explicit DatabaseManager(const map<string, string> &databaseInfo, QObject *parent = nullptr);

    DatabaseManager(const map<string, string> &databaseInfo);
    ReturnInfo connect(const map<string, string> &databaseInfo, const QString& connectionName = "");
    ReturnInfo connect(const QString& connectionName = "");
    bool isOpen(){
        return getDb().isOpen();
    }
    QSqlDatabase getDb(){
        return db;
    }
signals:
    void message(QString msg, int timeout = 0);

public slots:

private:
    QSqlDatabase db;

};

#endif // DATABASE_MANAGER_H
