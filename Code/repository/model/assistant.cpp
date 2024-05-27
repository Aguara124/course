#include "assistant.h"
#include <QImage>
#include "imageutils.h"

QByteArray Assistant::getAvatar() const
{
    return avatar;
}

void Assistant::setAvatar(const QByteArray &newAvatar)
{
    avatar = newAvatar;
}

QString Assistant::getName() const
{
    return name;
}

void Assistant::setName(const QString &newName)
{
    name = newName;
}

QString Assistant::getSurname() const
{
    return surname;
}

void Assistant::setSurname(const QString &newSurname)
{
    surname = newSurname;
}

Assistant::Assistant(const QSqlQuery &query)
{
    id = query.value("idassistants").toInt();
    login = query.value("login").toString();
    password = query.value("password").toString();
    program = query.value("program").toString();
    group = query.value("group").toInt();
    subGroup = query.value("subgroup").toInt();
    teacher = query.value("teacher").toString();
    subject = query.value("subject").toString();
    if (query.value("avatar").isNull()) {
        avatar = ImageUtils::imageToByteArray(QImage(":/icons/icons/social/svg/production/ic_person_24px.svg"));
    } else {
        avatar = query.value("avatar").toByteArray();
    }

    name = query.value("name").toString();
    surname = query.value("surname").toString();
}

Assistant::Assistant(const Assistant &assistant)
{
    id = assistant.getId();
    login = assistant.getLogin();
    password = assistant.getPassword();
    program = assistant.getProgram();
    group = assistant.getGroup();
    subGroup = assistant.getSubGroup();
    teacher = assistant.getTeacher();
    subject = assistant.getSubject();
    avatar = assistant.getAvatar();
    name = assistant.getName();
    surname = assistant.getSurname();
}

Assistant &Assistant::operator=(const Assistant &other)
{
    if (this != &other) // not a self-assignment
    {
        id = other.getId();
        login = other.getLogin();
        password = other.getPassword();
        program = other.getProgram();
        group = other.getGroup();
        subGroup = other.getSubGroup();
        teacher = other.getTeacher();
        subject = other.getSubject();
        avatar = other.getAvatar();
        name = other.getName();
        surname = other.getSurname();
    }

    return *this;
}

int Assistant::getId() const
{
    return id;
}

void Assistant::setId(int newId)
{
    id = newId;
}

QString Assistant::getLogin() const
{
    return login;
}

void Assistant::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString Assistant::getPassword() const
{
    return password;
}

void Assistant::setPassword(const QString &newPassword)
{
    password = newPassword;
}

QString Assistant::getProgram() const
{
    return program;
}

void Assistant::setProgram(const QString &newProgram)
{
    program = newProgram;
}

int Assistant::getGroup() const
{
    return group;
}

void Assistant::setGroup(int newGroup)
{
    group = newGroup;
}

int Assistant::getSubGroup() const
{
    return subGroup;
}

void Assistant::setSubGroup(int newSubGroup)
{
    subGroup = newSubGroup;
}

QString Assistant::getTeacher() const
{
    return teacher;
}

void Assistant::setTeacher(const QString &newTeacher)
{
    teacher = newTeacher;
}

QString Assistant::getSubject() const
{
    return subject;
}

void Assistant::setSubject(const QString &newSubject)
{
    subject = newSubject;
}
