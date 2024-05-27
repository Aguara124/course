#include "assistantdbrepository.h"
#include <QThreadStorage>
#include "authdata.h"
#include "authinfo.h"
#include "dbaccess.h"

const QList<std::shared_ptr<Assistant>> AssistantDbRepository::getByLogin(
    const QString &assistantLogin)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM assistants WHERE login=:login");
    query.bindValue(":login", assistantLogin);

    const QString allowedLogin = getAllowedLogin();

    QList<std::shared_ptr<Assistant>> result;
    if (query.exec()) {
        while (query.next()) {
            std::shared_ptr<Assistant> assistant = std::shared_ptr<Assistant>(new Assistant(query));
            if (allowedLogin != assistant->getLogin()) {
                assistant->setPassword("*******");
            }

            result.append(assistant);
        }
    }

    return result;
}

const QString AssistantDbRepository::getAllowedLogin()
{
    QThreadStorage<AuthInfo *> threadStorage;
    if (threadStorage.hasLocalData() && threadStorage.localData() != nullptr) {
        AuthInfo *authInfo = threadStorage.localData();
        if (authInfo->getRole() == IAuthRepository::TA) {
            return authInfo->getLogin();
        }
    }

    if (AppSettings::instance().getMode() == AppSettings::AppMode::MIXED) {
        if (AuthData::instance().getRole() == IAuthRepository::TA) {
            return AuthData::instance().getLogin();
        }
    }

    return "";
}

const QList<std::shared_ptr<Assistant>> AssistantDbRepository::getByGroup(int group, int subgroup)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM assistants WHERE `group`=:group and subgroup=:subgroup");
    query.bindValue(":group", group);
    query.bindValue(":subgroup", subgroup);

    const QString allowedLogin = getAllowedLogin();
    QList<std::shared_ptr<Assistant>> result;

    if (query.exec()) {
        while (query.next()) {
            Assistant *assistant = new Assistant(query);
            if (allowedLogin != assistant->getLogin()) {
                assistant->setPassword("*******");
            }

            result.append(std::shared_ptr<Assistant>(assistant));
        }
    }

    return result;
}

const QList<std::shared_ptr<Assistant>> AssistantDbRepository::getByTeacher(
    const QString &teacherLogin)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM assistants WHERE teacher=:teacher");
    query.bindValue(":teacher", teacherLogin);

    const QString allowedLogin = getAllowedLogin();
    QList<std::shared_ptr<Assistant>> result;

    if (query.exec()) {
        while (query.next()) {
            Assistant *assistant = new Assistant(query);
            if (allowedLogin != assistant->getLogin()) {
                assistant->setPassword("*******");
            }

            result.append(std::shared_ptr<Assistant>(assistant));
        }
    }

    return result;
}

bool AssistantDbRepository::updateAssistant(const Assistant &assistant)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();

    db->transaction();
    try {
        QSqlQuery query(*db);

        const QString oldPassword = assistant.getProgram();
        query.prepare("UPDATE assistants SET `avatar`=:avatar, password=`password` WHERE "
                      "`login`=:login and `password`=:oldpassword");

        query.bindValue(":login", getAllowedLogin());
        query.bindValue(":password", assistant.getPassword());
        query.bindValue(":oldpassword", oldPassword);
        query.bindValue(":avatar", assistant.getAvatar());

        return query.exec() && db->commit();

    } catch (...) {
        qDebug() << "update assistant error" << db->lastError().text();
        db->rollback();
    }

    return false;
}
