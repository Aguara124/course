#include "teacherdbrepository.h"
#include <QThreadStorage>
#include "authdata.h"
#include "authinfo.h"
#include "dbaccess.h"

TeacherDbRepository::TeacherDbRepository() {}

const QList<std::shared_ptr<Teacher>> TeacherDbRepository::getByLogin(const QString &teacherLogin)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM teachers WHERE login=:login");
    query.bindValue(":login", teacherLogin);

    QList<std::shared_ptr<Teacher>> result;
    if (query.exec()) {
        while (query.next()) {
            Teacher *teacher = new Teacher(query);
            if (teacher->getLogin() != getAllowedLogin()) {
                teacher->setPassword("********");
            }

            result.append(std::shared_ptr<Teacher>(teacher));
        }
    }

    return result;
}

const QString TeacherDbRepository::getAllowedLogin()
{
    QThreadStorage<AuthInfo *> threadStorage;
    if (threadStorage.hasLocalData() && threadStorage.localData() != nullptr) {
        AuthInfo *authInfo = threadStorage.localData();
        if (authInfo->getRole() == IAuthRepository::TEACHER) {
            return authInfo->getLogin();
        }
    }

    if (AppSettings::instance().getMode() == AppSettings::AppMode::MIXED) {
        if (AuthData::instance().getRole() == IAuthRepository::TEACHER) {
            return AuthData::instance().getLogin();
        }
    }

    return "";
}

bool TeacherDbRepository::updateTeacher(const Teacher &teacher)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();

    db->transaction();
    try {
        QSqlQuery query(*db);

        const QString oldPassword = teacher.getProgram();
        query.prepare("UPDATE teachers SET `avatar`=:avatar, password=`password` WHERE "
                      "`login`=:login and `password`=:oldpassword");

        query.bindValue(":login", getAllowedLogin());
        query.bindValue(":password", teacher.getPassword());
        query.bindValue(":oldpassword", oldPassword);
        query.bindValue(":avatar", teacher.getAvatar());

        return query.exec() && db->commit();

    } catch (...) {
        qDebug() << "update teacher error" << db->lastError().text();
        db->rollback();
    }

    return false;
}
