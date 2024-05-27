#include "token.h"

quint32 Token::getId() const
{
    return id;
}

void Token::setId(quint32 newId)
{
    id = newId;
}

QString Token::getLogin() const
{
    return login;
}

void Token::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString Token::getIpAddr() const
{
    return ipAddr;
}

void Token::setIpAddr(const QString &newIpAddr)
{
    ipAddr = newIpAddr;
}

QString Token::getToken() const
{
    return token;
}

void Token::setToken(const QString &newToken)
{
    token = newToken;
}

IAuthRepository::Role Token::getRole() const
{
    return role;
}

void Token::setRole(IAuthRepository::Role newRole)
{
    role = newRole;
}

quint64 Token::getExpiredAt() const
{
    return expiredAt;
}

void Token::setExpiredAt(quint64 newExpiredAt)
{
    expiredAt = newExpiredAt;
}

Token::Token() {}

Token::Token(const QSqlQuery &query)
{
    id = query.value("id").toUInt();
    login = query.value("login").toString();
    token = query.value("token").toString();
    ipAddr = query.value("ipaddr").toString();
    role = static_cast<IAuthRepository::Role>(query.value("role").toInt());
    expiredAt = query.value("exp").toULongLong();
}

Token::Token(const Token &sourceToken)
{
    id = sourceToken.getId();
    login = sourceToken.getLogin();
    token = sourceToken.getToken();
    ipAddr = sourceToken.getIpAddr();
    role = sourceToken.getRole();
    expiredAt = sourceToken.getExpiredAt();
}

Token &Token::operator=(const Token &sourceToken)
{
    if (this != &sourceToken)
    {
        id = sourceToken.getId();
        login = sourceToken.getLogin();
        token = sourceToken.getToken();
        ipAddr = sourceToken.getIpAddr();
        role = sourceToken.getRole();
        expiredAt = sourceToken.getExpiredAt();
    }
    return *this;
}
