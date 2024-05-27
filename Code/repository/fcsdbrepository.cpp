#include "fcsdbrepository.h"
#include "QThreadStorage"
#include "authdata.h"
#include "authinfo.h"
#include "dbaccess.h"

FcsDbRepository::FcsDbRepository() {}

const QList<std::shared_ptr<Fcs>> FcsDbRepository::getFcsByFilter(const QString &program,
                                                                  bool pilot,
                                                                  int course)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM fcs WHERE program=:program AND pilot=:pilot AND course=:course");
    query.bindValue(":program", program);
    query.bindValue(":pilot", pilot);
    query.bindValue(":course", course);

    QList<std::shared_ptr<Fcs>> result;
    if (query.exec()) {
        while (query.next()) {
            result.append(std::shared_ptr<Fcs>(new Fcs(query)));
        }
    }

    return result;
}

const QList<std::shared_ptr<Fcs>> FcsDbRepository::getFcsByLecturer(const QString &lecturer,
                                                                    const QString &program,
                                                                    const QString &subject)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    QString strQuery = "SELECT * FROM fcs";
    QStringList predicates;
    predicates.append("lecturer=:lecturer");

    QVariantMap parameters;
    parameters.insert(":lecturer", lecturer);

    if (!program.isNull() && !program.isEmpty()) {
        predicates.append("program=:program");
        parameters.insert(":program", program);
    }

    if (!subject.isNull() && !subject.isEmpty()) {
        predicates.append("subject=:subject");
        parameters.insert(":subject", subject);
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

    QList<std::shared_ptr<Fcs>> result;
    if (query.exec()) {
        while (query.next()) {
            result.append(std::shared_ptr<Fcs>(new Fcs(query)));
        }
    }

    return result;
}

bool FcsDbRepository::update(const Fcs &fcs)
{
    if (allowedByRole()) {
        QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
        QSqlQuery query(*db);

        db->transaction();

        try {
            query.prepare("UPDATE `fcs` "
                          "SET "
                          "`program` = :program, "
                          "`subject` = :subject, "
                          "`course` = :course, "
                          "`pilot` = :pilot, "
                          "`formula1st` = :formula1, "
                          "`formula2nd` = :formula2, "
                          "`formulaFinal` = :formulaFinal, "
                          "`lecturer` = :lecturer "
                          " WHERE `idsubjects` = :id");

            query.bindValue(":program", fcs.getProgram());
            query.bindValue(":subject", fcs.getSubject());
            query.bindValue(":course", fcs.getCourse());
            query.bindValue(":pilot", fcs.getPilot());
            query.bindValue(":formula1", fcs.getFormula1());
            query.bindValue(":formula2", fcs.getFormula2());
            query.bindValue(":formulaFinal", fcs.getFormulaFinal());
            query.bindValue(":lecturer", fcs.getLecturer());
            query.bindValue(":id", fcs.getId());
            return query.exec() && db->commit();
        } catch (...) {
            db->rollback();
            qDebug() << db->lastError().text();
        }
    }

    return false;
}

const std::shared_ptr<Fcs> FcsDbRepository::getById(int id)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM fcs WHERE `idsubjects`=:id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return std::shared_ptr<Fcs>(new Fcs(query));
    }

    return nullptr;
}

const QList<std::shared_ptr<Fcs>> FcsDbRepository::getFaculty()
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT DISTINCT faculty, facultyName FROM fcs");

    QList<std::shared_ptr<Fcs>> result;
    if (query.exec()) {
        qDebug() << query.lastError();
        while (query.next()) {
            result.append(std::shared_ptr<Fcs>(new Fcs(query)));
        }
    }

    return result;
}

const QList<std::shared_ptr<Fcs>> FcsDbRepository::getProgram(const QString &faculty)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    if (faculty.isNull() || faculty.isEmpty()) {
        query.prepare("SELECT DISTINCT program, programName FROM fcs");
    } else {
        query.prepare("SELECT DISTINCT program, programName FROM fcs WHERE faculty=:faculty OR "
                      "facultyName=:faculty");
        query.bindValue(":faculty", faculty);
    }

    QList<std::shared_ptr<Fcs>> result;
    if (query.exec()) {
        while (query.next()) {
            result.append(std::shared_ptr<Fcs>(new Fcs(query)));
        }
    }

    return result;
}

bool FcsDbRepository::allowedByRole()
{
    QThreadStorage<AuthInfo *> threadStorage;
    if (threadStorage.hasLocalData() && threadStorage.localData() != nullptr) {
        AuthInfo *authInfo = threadStorage.localData();
        if (authInfo->getRole() == IAuthRepository::TEACHER) {
            return true;
        }
    }

    if (AppSettings::instance().getMode() == AppSettings::AppMode::MIXED) {
        if (AuthData::instance().getRole() == IAuthRepository::TEACHER) {
            return true;
        }
    }

    return false;
}
