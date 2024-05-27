#ifndef AUTHDATA_H
#define AUTHDATA_H

#include <QReadWriteLock>
#include <QString>
#include "appsettings.h"
#include "authrepository.h"
#include "singleton.h"

class AuthData : public Singleton<AuthData>
{
private:
    QString token;
    QString apiUrl;
    IAuthRepository::Role role;
    QString login;

    QReadWriteLock *rwLock;

public:
    AuthData()
        : rwLock(new QReadWriteLock())
    {}
    ~AuthData() { delete rwLock; }

    const QString getToken();
    void setToken(const QString &newToken);

    const QString getApiUrl();
    void setApiUrl(const QString &newApiUrl);

    IAuthRepository::Role getRole() const;
    void setRole(IAuthRepository::Role newRole);

    QString getLogin() const;
    void setLogin(const QString &newLogin);
};

#endif // AUTHDATA_H
