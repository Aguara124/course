#include "assignment.h"
#include <QSqlQuery>

int Assignment::getId() const
{
    return id;
}

void Assignment::setId(int newId)
{
    id = newId;
}

int Assignment::getGroup() const
{
    return group;
}

void Assignment::setGroup(int newGroup)
{
    group = newGroup;
}

int Assignment::getSubGroup() const
{
    return subGroup;
}

void Assignment::setSubGroup(int newSubGroup)
{
    subGroup = newSubGroup;
}

QDateTime Assignment::getDeadLine() const
{
    return deadLine;
}

void Assignment::setDeadLine(const QDateTime &newDeadLine)
{
    deadLine = newDeadLine;
}

QByteArray Assignment::getAssign() const
{
    return assign;
}

void Assignment::setAssign(const QByteArray &newAssign)
{
    assign = newAssign;
}

QString Assignment::getSubject() const
{
    return subject;
}

void Assignment::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

QString Assignment::getControl() const
{
    return control;
}

void Assignment::setControl(const QString &newControl)
{
    control = newControl;
}

QString Assignment::getComment() const
{
    return comment;
}

void Assignment::setComment(const QString &newComment)
{
    comment = newComment;
}

QByteArray Assignment::getGradingTable() const
{
    return gradingTable;
}

void Assignment::setGradingTable(const QByteArray &newGradingTable)
{
    gradingTable = newGradingTable;
}

QString Assignment::getExaminerName() const
{
    return examinerName;
}

void Assignment::setExaminerName(const QString &newExaminerName)
{
    examinerName = newExaminerName;
}

QString Assignment::getName() const
{
    return name;
}

void Assignment::setName(const QString &newName)
{
    name = newName;
}

bool Assignment::getIsPdf() const
{
    return isPdf;
}

void Assignment::setIsPdf(bool newIsPdf)
{
    isPdf = newIsPdf;
}

Assignment::Assignment(const QSqlQuery &query)
{
    id = query.value("idassignments").toInt();
    group = query.value("group").toInt();
    subGroup = query.value("subgroup").toInt();
    deadLine = query.value("deadline").toDateTime();
    assign = query.value("assign").toByteArray();
    subject = query.value("subject").toString();
    control = query.value("control").toString();
    comment = query.value("comment").toString();
    gradingTable = query.value("gradingTable").toByteArray();
    examinerName = query.value("examinerName").toString();
    name = query.value("name").toString();
    isPdf = query.value("flag_pdf").toBool();
}

Assignment::Assignment(const Assignment &other)
{
    id = other.getId();
    group = other.getGroup();
    subGroup = other.getSubGroup();
    deadLine = other.getDeadLine();
    assign = other.getAssign();
    subject = other.getSubject();
    control = other.getControl();
    comment = other.getComment();
    gradingTable = other.getGradingTable();
    examinerName = other.getExaminerName();
    name = other.getName();
    isPdf = other.getIsPdf();
}

Assignment &Assignment::operator=(const Assignment &other)
{
    if (this != &other) // not a self-assignment
    {
        id = other.getId();
        group = other.getGroup();
        subGroup = other.getSubGroup();
        deadLine = other.getDeadLine();
        assign = other.getAssign();
        subject = other.getSubject();
        control = other.getControl();
        comment = other.getComment();
        gradingTable = other.getGradingTable();
        examinerName = other.getExaminerName();
        name = other.getName();
        isPdf = other.getIsPdf();
    }

    return *this;
}
