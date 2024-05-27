#include "gradedto.h"
#include <QJsonDocument>
#include <QJsonObject>

int GradeDto::getId() const
{
    return id;
}

void GradeDto::setId(int newId)
{
    id = newId;
}

QString GradeDto::getNameOfStudent() const
{
    return nameOfStudent;
}

void GradeDto::setNameOfStudent(const QString &newNameOfStudent)
{
    nameOfStudent = newNameOfStudent;
}

QString GradeDto::getSubject() const
{
    return subject;
}

void GradeDto::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

double GradeDto::getMark() const
{
    return mark;
}

void GradeDto::setMark(double newMark)
{
    mark = newMark;
}

QDate GradeDto::getDate() const
{
    return date;
}

void GradeDto::setDate(const QDate &newDate)
{
    date = newDate;
}

QTime GradeDto::getGradeTime() const
{
    return gradeTime;
}

void GradeDto::setGradeTime(const QTime &newGradeTime)
{
    gradeTime = newGradeTime;
}

QString GradeDto::getComment() const
{
    return comment;
}

void GradeDto::setComment(const QString &newComment)
{
    comment = newComment;
}

QString GradeDto::getElementOfControl() const
{
    return elementOfControl;
}

void GradeDto::setElementOfControl(const QString &newElementOfControl)
{
    elementOfControl = newElementOfControl;
}

QString GradeDto::getModule() const
{
    return module;
}

void GradeDto::setModule(const QString &newModule)
{
    module = newModule;
}

QString GradeDto::getExaminerLogin() const
{
    return examinerLogin;
}

void GradeDto::setExaminerLogin(const QString &newExaminerLogin)
{
    examinerLogin = newExaminerLogin;
}

QString GradeDto::getName() const
{
    return name;
}

void GradeDto::setName(const QString &newName)
{
    name = newName;
}

QString GradeDto::getSurName() const
{
    return surName;
}

void GradeDto::setSurName(const QString &newSurName)
{
    surName = newSurName;
}

GradeDto::GradeDto() {}

GradeDto::GradeDto(const QJsonObject &object)
{
    initFromJsonObject(object);
}

int GradeDto::getAssignmentId() const
{
    return assignmentId;
}

void GradeDto::setAssignmentId(int newAssigmentId)
{
    assignmentId = newAssigmentId;
}

QString GradeDto::getAssignmentName() const
{
    return assignmentName;
}

void GradeDto::setAssignmentName(const QString &newAssigmentName)
{
    assignmentName = newAssigmentName;
}

bool GradeDto::getAssigned() const
{
    return assigned;
}

void GradeDto::setAssigned(bool newAssigned)
{
    assigned = newAssigned;
}

void GradeDto::initFromJsonObject(const QJsonObject &object)
{
    id = object.value("id").toInt();
    nameOfStudent = object.value("nameOfStudent").toString();
    subject = object.value("subject").toString();
    mark = object.value("mark").toDouble();
    date = QDate::fromString(object.value("date").toString());
    gradeTime = QTime::fromString(object.value("gradeTime").toString());
    comment = object.value("comment").toString();
    elementOfControl = object.value("elementOfControl").toString();
    module = object.value("module").toString();
    examinerLogin = object.value("examinerLogin").toString();
    name = object.value("name").toString();
    surName = object.value("surName").toString();

    assignmentId = object.value("assignmentId").toInt();
    assignmentName = object.value("assignmentName").toString();
    assigned = object.value("assigned").toBool();
}

void GradeDto::fromJson(const QByteArray &content)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);

    if (jsonError.error != QJsonParseError::ParseError::NoError) {
        qDebug() << jsonError.errorString() << " error: " << jsonError.error;
        return;
    };

    QJsonObject obj = doc.object();
    initFromJsonObject(obj);
}

const QByteArray GradeDto::toJson() const
{
    QJsonObject object;

    object["id"] = getId();
    object["nameOfStudent"] = getNameOfStudent();
    object["subject"] = getSubject();
    object["mark"] = getMark();
    object["date"] = getDate().toString();
    object["gradeTime"] = getGradeTime().toString();
    object["comment"] = getComment();
    object["elementOfControl"] = getElementOfControl();
    object["module"] = getModule();;
    object["examinerLogin"] = getExaminerLogin();
    object["name"] = getName();
    object["surName"] = getSurName();

    object["assignmentId"] = getAssignmentId();
    object["assignmentName"] = getAssignmentName();
    object["assigned"] = getAssigned();

    QJsonDocument jsonDoc(object);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}
