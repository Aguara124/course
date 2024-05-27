#include "studentdbrepository.h"
#include <QSqlQuery>
#include <QThreadStorage>
#include "authdata.h"
#include "authinfo.h"
#include "dbaccess.h"

StudentDbRepository::StudentDbRepository() {}

const std::shared_ptr<Student> StudentDbRepository::getByLogin(const QString &login)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM user WHERE login=:login");
    query.bindValue(":login", login);

    if (query.exec() && query.next()) {
        Student *student = new Student(query);
        if (student->getLogin() != getAllowedLogin()) {
            student->setPassword("********");
        }

        return std::shared_ptr<Student>(student);
    }

    return nullptr;
}

bool StudentDbRepository::addStudent(const Student &student)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    db->transaction();
    try {
        QSqlQuery query(*db);

        query.prepare("INSERT INTO user (iduser, login, `password`, program, `group`, pilot, "
                      "subgroup, name, surname, avatar) VALUES (null, :login, :password, :program, "
                      ":group, :pilot, :subgroup, :name, :surname, :avatar)");

        query.bindValue(":login", student.getLogin());
        query.bindValue(":password", student.getPassword());
        query.bindValue(":program", student.getProgram());
        query.bindValue(":group", student.getGroup());
        query.bindValue(":pilot", student.getPilot());
        query.bindValue(":subgroup", student.getSubgroup());
        query.bindValue(":name", student.getName());
        query.bindValue(":surname", student.getSurname());
        query.bindValue(":avatar", student.getAvatar());

        return query.exec() && db->commit();

    } catch (...) {
        qDebug() << "add student error" << db->lastError().text();
        db->rollback();
    }

    return false;
}

const QList<std::shared_ptr<Student>> StudentDbRepository::getByGroup(int group,
                                                                      int subgroup,
                                                                      const QString &assistantLogin)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    if (assistantLogin.isNull() || assistantLogin.isEmpty()) {
        query.prepare("SELECT distinct u.* FROM user u where u.group=:group and "
                      "u.subgroup=:subgroup");
    } else {
        query.prepare(
            "SELECT distinct u.* FROM user u "
            "inner join assistants a on u.group = a.group and u.subgroup = a.subgroup "
            "where a.login=:assistLogin and u.group=:group and u.subgroup=:subgroup");
        query.bindValue(":assistLogin", assistantLogin);
    }
    query.bindValue(":group", group);
    query.bindValue(":subgroup", subgroup);

    const QString allowedLogin = getAllowedLogin();
    QList<std::shared_ptr<Student>> result;
    if (query.exec()) {
        qDebug() << "--------------------";
        qDebug() << "QUERY getByGroup succes";
        qDebug() << query.size();
        qDebug() << query.lastQuery();
        qDebug() << group << "-" << subgroup << " " << assistantLogin;
        qDebug() << "--------------------";
        while (query.next()) {
            Student *student = new Student(query);
            if (allowedLogin != student->getLogin()) {
                student->setPassword("********");
            }

            result.append(std::shared_ptr<Student>(student));
        }
    }

    return result;
}

bool StudentDbRepository::updateStudent(const Student &student)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();

    db->transaction();
    try {
        QSqlQuery query(*db);

        const QString oldPassword = student.getProgram();
        query.prepare("UPDATE user SET `avatar`=:avatar, password=`password` WHERE `login`=:login "
                      "and `password`=:oldpassword");

        query.bindValue(":login", getAllowedLogin());
        query.bindValue(":password", student.getPassword());
        query.bindValue(":oldpassword", oldPassword);
        query.bindValue(":avatar", student.getAvatar());

        return query.exec() && db->commit();

    } catch (...) {
        qDebug() << "add student error" << db->lastError().text();
        db->rollback();
    }

    return false;
}

const QString StudentDbRepository::getAllowedLogin()
{
    QThreadStorage<AuthInfo *> threadStorage;
    if (threadStorage.hasLocalData() && threadStorage.localData() != nullptr) {
        AuthInfo *authInfo = threadStorage.localData();
        if (authInfo->getRole() == IAuthRepository::STUDENT) {
            return authInfo->getLogin();
        }
    }

    if (AppSettings::instance().getMode() == AppSettings::AppMode::MIXED) {
        if (AuthData::instance().getRole() == IAuthRepository::STUDENT) {
            return AuthData::instance().getLogin();
        }
    }

    return "";
}
