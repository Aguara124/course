#ifndef AUTHDBREPOSITORY_H
#define AUTHDBREPOSITORY_H

#include "authrepository.h"

#include <QThreadStorage>
#include "authinfo.h"

class AuthDbRepository : public IAuthRepository
{
private:
    const static qint64 tokenLifeTime;
    QThreadStorage<AuthInfo *> threadStorage;
    void setToken(const QString &login, const Role role, const QString &ipAddr);

public:
    AuthDbRepository() {}

public:
    bool authenticate(const QString &login,
                      const QString &password,
                      const Role role,
                      const QString &ipAddr = QString()) override;

public:
    bool checkToken(const QString &token, const QString &ipAddr) override;
    bool checkLogin(const QString &login) override;
};

#endif // AUTHDBREPOSITORY_H
