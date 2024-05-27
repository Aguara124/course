#include "assistantdto.h"
#include <QJsonDocument>

int AssistantDto::getId() const
{
    return id;
}

void AssistantDto::setId(int newId)
{
    id = newId;
}

QString AssistantDto::getLogin() const
{
    return login;
}

void AssistantDto::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString AssistantDto::getPassword() const
{
    return password;
}

void AssistantDto::setPassword(const QString &newPassword)
{
    password = newPassword;
}

QString AssistantDto::getProgram() const
{
    return program;
}

void AssistantDto::setProgram(const QString &newProgram)
{
    program = newProgram;
}

int AssistantDto::getGroup() const
{
    return group;
}

void AssistantDto::setGroup(int newGroup)
{
    group = newGroup;
}

int AssistantDto::getSubGroup() const
{
    return subGroup;
}

void AssistantDto::setSubGroup(int newSubGroup)
{
    subGroup = newSubGroup;
}

QString AssistantDto::getTeacher() const
{
    return teacher;
}

void AssistantDto::setTeacher(const QString &newTeacher)
{
    teacher = newTeacher;
}

QString AssistantDto::getSubject() const
{
    return subject;
}

void AssistantDto::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

QByteArray AssistantDto::getAvatar() const
{
    return avatar;
}

void AssistantDto::setAvatar(const QByteArray &newAvatar)
{
    avatar = newAvatar;
}

QString AssistantDto::getName() const
{
    return name;
}

void AssistantDto::setName(const QString &newName)
{
    name = newName;
}

QString AssistantDto::getSurname() const
{
    return surname;
}

void AssistantDto::setSurname(const QString &newSurname)
{
    surname = newSurname;
}

void AssistantDto::initFromJsonObject(const QJsonObject &object)
{
    id = object.value("id").toInt();
    login = object.value("login").toString();
    password = object.value("password").toString();
    program = object.value("program").toString();
    group = object.value("group").toInt();
    subGroup = object.value("subGroup").toInt();
    teacher = object.value("teacher").toString();
    subject = object.value("subject").toString();
    avatar = QByteArray::fromBase64(object.value("avatar").toString().toUtf8());
    name = object.value("name").toString();
    surname = object.value("surname").toString();
}

AssistantDto::AssistantDto(const QJsonObject &object)
{
    initFromJsonObject(object);
}

void AssistantDto::fromJson(const QByteArray &content)
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

const QByteArray AssistantDto::toJson() const
{
    QJsonObject object;

    object["id"] = getId();
    object["login"] = getLogin();
    object["password"] = getPassword();
    object["program"] = getProgram();
    object["group"] = getGroup();
    object["subGroup"] = getSubGroup();
    object["teacher"] = getTeacher();
    object["subject"] = getSubject();
    object["avatar"] = QString(getAvatar().toBase64());
    object["name"] = getName();
    object["surname"] = getSurname();

    QJsonDocument jsonDoc(object);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}
