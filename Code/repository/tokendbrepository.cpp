#include "tokendbrepository.h"
#include <QSqlDatabase>
#include <repository/dbaccess.h>

TokenDbRepository::TokenDbRepository() {}

const Token TokenDbRepository::findByToken(const QString &tokenValue) const
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);
    query.prepare("SELECT * FROM tokens WHERE token=:token");
    query.bindValue(":token", tokenValue);

    if (query.exec() && query.next()) {
        return Token(query);
    }

    return Token();
}

bool TokenDbRepository::insert(const Token &token)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    db->transaction();

    try {
        QSqlQuery query(*db);
        query.prepare(
            "INSERT INTO tokens (id, login, role, token, exp, ipaddr) VALUES (null, :login, :role, "
            ":token, :exp, :ipaddr)");

        query.bindValue(":login", token.getLogin());
        query.bindValue(":role", (int) token.getRole());
        query.bindValue(":token", token.getToken());
        query.bindValue(":exp", token.getExpiredAt());
        query.bindValue(":ipaddr", token.getIpAddr());

        query.exec();

        db->commit();

        return true;
    } catch (...) {
        db->rollback();
    }

    return false;
}

bool TokenDbRepository::clearExpired()
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    db->transaction();

    try {
        QSqlQuery clearQuery(*db);
        clearQuery.prepare("DELETE FROM tokens WHERE exp < :exp");
        clearQuery.bindValue(":exp", QDateTime::currentMSecsSinceEpoch());
        clearQuery.exec();
        db->commit();

        return true;
    } catch (...) {
        db->rollback();
    }

    return false;
}
