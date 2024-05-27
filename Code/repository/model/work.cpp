#include "work.h"
#include <QSqlQuery>

Work::Work(const QSqlQuery &query)
{
    id = query.value("idworks").toInt();
    login = query.value("login").toString();
    name = query.value("name").toString();
    surName = query.value("surname").toString();
    dateTime = query.value("dateTime").toDateTime();
    assign = query.value("assign").toByteArray();
    isPdf = query.value("flag_pdf").toBool();
    group = query.value("group").toInt();
    subGroup = query.value("subgroup").toInt();
    assignmentId = query.value("idassignments").toInt();
    comment = query.value("comment").toString();
    isLate = query.value("late").toBool();
    isChecked = query.value("checked").toBool();
}

Work::Work(const Work &work)
{
    id = work.getId();
    login = work.getLogin();
    name = work.getName();
    surName = work.getSurName();
    dateTime = work.getDateTime();
    assign = work.getAssign();
    isPdf = work.getIsPdf();
    group = work.getGroup();
    subGroup = work.getSubGroup();
    assignmentId = work.getAssignmentId();
    comment = work.getComment();
    isLate = work.getIsLate();
    isChecked = work.getIsChecked();
}

Work &Work::operator=(const Work &other)
{
    if (this != &other)
    {
        id = other.getId();
        login = other.getLogin();
        name = other.getName();
        surName = other.getSurName();
        dateTime = other.getDateTime();
        assign = other.getAssign();
        isPdf = other.getIsPdf();
        group = other.getGroup();
        subGroup = other.getSubGroup();
        assignmentId = other.getAssignmentId();
        comment = other.getComment();
        isLate = other.getIsLate();
        isChecked = other.getIsChecked();
    }

    return *this;
}

int Work::getId() const
{
    return id;
}

void Work::setId(int newId)
{
    id = newId;
}

QString Work::getLogin() const
{
    return login;
}

void Work::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString Work::getName() const
{
    return name;
}

void Work::setName(const QString &newName)
{
    name = newName;
}

QString Work::getSurName() const
{
    return surName;
}

void Work::setSurName(const QString &newSurName)
{
    surName = newSurName;
}

QDateTime Work::getDateTime() const
{
    return dateTime;
}

void Work::setDateTime(const QDateTime &newDateTime)
{
    dateTime = newDateTime;
}

QByteArray Work::getAssign() const
{
    return assign;
}

void Work::setAssign(const QByteArray &newAssign)
{
    assign = newAssign;
}

bool Work::getIsPdf() const
{
    return isPdf;
}

void Work::setIsPdf(bool newIsPdf)
{
    isPdf = newIsPdf;
}

int Work::getGroup() const
{
    return group;
}

void Work::setGroup(int newGroup)
{
    group = newGroup;
}

int Work::getSubGroup() const
{
    return subGroup;
}

void Work::setSubGroup(int newSubgroup)
{
    subGroup = newSubgroup;
}

int Work::getAssignmentId() const
{
    return assignmentId;
}

void Work::setAssignmentId(int newAssignmentId)
{
    assignmentId = newAssignmentId;
}

QString Work::getComment() const
{
    return comment;
}

void Work::setComment(const QString &newComment)
{
    comment = newComment;
}

bool Work::getIsLate() const
{
    return isLate;
}

void Work::setIsLate(bool newIsLate)
{
    isLate = newIsLate;
}

bool Work::getIsChecked() const
{
    return isChecked;
}

void Work::setIsChecked(bool newIsChecked)
{
    isChecked = newIsChecked;
}
