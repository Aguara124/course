#include "grade.h"

int Grade::getId() const
{
    return id;
}

void Grade::setId(int newId)
{
    id = newId;
}

QString Grade::getNameOfStudent() const
{
    return nameOfStudent;
}

void Grade::setNameOfStudent(const QString &newNameOfStudent)
{
    nameOfStudent = newNameOfStudent;
}

QString Grade::getSubject() const
{
    return subject;
}

void Grade::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

double Grade::getMark() const
{
    return mark;
}

void Grade::setMark(double newMark)
{
    mark = newMark;
}

QDate Grade::getDate() const
{
    return date;
}

void Grade::setDate(const QDate &newDate)
{
    date = newDate;
}

QTime Grade::getGradeTime() const
{
    return gradeTime;
}

void Grade::setGradeTime(const QTime &newGradeTime)
{
    gradeTime = newGradeTime;
}

QString Grade::getComment() const
{
    return comment;
}

void Grade::setComment(const QString &newComment)
{
    comment = newComment;
}

QString Grade::getElementOfControl() const
{
    return elementOfControl;
}

void Grade::setElementOfControl(const QString &newElementOfControl)
{
    elementOfControl = newElementOfControl;
}

QString Grade::getModule() const
{
    return module;
}

void Grade::setModule(const QString &newModule)
{
    module = newModule;
}

QString Grade::getExaminerLogin() const
{
    return examinerLogin;
}

void Grade::setExaminerLogin(const QString &newExaminerLogin)
{
    examinerLogin = newExaminerLogin;
}

QString Grade::getName() const
{
    return name;
}

void Grade::setName(const QString &newName)
{
    name = newName;
}

QString Grade::getSurName() const
{
    return surName;
}

void Grade::setSurName(const QString &newSurName)
{
    surName = newSurName;
}

int Grade::getAssignmentId() const
{
    return assignmentId;
}

void Grade::setAssignmentId(int newAssigmentId)
{
    assignmentId = newAssigmentId;
}

QString Grade::getAssignmentName() const
{
    return assignmentName;
}

void Grade::setAssignmentName(const QString &newAssigmentName)
{
    assignmentName = newAssigmentName;
}

bool Grade::getAssigned() const
{
    return assigned;
}

void Grade::setAssigned(bool newAssigned)
{
    assigned = newAssigned;
}

Grade::Grade(const QSqlQuery &query)
{
    id = query.value("idgrades").toInt();
    nameOfStudent = query.value("nameOfStudent").toString();
    subject = query.value("subject").toString();
    mark = query.value("mark").toDouble();
    date = query.value("date").toDate();
    gradeTime = query.value("gradeTime").toTime();
    comment = query.value("comment").toString();
    elementOfControl = query.value("elementOfControl").toString();
    module = query.value("module").toString();
    examinerLogin = query.value("examinerLogin").toString();
    name = query.value("name").toString();
    surName = query.value("surname").toString();

    assignmentId = query.value("idassignments").toInt();
    assignmentName = query.value("nameAssignment").toString();
    assigned = query.value("flag").toBool();
}

Grade::Grade(const Grade &source)
{
    id = source.getId();
    nameOfStudent = source.getNameOfStudent();
    subject = source.getSubject();
    mark = source.getMark();
    date = source.getDate();
    gradeTime = source.getGradeTime();
    comment = source.getComment();
    elementOfControl = source.getElementOfControl();
    module = source.getModule();
    examinerLogin = source.getExaminerLogin();
    name = source.getName();
    surName = source.getSurName();

    assignmentId = source.getAssignmentId();
    assignmentName = source.getAssignmentName();
    assigned = source.getAssigned();
}

Grade &Grade::operator=(const Grade &source)
{
    if (this != &source)
    {
        id = source.getId();
        nameOfStudent = source.getNameOfStudent();
        subject = source.getSubject();
        mark = source.getMark();
        date = source.getDate();
        gradeTime = source.getGradeTime();
        comment = source.getComment();
        elementOfControl = source.getElementOfControl();
        module = source.getModule();
        examinerLogin = source.getExaminerLogin();
        name = source.getName();
        surName = source.getSurName();

        assignmentId = source.getAssignmentId();
        assignmentName = source.getAssignmentName();
        assigned = source.getAssigned();
    }

    return *this;
}
