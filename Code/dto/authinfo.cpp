#include "authinfo.h"

AuthInfo::AuthInfo(const QString &login, const IAuthRepository::Role role, const QString &token)
{
    this->login = login;
    this->role = role;
    this->token = token;
}

const QString AuthInfo::getLogin() const
{
    return login;
}

const QString AuthInfo::getToken() const
{
    return token;
}

IAuthRepository::Role AuthInfo::getRole() const
{
    return role;
}
