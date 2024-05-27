#ifndef TOKEN_H
#define TOKEN_H

#include <QtCore>
#include "../authrepository.h"
#include "QSqlQuery"

class Token
{
private:
    quint32 id;
    QString login;
    QString token;
    QString ipAddr;

    IAuthRepository::Role role;
    quint64 expiredAt;

public:
    Token();
    Token(const QSqlQuery &query);
    Token(const Token &token);

    Token &operator=(const Token &other);

    quint32 getId() const;
    void setId(quint32 newId);
    QString getLogin() const;
    void setLogin(const QString &newLogin);
    QString getIpAddr() const;
    void setIpAddr(const QString &newIpAddr);
    QString getToken() const;
    void setToken(const QString &newToken);
    IAuthRepository::Role getRole() const;
    void setRole(IAuthRepository::Role newRole);
    quint64 getExpiredAt() const;
    void setExpiredAt(quint64 newExpiredAt);
};

#endif // TOKEN_H
