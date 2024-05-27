#include "consultationdbrepository.h"
#include "dbaccess.h"

const std::shared_ptr<Consultation> ConsultationDbRepository::getById(int id)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM consultations WHERE idconsultations=:id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return std::shared_ptr<Consultation>(new Consultation(query));
    }

    return nullptr;
}

bool ConsultationDbRepository::updateConsultationState(int consultationId, bool isChecked)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();

    db->transaction();
    try {
        QSqlQuery query(*db);

        query.prepare("UPDATE consultations SET flag=:flag WHERE idconsultations=:id");
        query.bindValue(":flag", isChecked);
        query.bindValue(":id", consultationId);

        return query.exec() && db->commit();
    } catch (...) {
        db->rollback();
    }

    return false;
}

const QList<std::shared_ptr<Consultation>> ConsultationDbRepository::getConsultations(
    const QString &assistantLogin, bool checkedOnly)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);
    query.prepare(QString("SELECT * FROM consultations WHERE examinerName=:examinerName %1")
                      .arg(checkedOnly ? "AND flag=true" : ""));
    query.bindValue(":examinerName", assistantLogin);

    QList<std::shared_ptr<Consultation>> result;
    if (query.exec()) {
        while (query.next()) {
            result.append(std::shared_ptr<Consultation>(new Consultation(query)));
        }
    }

    return result;
}

bool ConsultationDbRepository::addConsultation(const Consultation &consultation)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    db->transaction();
    try {
        query.prepare(
            "INSERT INTO consultations (login, `name`, surname, subject, examinerName, "
            "question, `date`, `time`, `group`, subgroup, gr) VALUES(:login, :name, :surname, "
            ":subject, :examinerName, :question, :date, :time, :group, :subgroup, concat(:group, "
            "'-', :subgroup))");
        query.bindValue(":login", consultation.getLogin());
        query.bindValue(":name", consultation.getName());
        query.bindValue(":surname", consultation.getSurName());
        query.bindValue(":subject", consultation.getSubject());
        query.bindValue(":examinerName", consultation.getExaminerName());
        query.bindValue(":question", consultation.getQuestion());
        query.bindValue(":date", consultation.getDate());
        query.bindValue(":time", consultation.getTime());
        query.bindValue(":group", consultation.getGroup());
        query.bindValue(":subgroup", consultation.getSubGroup());

        return query.exec() && db->commit();
    } catch (...) {
        db->rollback();
    }

    return false;
}
