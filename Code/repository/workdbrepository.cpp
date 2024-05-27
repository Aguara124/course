#include "workdbrepository.h"
#include <QThreadStorage>
#include "authdata.h"
#include "authinfo.h"
#include "dbaccess.h"

WorkDbRepository::WorkDbRepository() {}

const std::shared_ptr<Work> WorkDbRepository::getById(int id)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM works WHERE idworks=:id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        Work *work = new Work(query);
        return (allowedByRole() || currentLogin() == work->getLogin()) ? std::shared_ptr<Work>(work)
                                                                       : nullptr;
    }

    return nullptr;
}

bool WorkDbRepository::updateWork(const Work &work)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    db->transaction();
    try {
        query.prepare(
            "UPDATE `works` SET `login` = :login, `name`=:name, `surname`=:surname, "
            "`assign`=:assign, `flag_pdf`=:flag_pdf,"
            "`group` =:group, `dateTime`=now(), "
            "`subgroup`=:subgroup,`idassignments`=:idassignments,`comment`=:comment, "
            "`checked`=:checked, `late`=((select a.deadline from "
            "assignments a where a.idassignments=:idassignments) < now()) WHERE `idworks`=:id");

        query.bindValue(":login", currentLogin());
        query.bindValue(":name", work.getName());
        query.bindValue(":surname", work.getSurName());
        query.bindValue(":assign", work.getAssign());
        query.bindValue(":flag_pdf", work.getIsPdf());
        query.bindValue(":group", work.getGroup());
        query.bindValue(":subgroup", work.getSubGroup());
        query.bindValue(":idassignments", work.getAssignmentId());
        query.bindValue(":comment", work.getComment());
        query.bindValue(":checked", work.getIsChecked());
        query.bindValue(":idworks", work.getId());

        return query.exec() && db->commit();
    } catch (...) {
        db->rollback();
        qDebug() << db->lastError().text();
    }

    return false;
}

const QList<std::shared_ptr<Work>> WorkDbRepository::getByAssignmentId(int assignmentId)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM works WHERE idassignments=:id");
    query.bindValue(":id", assignmentId);

    QList<std::shared_ptr<Work>> result;
    if (query.exec()) {
        bool allowed = allowedByRole();
        QString login = currentLogin();

        while (query.next()) {
            Work *work = new Work(query);
            if (allowed || login == work->getLogin()) {
                result.append(std::shared_ptr<Work>(work));
            }
        }
    }

    return result;
}

bool WorkDbRepository::addWork(const Work &work)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    db->transaction();
    try {
        query.prepare("INSERT INTO `works` (`idworks`, `login`, `name`, `surname`, "
                      "`dateTime`, `assign`, `flag_pdf`,"
                      "`group`, "
                      "`subgroup`,`idassignments`,`comment`,`late`,`checked`) VALUES ("
                      "null, :login, :name, :surname, "
                      "now(), :assign, :flag_pdf,"
                      ":group, :subgroup, :idassignments, :comment, (select a.deadline from "
                      "assignments a where a.idassignments=:idassignments) < now(), :checked)");

        query.bindValue(":login", currentLogin());
        query.bindValue(":name", work.getName());
        query.bindValue(":surname", work.getSurName());
        query.bindValue(":assign", work.getAssign());
        query.bindValue(":flag_pdf", work.getIsPdf());
        query.bindValue(":group", work.getGroup());
        query.bindValue(":subgroup", work.getSubGroup());
        query.bindValue(":idassignments", work.getAssignmentId());
        query.bindValue(":comment", work.getComment());
        query.bindValue(":checked", work.getIsChecked());
        query.bindValue(":idworks", work.getId());

        return query.exec() && db->commit();
    } catch (...) {
        db->rollback();
        qDebug() << db->lastError().text();
    }

    return false;
}

const QList<std::shared_ptr<Work>> WorkDbRepository::getByStudentLogin(const QString &studentLogin)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM works WHERE login=:login");
    query.bindValue(":login", studentLogin);

    QList<std::shared_ptr<Work>> result;
    if (query.exec()) {
        bool allowed = allowedByRole();
        QString login = currentLogin();

        while (query.next()) {
            Work *work = new Work(query);
            if (allowed || login == work->getLogin()) {
                result.append(std::shared_ptr<Work>(work));
            }
        }
    }

    return result;
}

const QString WorkDbRepository::currentLogin()
{
    QThreadStorage<AuthInfo *> threadStorage;
    if (threadStorage.hasLocalData() && threadStorage.localData() != nullptr) {
        AuthInfo *authInfo = threadStorage.localData();
        return authInfo->getLogin();
    }

    if (AppSettings::instance().getMode() == AppSettings::AppMode::MIXED) {
        return AuthData::instance().getLogin();
    }

    qDebug() << "current login is missed!";
    return nullptr;
}

bool WorkDbRepository::allowedByRole()
{
    QThreadStorage<AuthInfo *> threadStorage;
    if (threadStorage.hasLocalData() && threadStorage.localData() != nullptr) {
        AuthInfo *authInfo = threadStorage.localData();
        if (authInfo->getRole() == IAuthRepository::TA
            || authInfo->getRole() == IAuthRepository::TEACHER) {
            return true;
        }
    }

    if (AppSettings::instance().getMode() == AppSettings::AppMode::MIXED) {
        if (AuthData::instance().getRole() == IAuthRepository::TA
            || AuthData::instance().getRole() == IAuthRepository::TEACHER) {
            return true;
        }
    }

    return false;
}
