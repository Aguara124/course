#include "assignmentdbrepository.h"
#include "dbaccess.h"

AssignmentDbRepository::AssignmentDbRepository() {}

const std::shared_ptr<Assignment> AssignmentDbRepository::getById(int id)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM assignments WHERE idassignments=:id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return std::shared_ptr<Assignment>(new Assignment(query));
    }

    return nullptr;
}

bool AssignmentDbRepository::addAssignment(const Assignment &assignment)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    db->transaction();

    try {
        query.prepare(
            "INSERT INTO `assignments` (`idassignments`, `group`, `subgroup`, `deadline`,`assign`, `pdf_assign`,`subject`,`control`,`comment`,`gradingTable`,`examinerName`,`name`,`flag_pdf`) \
VALUES (null, :group, :subgroup, :deadline, :assign, null, :subject, :control, :comment, :gradingTable, :examinerName, :name, :flag_pdf)");
        query.bindValue(":group", assignment.getGroup());
        query.bindValue(":subgroup", assignment.getSubGroup());
        query.bindValue(":deadline", assignment.getDeadLine());
        query.bindValue(":assign", assignment.getAssign());
        query.bindValue(":subject", assignment.getSubject());
        query.bindValue(":control", assignment.getControl());
        query.bindValue(":comment", assignment.getComment());
        query.bindValue(":gradingTable", assignment.getGradingTable());
        query.bindValue(":examinerName", assignment.getExaminerName());
        query.bindValue(":name", assignment.getName());
        query.bindValue(":flag_pdf", assignment.getIsPdf());

        return query.exec() && db->commit();
    } catch (...) {
        qDebug() << db->lastError().text();
        db->rollback();
    }

    return true;
}

bool AssignmentDbRepository::updateAssignment(const Assignment &assignment)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    db->transaction();

    try {
        query.prepare("UPDATE `assignments` SET "
                      "`group`=:group, "
                      "`subgroup`=:subgroup, "
                      "`deadline`=:deadline,"
                      "`assign`=:assign, "
                      "`subject`=:subject,"
                      "`control`=:control,"
                      "`comment`=:comment,"
                      "`gradingTable`=:gradingTable,"
                      "`examinerName`=:examinerName,"
                      "`name`=:name,"
                      "`flag_pdf`=:flag_pdf"
                      " WHERE `idassignments`=:id");

        query.bindValue(":id", assignment.getId());

        query.bindValue(":group", assignment.getGroup());
        query.bindValue(":subgroup", assignment.getSubGroup());
        query.bindValue(":deadline", assignment.getDeadLine());
        query.bindValue(":assign", assignment.getAssign());
        query.bindValue(":subject", assignment.getSubject());
        query.bindValue(":control", assignment.getControl());
        query.bindValue(":comment", assignment.getComment());
        query.bindValue(":gradingTable", assignment.getGradingTable());
        query.bindValue(":examinerName", assignment.getExaminerName());
        query.bindValue(":name", assignment.getName());
        query.bindValue(":flag_pdf", assignment.getIsPdf());

        return query.exec() && db->commit();
    } catch (...) {
        qDebug() << db->lastError().text();
        db->rollback();
    }

    return true;
}

const QList<std::shared_ptr<Assignment>> AssignmentDbRepository::getByExaminerName(
    const QString &assistantLogin)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM assignments WHERE examinerName=:examinerName");
    query.bindValue(":examinerName", assistantLogin);

    QList<std::shared_ptr<Assignment>> result;
    if (query.exec()) {
        while (query.next()) {
            result.append(std::shared_ptr<Assignment>(new Assignment(query)));
        }
    }

    return result;
}

const QList<std::shared_ptr<Assignment>> AssignmentDbRepository::getByStudentLogin(
    const QString &studentLogin)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare(
        "SELECT DISTINCT idassignments, assignments.name, deadline FROM assignments INNER JOIN "
        "user ON user.group = assignments.group AND user.subgroup = assignments.subgroup"
        " WHERE user.login=:login");

    query.bindValue(":login", studentLogin);

    QList<std::shared_ptr<Assignment>> result;
    if (query.exec()) {
        while (query.next()) {
            result.append(std::shared_ptr<Assignment>(new Assignment(query)));
        }
    }

    return result;
}

const QList<std::shared_ptr<Assignment>> AssignmentDbRepository::getByGroup(int group, int subGroup)
{
    QSqlDatabase *db = DBAccess::instance().getDatabaseCopy();
    QSqlQuery query(*db);

    query.prepare("SELECT * FROM assignments WHERE `group`=:gr AND subgroup=:subgr");
    query.bindValue(":gr", group);
    query.bindValue(":subgr", subGroup);

    QList<std::shared_ptr<Assignment>> result;
    if (query.exec()) {
        while (query.next()) {
            result.append(std::shared_ptr<Assignment>(new Assignment(query)));
        }
    }

    return result;
}
