#include "authdata.h"
#include "rwlockhelper.h"

const QString AuthData::getApiUrl()
{
    RwLockHelper lock(rwLock);
    return apiUrl;
}

void AuthData::setApiUrl(const QString &newApiUrl)
{
    RwLockHelper lock(rwLock, false);
    apiUrl = newApiUrl;
}

IAuthRepository::Role AuthData::getRole() const
{
    RwLockHelper lock(rwLock);
    return role;
}

void AuthData::setRole(IAuthRepository::Role newRole)
{
    RwLockHelper lock(rwLock, false);
    role = newRole;
}

QString AuthData::getLogin() const
{
    RwLockHelper lock(rwLock);
    return login;
}

void AuthData::setLogin(const QString &newLogin)
{
    RwLockHelper lock(rwLock, false);
    login = newLogin;
}

const QString AuthData::getToken()
{
    RwLockHelper lock(rwLock);
    return token;
}

void AuthData::setToken(const QString &newToken)
{
    RwLockHelper lock(rwLock, false);
    token = newToken;
}
