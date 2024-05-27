#include "teacherdto.h"
#include <QJsonDocument>
#include <QJsonObject>

int TeacherDto::getId() const
{
    return id;
}

void TeacherDto::setId(int newId)
{
    id = newId;
}

QString TeacherDto::getLogin() const
{
    return login;
}

void TeacherDto::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString TeacherDto::getPassword() const
{
    return password;
}

void TeacherDto::setPassword(const QString &newPassword)
{
    password = newPassword;
}

QString TeacherDto::getProgram() const
{
    return program;
}

void TeacherDto::setProgram(const QString &newProgram)
{
    program = newProgram;
}

QString TeacherDto::getSubject() const
{
    return subject;
}

void TeacherDto::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

int TeacherDto::getGroup() const
{
    return group;
}

void TeacherDto::setGroup(int newGroup)
{
    group = newGroup;
}

int TeacherDto::getSubGroup() const
{
    return subGroup;
}

void TeacherDto::setSubGroup(int newSubGroup)
{
    subGroup = newSubGroup;
}

bool TeacherDto::getLecturer() const
{
    return lecturer;
}

void TeacherDto::setLecturer(bool newLecturer)
{
    lecturer = newLecturer;
}

QByteArray TeacherDto::getAvatar() const
{
    return avatar;
}

void TeacherDto::setAvatar(const QByteArray &newAvatar)
{
    avatar = newAvatar;
}

QString TeacherDto::getName() const
{
    return name;
}

void TeacherDto::setName(const QString &newName)
{
    name = newName;
}

QString TeacherDto::getSurname() const
{
    return surname;
}

void TeacherDto::setSurname(const QString &newSurname)
{
    surname = newSurname;
}

void TeacherDto::initFromJsonObject(const QJsonObject &object)
{
    id = object.value("id").toInt();
    login = object.value("login").toString();
    password = object.value("password").toString();
    program = object.value("program").toString();
    subject = object.value("subject").toString();
    group = object.value("group").toInt();
    subGroup = object.value("subGroup").toInt();
    lecturer = object.value("lecturer").toBool();
    avatar = QByteArray::fromBase64(object.value("avatar").toString().toUtf8());
    name = object.value("name").toString();
    surname = object.value("surname").toString();
}

TeacherDto::TeacherDto(const QJsonObject &object)
{
    initFromJsonObject(object);
}

void TeacherDto::fromJson(const QByteArray &content)
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

const QByteArray TeacherDto::toJson() const
{
    QJsonObject object;

    object["id"] = getId();
    object["login"] = getLogin();
    object["password"] = getPassword();
    object["program"] = getProgram();
    object["subject"] = getSubject();
    object["group"] = getGroup();
    object["subGroup"] = getSubGroup();
    object["lecturer"] = getLecturer();
    object["avatar"] = QString(getAvatar().toBase64());
    object["name"] = getName();
    object["surname"] = getSurname();

    QJsonDocument jsonDoc(object);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}
