#include "studentdto.h"
#include <QJsonDocument>
#include <QJsonObject>

StudentDto::StudentDto(const QJsonObject &object)
{
    initFromJsonObject(object);
}

QByteArray StudentDto::getAvatar() const
{
    return avatar;
}

void StudentDto::setAvatar(const QByteArray &newAvatar)
{
    avatar = newAvatar;
}

void StudentDto::initFromJsonObject(const QJsonObject &object)
{
    id = object.value("id").toInt();
    program = object.value("program").toString();
    course = object.value("course").toInt();
    pilot = object.value("course").toBool();
    login = object.value("login").toString();
    password = object.value("password").toString();
    name = object.value("name").toString();
    surname = object.value("surname").toString();
    group = object.value("group").toInt();
    subgroup = object.value("subgroup").toInt();
    avatar = QByteArray::fromBase64(object.value("avatar").toString().toUtf8());
}

void StudentDto::fromJson(const QByteArray &content)
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

const QByteArray StudentDto::toJson() const
{
    QJsonObject obj;

    obj["id"] = getId();
    obj["program"] = getProgram();
    obj["course"] = getCourse();
    obj["pilot"] = getPilot();
    obj["login"] = getLogin();
    obj["password"] = getPassword();
    obj["name"] = getName();
    obj["surname"] = getSurname();
    obj["group"] = getGroup();
    obj["subgroup"] = getSubgroup();
    obj["avatar"] = QString(getAvatar().toBase64());

    QJsonDocument jsonDoc(obj);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}

int StudentDto::getId() const
{
    return id;
}

void StudentDto::setId(int newId)
{
    id = newId;
}

QString StudentDto::getLogin() const
{
    return login;
}

void StudentDto::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString StudentDto::getPassword() const
{
    return password;
}

void StudentDto::setPassword(const QString &newPassword)
{
    password = newPassword;
}

int StudentDto::getGroup() const
{
    return group;
}

void StudentDto::setGroup(int newGroup)
{
    group = newGroup;
}

int StudentDto::getSubgroup() const
{
    return subgroup;
}

void StudentDto::setSubgroup(int newSubgroup)
{
    subgroup = newSubgroup;
}

QString StudentDto::getProgram() const
{
    return program;
}

void StudentDto::setProgram(const QString &newProgram)
{
    program = newProgram;
}

bool StudentDto::getPilot() const
{
    return pilot;
}

void StudentDto::setPilot(bool newPilot)
{
    pilot = newPilot;
}

int StudentDto::getCourse() const
{
    return course;
}

void StudentDto::setCourse(int newCourse)
{
    course = newCourse;
}

QString StudentDto::getName() const
{
    return name;
}

void StudentDto::setName(const QString &newName)
{
    name = newName;
}

QString StudentDto::getSurname() const
{
    return surname;
}

void StudentDto::setSurname(const QString &newSurname)
{
    surname = newSurname;
}
