#include "consultationdto.h"
#include <QJsonDocument>
#include <QJsonObject>

int ConsultationDto::getId() const
{
    return id;
}

void ConsultationDto::setId(int newId)
{
    id = newId;
}

QString ConsultationDto::getLogin() const
{
    return login;
}

void ConsultationDto::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString ConsultationDto::getName() const
{
    return name;
}

void ConsultationDto::setName(const QString &newName)
{
    name = newName;
}

QString ConsultationDto::getSurName() const
{
    return surName;
}

void ConsultationDto::setSurName(const QString &newSurName)
{
    surName = newSurName;
}

QString ConsultationDto::getSubject() const
{
    return subject;
}

void ConsultationDto::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

QString ConsultationDto::getExaminerName() const
{
    return examinerName;
}

void ConsultationDto::setExaminerName(const QString &newExaminerName)
{
    examinerName = newExaminerName;
}

QString ConsultationDto::getQuestion() const
{
    return question;
}

void ConsultationDto::setQuestion(const QString &newQuestion)
{
    question = newQuestion;
}

QDate ConsultationDto::getDate() const
{
    return date;
}

void ConsultationDto::setDate(const QDate &newDate)
{
    date = newDate;
}

QTime ConsultationDto::getTime() const
{
    return time;
}

void ConsultationDto::setTime(const QTime &newTime)
{
    time = newTime;
}

bool ConsultationDto::getChecked() const
{
    return checked;
}

void ConsultationDto::setChecked(bool newChecked)
{
    checked = newChecked;
}

int ConsultationDto::getGroup() const
{
    return group;
}

void ConsultationDto::setGroup(int newGroup)
{
    group = newGroup;
}

int ConsultationDto::getSubGroup() const
{
    return subGroup;
}

void ConsultationDto::setSubGroup(int newSubGroup)
{
    subGroup = newSubGroup;
}

void ConsultationDto::initFromJsonObject(const QJsonObject &object)
{
    id = object.value("id").toInt();
    login = object.value("login").toString();
    name = object.value("name").toString();
    surName = object.value("surName").toString();
    subject = object.value("subject").toString();
    examinerName = object.value("examinerName").toString();
    question = object.value("question").toString();
    date = QDate::fromString(object.value("date").toString());
    time = QTime::fromString(object.value("time").toString());
    checked = object.value("checked").toBool();
    group = object.value("group").toInt();
    subGroup = object.value("subGroup").toInt();
}

ConsultationDto::ConsultationDto(const QJsonObject &object)
{
    initFromJsonObject(object);
}

void ConsultationDto::fromJson(const QByteArray &content)
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

const QByteArray ConsultationDto::toJson() const
{
    QJsonObject object;

    object["id"] = getId();
    object["login"] = getLogin();
    object["name"] = getName();
    object["surName"] = getSurName();
    object["subject"] = getSubject();
    object["examinerName"] = getExaminerName();
    object["question"] = getQuestion();
    object["date"] = getDate().toString();
    object["time"] = getTime().toString();
    object["checked"] = getChecked();
    object["group"] = getGroup();
    object["subGroup"] = getSubGroup();

    QJsonDocument jsonDoc(object);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}
