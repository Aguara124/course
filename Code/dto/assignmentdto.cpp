#include "assignmentdto.h"
#include <QJsonDocument>
#include <QJsonObject>

int AssignmentDto::getId() const
{
    return id;
}

void AssignmentDto::setId(int newId)
{
    id = newId;
}

int AssignmentDto::getGroup() const
{
    return group;
}

void AssignmentDto::setGroup(int newGroup)
{
    group = newGroup;
}

int AssignmentDto::getSubGroup() const
{
    return subGroup;
}

void AssignmentDto::setSubGroup(int newSubGroup)
{
    subGroup = newSubGroup;
}

QDateTime AssignmentDto::getDeadLine() const
{
    return deadLine;
}

void AssignmentDto::setDeadLine(const QDateTime &newDeadLine)
{
    deadLine = newDeadLine;
}

QByteArray AssignmentDto::getAssign() const
{
    return assign;
}

void AssignmentDto::setAssign(const QByteArray &newAssign)
{
    assign = newAssign;
}

QString AssignmentDto::getSubject() const
{
    return subject;
}

void AssignmentDto::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

QString AssignmentDto::getControl() const
{
    return control;
}

void AssignmentDto::setControl(const QString &newControl)
{
    control = newControl;
}

QString AssignmentDto::getComment() const
{
    return comment;
}

void AssignmentDto::setComment(const QString &newComment)
{
    comment = newComment;
}

QByteArray AssignmentDto::getGradingTable() const
{
    return gradingTable;
}

void AssignmentDto::setGradingTable(const QByteArray &newGradingTable)
{
    gradingTable = newGradingTable;
}

QString AssignmentDto::getExaminerName() const
{
    return examinerName;
}

void AssignmentDto::setExaminerName(const QString &newExaminerName)
{
    examinerName = newExaminerName;
}

QString AssignmentDto::getName() const
{
    return name;
}

void AssignmentDto::setName(const QString &newName)
{
    name = newName;
}

bool AssignmentDto::getIsPdf() const
{
    return isPdf;
}

void AssignmentDto::setIsPdf(bool newIsPdf)
{
    isPdf = newIsPdf;
}

void AssignmentDto::initFromJsonObject(const QJsonObject &object)
{
    id = object.value("id").toInt();
    group = object.value("group").toInt();
    subGroup = object.value("subGroup").toInt();
    deadLine = QDateTime::fromString(object.value("deadLine").toString());
    assign = QByteArray::fromBase64(object.value("assign").toString().toUtf8());
    subject = object.value("subject").toString();
    control = object.value("control").toString();
    comment = object.value("comment").toString();
    gradingTable = QByteArray::fromBase64(object.value("gradingTable").toString().toUtf8());
    examinerName = object.value("examinerName").toString();
    name = object.value("name").toString();
    isPdf = object.value("isPdf").toBool();
}

AssignmentDto::AssignmentDto(const QJsonObject &object)
{
    initFromJsonObject(object);
}

void AssignmentDto::fromJson(const QByteArray &content)
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

const QByteArray AssignmentDto::toJson() const
{
    QJsonObject object;

    object["id"] = getId();
    object["group"] = getGroup();
    object["subGroup"] = getSubGroup();
    object["deadLine"] = getDeadLine().toString();
    object["assign"] = QString(getAssign().toBase64());
    object["subject"] = getSubject();
    object["control"] = getControl();
    object["comment"] = getComment();
    object["gradingTable"] = QString(getGradingTable().toBase64());
    object["examinerName"] = getExaminerName();
    object["name"] = getName();
    object["isPdf"] = getIsPdf();

    QJsonDocument jsonDoc(object);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}
