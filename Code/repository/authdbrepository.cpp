#include "authdbrepository.h"
#include <QSqlQuery>
#include <QThreadStorage>
#include "QDateTime"
#include "dbaccess.h"
#include "encryption.h"
#include <dto/authinfo.h>

#include "authdata.h"
#include "repositorymanager.h"
#include "token.h"
#include "tokenrepository.h"

const qint64 AuthDbRepository::tokenLifeTime = 3600000;

bool AuthDbRepository::authenticate(const QString &login,
                                    const QString &password,
                                    const Role role,
                                    const QString &ipAddr)
{
    QSqlQuery query(*DBAccess::instance().getDatabaseCopy());

    if (role == Role::TA) {
        query.prepare("SELECT * FROM assistants WHERE login=:login and `password`=:password");
    } else if (role == Role::STUDENT) {
        query.prepare("SELECT * FROM user WHERE login=:login and `password`=:password");
    } else if (role == Role::TEACHER) {
        query.prepare("SELECT * FROM teachers WHERE login=:login and `password`=:password");
    } else if (role == Role::UNKNOWN) {
        return authenticate(login, password, Role::STUDENT, ipAddr)
               || authenticate(login, password, Role::TA, ipAddr)
               || authenticate(login, password, Role::TEACHER, ipAddr);
    }

    query.bindValue(":login", login);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
        setToken(login, role, ipAddr);
        return true;
    }

    return false;
}

void AuthDbRepository::setToken(const QString &login, const Role role, const QString &ipAddr)
{
    const QString tokenTemplate = "%1-%2-%3-%4";

    Token token;
    token.setLogin(login);
    token.setRole(role);
    token.setIpAddr(ipAddr);
    token.setExpiredAt(QDateTime::currentMSecsSinceEpoch() + tokenLifeTime);

    const QString rawToken = tokenTemplate.arg(token.getExpiredAt()).arg(login).arg(role).arg(ipAddr);
    token.setToken(EncryptionHelper::encrypt(rawToken.toUtf8()).toBase64());

    std::unique_ptr<ITokenRepository> repository = RepositoryManager::instance().tokenRepository();
    repository->insert(token);

    threadStorage.setLocalData(new AuthInfo(login, role, token.getToken()));

    QThreadStorage<AuthInfo *> localThreadStorage;
    localThreadStorage.setLocalData(new AuthInfo(login, role, token.getToken()));

    if (AppSettings::instance().getMode() == AppSettings::AppMode::MIXED) {
        AuthData::instance().setToken(token.getToken());
        AuthData::instance().setRole(token.getRole());
        AuthData::instance().setLogin(token.getLogin());
    }

    repository->clearExpired();
}

bool AuthDbRepository::checkToken(const QString &queryToken, const QString &ipAddr)
{
    if (queryToken.isNull() || queryToken.isEmpty()) {
        return false;
    }

    std::unique_ptr<ITokenRepository> repository = RepositoryManager::instance().tokenRepository();
    Token token = repository->findByToken(queryToken);

    if (!token.getLogin().isEmpty()) {
        AuthInfo *authInfo = new AuthInfo(token.getLogin(), token.getRole(), queryToken);

        if (token.getIpAddr() != ipAddr) {
            qDebug() << QString("Wrong IP Address for token %1 (stored %2, received %3)")
                            .arg(queryToken)
                            .arg(token.getIpAddr(), ipAddr);

            return false;
        }

        if (token.getExpiredAt() < (quint64) QDateTime::currentMSecsSinceEpoch()) {
            qDebug() << QString("Expired for token %1 (stored %2, current %3)")
                            .arg(queryToken)
                            .arg(token.getExpiredAt())
                            .arg(QDateTime::currentMSecsSinceEpoch());
            return false;
        }

        if (token.getExpiredAt()
            < (quint64) QDateTime::currentMSecsSinceEpoch() + (tokenLifeTime >> 1)) {
            qDebug() << QString("Renewing for token %1 (stored %2, current %3)")
                            .arg(queryToken)
                            .arg(token.getExpiredAt())
                            .arg(QDateTime::currentMSecsSinceEpoch());

            setToken(authInfo->getLogin(), authInfo->getRole(), ipAddr);
        } else {
            //mixed
            threadStorage.setLocalData(authInfo);
            // current thread
            QThreadStorage<AuthInfo *> localThreadStorage;
            localThreadStorage.setLocalData(
                new AuthInfo(token.getLogin(), token.getRole(), queryToken));
        }

        return true;
    }

    return false;
}

bool AuthDbRepository::checkLogin(const QString &login)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM user WHERE login=:login");
    query.bindValue(":login", login);

    if (query.exec() && query.next()) {
        return true;
    }

    query.finish();
    query.prepare("SELECT * FROM assistants WHERE login=:login");
    query.bindValue(":login", login);

    if (query.exec() && query.next()) {
        return true;
    }

    query.finish();
    query.prepare("SELECT * FROM teachers WHERE login=:login");
    query.bindValue(":login", login);

    if (query.exec() && query.next()) {
        return true;
    }

    return false;
}
