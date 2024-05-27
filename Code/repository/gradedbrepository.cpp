#include "gradedbrepository.h"
#include <QThreadStorage>
#include "authdata.h"
#include "authinfo.h"
#include <repository/dbaccess.h>

const QList<std::shared_ptr<Grade>> GradeDbRepository::getByFilter(const QString &nameOfStudent,
                                                                   const QString &name,
                                                                   const QString &surName,
                                                                   const QString &subject,
                                                                   const QStringList modules,
                                                                   const QString &elementOfControl,
                                                                   const QString &examinerLogin)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    QString strQuery = "SELECT * FROM grades";
    QStringList predicates;
    QVariantMap parameters;

    if (!nameOfStudent.isNull() && !nameOfStudent.isEmpty()) {
        QStringList subPredicates;
        subPredicates.append("nameOfStudent=:nameOfStudent");
        parameters.insert(":nameOfStudent", nameOfStudent);

        if (!name.isNull() && !name.isEmpty()) {
            subPredicates.append("name=:name");
            parameters.insert(":name", name);
        }

        if (!surName.isNull() && !surName.isEmpty()) {
            subPredicates.append("surname=:surname");
            parameters.insert(":surname", surName);
        }

        if (subPredicates.size() > 1) {
            predicates.append("(" + subPredicates.join(" OR ") + ")");
        } else {
            predicates.append(subPredicates.first());
        }
    }

    if (!subject.isNull() && !subject.isEmpty()) {
        predicates.append("subject=:subject");
        parameters.insert(":subject", subject);
    }

    if (!elementOfControl.isNull() && !elementOfControl.isEmpty()) {
        predicates.append("elementOfControl=:elementOfControl");
        parameters.insert(":elementOfControl", elementOfControl);
    }

    if (!examinerLogin.isNull() && !examinerLogin.isEmpty()) {
        predicates.append("examinerLogin=:examinerLogin");
        parameters.insert(":examinerLogin", examinerLogin);

        QThreadStorage<AuthInfo *> threadStorage;
        if (threadStorage.hasLocalData() && threadStorage.localData() != nullptr) {
            AuthInfo *authInfo = threadStorage.localData();
            if (authInfo->getRole() == IAuthRepository::TA
                && examinerLogin != authInfo->getLogin()) {
                qWarning()
                    << QString("security warninng: grades mismatch ta %1 requested grades for %2")
                           .arg(authInfo->getLogin())
                           .arg(examinerLogin);
            }
        }
    }

    if (!modules.isEmpty()) {
        QStringList subPredicates;
        int nIndex = 0;
        for (QString module : modules) {
            QString paramName = QString(":module%1").arg(nIndex++);
            subPredicates.append("module=" + paramName);
            parameters.insert(paramName, module);
        }

        if (subPredicates.size() > 1) {
            predicates.append("(" + subPredicates.join(" OR ") + ")");
        } else {
            predicates.append(subPredicates.first());
        }
    }

    if (predicates.size() > 1) {
        query.prepare(strQuery + " WHERE " + predicates.join(" AND "));
    } else if (predicates.size() == 1) {
        query.prepare(strQuery + " WHERE " + predicates.first());
    } else {
        query.prepare(strQuery);
    }

    for (auto [k, v] : parameters.asKeyValueRange()) {
        query.bindValue(k, v);
    }

    QList<std::shared_ptr<Grade>> result;
    if (query.exec()) {
        while (query.next()) {
            result.append(std::shared_ptr<Grade>(new Grade(query)));
        }
    }

    return result;
}

bool GradeDbRepository::addGrade(const Grade &grade)
{
    if (allowedByRole()) {
        QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
        QSqlQuery query(*db);

        db->transaction();

        try {
            query.prepare(
                "INSERT INTO `grades` "
                "(`idgrades`,`nameOfStudent`,`subject`,`mark`,`date`,`gradeTime`,`comment`,`"
                "elementOfControl`,`module`,`examinerLogin`,`name`,`surname`,`idassignments`,"
                "`nameAssignment`,`flag`) "
                "VALUES (null, :nameOfStudent, :subject, :mark, :date, :gradeTime, :comment, "
                ":elementOfControl, :module, :examinerLogin, :name, :surname, "
                ":idassignments, :nameAssignment, :flag)");
            query.bindValue(":nameOfStudent", grade.getNameOfStudent());
            query.bindValue(":subject", grade.getSubject());
            query.bindValue(":mark", grade.getMark());
            query.bindValue(":date", QDate::currentDate());
            query.bindValue(":gradeTime", QTime::currentTime());
            query.bindValue(":comment", grade.getComment());
            query.bindValue(":elementOfControl", grade.getElementOfControl());
            query.bindValue(":module", grade.getModule());
            query.bindValue(":examinerLogin", currentLogin());
            query.bindValue(":name", grade.getName());
            query.bindValue(":surname", grade.getSurName());
            query.bindValue(":idassignments", grade.getAssignmentId());
            query.bindValue(":nameAssignment", grade.getAssignmentName());
            query.bindValue(":flag", grade.getAssigned());

            return query.exec() && db->commit();
        } catch (...) {
            db->rollback();
            qDebug() << db->lastError().text();
        }
    }

    return false;
}

bool GradeDbRepository::updateGrade(const Grade &grade)
{
    if (allowedByRole()) {
        QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
        QSqlQuery query(*db);

        db->transaction();

        try {
            query.prepare("UPDATE `grades` "
                          "SET `nameOfStudent`=:nameOfStudent,"
                          "`subject`=:subject,"
                          "`mark`=:mark,"
                          "`date`=:date,"
                          "`gradeTime`=:gradeTime,"
                          "`comment`=:comment,"
                          "`elementOfControl`=:elementOfControl,"
                          "`module`=:module,"
                          "`examinerLogin`=:examinerLogin,"
                          "`name`=:name,"
                          "`surname`=:surname,"
                          "`idassignments`=:idassignments,"
                          "`nameAssignment`=:nameAssignment,"
                          "`flag`=:flag "
                          "WHERE `idgrades`=:id");
            query.bindValue(":nameOfStudent", grade.getNameOfStudent());
            query.bindValue(":subject", grade.getSubject());
            query.bindValue(":mark", grade.getMark());
            query.bindValue(":date", QDate::currentDate());
            query.bindValue(":gradeTime", QTime::currentTime());
            query.bindValue(":comment", grade.getComment());
            query.bindValue(":elementOfControl", grade.getElementOfControl());
            query.bindValue(":module", grade.getModule());
            query.bindValue(":examinerLogin", currentLogin());
            query.bindValue(":name", grade.getName());
            query.bindValue(":surname", grade.getSurName());
            query.bindValue(":idassignments", grade.getAssignmentId());
            query.bindValue(":nameAssignment", grade.getAssignmentName());
            query.bindValue(":flag", grade.getAssigned());
            query.bindValue(":id", grade.getId());

            return query.exec() && db->commit();
        } catch (...) {
            db->rollback();
            qDebug() << db->lastError().text();
        }
    }

    return false;
}

bool GradeDbRepository::removeGrade(const Grade &grade)
{
    if (allowedByRole()) {
        QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
        QSqlQuery query(*db);

        db->transaction();

        try {
            query.prepare("DELETE FROM `grades` WHERE `idgrades`=:id");
            query.bindValue(":id", grade.getId());
            return query.exec() && db->commit();
        } catch (...) {
            db->rollback();
            qDebug() << db->lastError().text();
        }
    }

    return false;
}

const QList<std::shared_ptr<Grade>> GradeDbRepository::getByAssignmentId(int assignmentId)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM grades where idassignments=:id");
    query.bindValue(":id", assignmentId);

    QList<std::shared_ptr<Grade>> result;
    if (query.exec()) {
        while (query.next()) {
            result.append(std::shared_ptr<Grade>(new Grade(query)));
        }
    }

    return result;
}

const QString GradeDbRepository::currentLogin()
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

bool GradeDbRepository::allowedByRole()
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
