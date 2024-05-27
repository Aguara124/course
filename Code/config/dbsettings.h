#ifndef DBSETTINGS_H
#define DBSETTINGS_H

class DBSettings {


private:
    QString host;
    int port;
    QString databaseName;
    QString userName;
    QString password;
public:

    DBSettings() {}

    QString getHost() {
        return host;
    }
    void setHost(QVariant host) {
        this->host = host.toString();
    }

    QString getUsername() {
        return userName;
    }
    void setUsername(QVariant username) {
        this->userName = username.toString();
    }

    int getPort() {
        return port;
    }
    void setPort(QVariant port) {
        this->port = port.toInt();
    }

    QString getDatabaseName() {
        return databaseName;
    }
    void setDatabaseName(QVariant dbName) {
        this->databaseName = dbName.toString();
    }

    QString getPassword() {
        return password;
    }

    void setPassword(QVariant password) {
        setPassword(password.toString());
    }

    void setPassword(QString password) {
        this->password = password;
    }

};


#endif // DBSETTINGS_H
