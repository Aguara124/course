#ifndef AUTHINFO_H
#define AUTHINFO_H

#include <QCache>
#include "authrepository.h"

class AuthInfo
{
private:
    QString login;
    QString token;

    IAuthRepository::Role role;

public:
    AuthInfo(const QString &login, const IAuthRepository::Role role, const QString &token);
    const QString getLogin() const;
    const QString getToken() const;
    IAuthRepository::Role getRole() const;

};

#endif // AUTHINFO_H
