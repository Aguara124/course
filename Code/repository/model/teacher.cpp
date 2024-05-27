#include "teacher.h"
#include <QImage>
#include "imageutils.h"

int Teacher::getId() const
{
    return id;
}

void Teacher::setId(int newId)
{
    id = newId;
}

QString Teacher::getLogin() const
{
    return login;
}

void Teacher::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString Teacher::getPassword() const
{
    return password;
}

void Teacher::setPassword(const QString &newPassword)
{
    password = newPassword;
}

QString Teacher::getProgram() const
{
    return program;
}

void Teacher::setProgram(const QString &newProgram)
{
    program = newProgram;
}

QString Teacher::getSubject() const
{
    return subject;
}

void Teacher::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

int Teacher::getGroup() const
{
    return group;
}

void Teacher::setGroup(int newGroup)
{
    group = newGroup;
}

int Teacher::getSubGroup() const
{
    return subGroup;
}

void Teacher::setSubGroup(int newSubGroup)
{
    subGroup = newSubGroup;
}

bool Teacher::getLecturer() const
{
    return lecturer;
}

void Teacher::setLecturer(bool newLecturer)
{
    lecturer = newLecturer;
}

QByteArray Teacher::getAvatar() const
{
    return avatar;
}

void Teacher::setAvatar(const QByteArray &newAvatar)
{
    avatar = newAvatar;
}

QString Teacher::getName() const
{
    return name;
}

void Teacher::setName(const QString &newName)
{
    name = newName;
}

QString Teacher::getSurname() const
{
    return surname;
}

void Teacher::setSurname(const QString &newSurname)
{
    surname = newSurname;
}

Teacher::Teacher(const QSqlQuery &query)
{
    id = query.value("iduser").toInt();
    login = query.value("login").toString();
    password = query.value("password").toString();
    program = query.value("programm").toString();
    subject = query.value("subject").toString();
    group = query.value("group").toInt();
    subGroup = query.value("subgroup").toInt();
    lecturer = query.value("lecturer").toBool();
    if (query.value("avatar").isNull()) {
        avatar = ImageUtils::imageToByteArray(QImage(":/icons/icons/png/avatar.png"));
    } else {
        avatar = query.value("avatar").toByteArray();
    }

    name = query.value("name").toString();
    surname = query.value("surname").toString();
}

Teacher::Teacher(const Teacher &other)
{
    id = other.getId();
    login = other.getLogin();
    password = other.getPassword();
    program = other.getProgram();
    subject = other.getSubject();
    group = other.getGroup();
    subGroup = other.getSubGroup();
    lecturer = other.getLecturer();
    avatar = other.getAvatar();
    name = other.getName();
    surname = other.getSurname();
}

Teacher &Teacher::operator=(const Teacher &other)
{
    if (this != &other)
    {
        id = other.getId();
        login = other.getLogin();
        password = other.getPassword();
        program = other.getProgram();
        subject = other.getSubject();
        group = other.getGroup();
        subGroup = other.getSubGroup();
        lecturer = other.getLecturer();
        avatar = other.getAvatar();
        name = other.getName();
        surname = other.getSurname();
    }

    return *this;
}
