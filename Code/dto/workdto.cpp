#include "workdto.h"
#include <QJsonDocument>
#include <QJsonObject>

int WorkDto::getId() const
{
    return id;
}

void WorkDto::setId(int newId)
{
    id = newId;
}

QString WorkDto::getLogin() const
{
    return login;
}

void WorkDto::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString WorkDto::getName() const
{
    return name;
}

void WorkDto::setName(const QString &newName)
{
    name = newName;
}

QString WorkDto::getSurName() const
{
    return surName;
}

void WorkDto::setSurName(const QString &newSurName)
{
    surName = newSurName;
}

QDateTime WorkDto::getDateTime() const
{
    return dateTime;
}

void WorkDto::setDateTime(const QDateTime &newDateTime)
{
    dateTime = newDateTime;
}

QByteArray WorkDto::getAssign() const
{
    return assign;
}

void WorkDto::setAssign(const QByteArray &newAssign)
{
    assign = newAssign;
}

bool WorkDto::getIsPdf() const
{
    return isPdf;
}

void WorkDto::setIsPdf(bool newIsPdf)
{
    isPdf = newIsPdf;
}

int WorkDto::getGroup() const
{
    return group;
}

void WorkDto::setGroup(int newGroup)
{
    group = newGroup;
}

int WorkDto::getSubGroup() const
{
    return subGroup;
}

void WorkDto::setSubGroup(int newSubGroup)
{
    subGroup = newSubGroup;
}

int WorkDto::getAssignmentId() const
{
    return assignmentId;
}

void WorkDto::setAssignmentId(int newAssignmentId)
{
    assignmentId = newAssignmentId;
}

QString WorkDto::getComment() const
{
    return comment;
}

void WorkDto::setComment(const QString &newComment)
{
    comment = newComment;
}

bool WorkDto::getIsLate() const
{
    return isLate;
}

void WorkDto::setIsLate(bool newIsLate)
{
    isLate = newIsLate;
}

bool WorkDto::getIsChecked() const
{
    return isChecked;
}

void WorkDto::setIsChecked(bool newIsChecked)
{
    isChecked = newIsChecked;
}

void WorkDto::initFromJsonObject(const QJsonObject &object)
{
    id = object.value("id").toInt();
    login = object.value("login").toString();
    name = object.value("name").toString();
    surName = object.value("surName").toString();
    dateTime = QDateTime::fromString(object.value("dateTime").toString());
    assign = QByteArray::fromBase64(object.value("assign").toString().toUtf8());
    isPdf = object.value("isPdf").toBool();
    group = object.value("group").toInt();
    subGroup = object.value("subGroup").toInt();
    assignmentId = object.value("assignmentId").toInt();
    isLate = object.value("isLate").toBool();
    isChecked = object.value("isChecked").toBool();
}

WorkDto::WorkDto(const QJsonObject &object)
{
    initFromJsonObject(object);
}

void WorkDto::fromJson(const QByteArray &content)
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

const QByteArray WorkDto::toJson() const
{
    QJsonObject object;

    object["id"] = getId();
    object["login"] = getLogin();
    object["name"] = getName();
    object["surName"] = getSurName();
    object["dateTime"] = getDateTime().toString();
    object["assign"] = QString(getAssign().toBase64());
    object["isPdf"] = getIsPdf();
    object["group"] = getGroup();
    object["subGroup"] = getSubGroup();
    object["assignmentId"] = getAssignmentId();
    object["isLate"] = getIsLate();
    object["isChecked"] = getIsChecked();

    QJsonDocument jsonDoc(object);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}
