#include "student.h"
#include <QDateTime>
#include <QImage>
#include <QSqlQuery>
#include <QTemporaryFile>
#include "utils/imageutils.h"
#include <string>

QString Student::getLogin() const
{
    return login;
}

void Student::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString Student::getPassword() const
{
    return password;
}

void Student::setPassword(const QString &newPassword)
{
    password = newPassword;
}

int Student::getGroup() const
{
    return group;
}

void Student::setGroup(int newGroup)
{
    group = newGroup;
}

int Student::getSubgroup() const
{
    return subgroup;
}

void Student::setSubgroup(int newSubgroup)
{
    subgroup = newSubgroup;
}

QString Student::getProgram() const
{
    return program;
}

void Student::setProgram(const QString &newProgram)
{
    program = newProgram;
}

bool Student::getPilot() const
{
    return pilot;
}

void Student::setPilot(bool newPilot)
{
    pilot = newPilot;
}

int Student::getCourse() const
{
    return course;
}

void Student::setCourse(int newCourse)
{
    course = newCourse;
}

QString Student::getName() const
{
    return name;
}

void Student::setName(const QString &newName)
{
    name = newName;
}

QString Student::getSurname() const
{
    return surname;
}

void Student::setSurname(const QString &newSurname)
{
    surname = newSurname;
}

int Student::getId() const
{
    return id;
}

void Student::setId(int newId)
{
    id = newId;
}

int Student::calcCourse(int group)
{
    int year = QDate::currentDate().year() - 2000;
    int month = QDate::currentDate().month();
    int was = std::stoi(std::to_string(group).substr(0, 2));
    int cours;
    if (month > 6)
        cours = year - was + 1;
    else
        cours = year - was;
    return cours;
}

QByteArray Student::getAvatar() const
{
    return avatar;
}

void Student::setAvatar(const QByteArray &newAvatar)
{
    avatar = newAvatar;
}

Student::Student(const QSqlQuery &query)
{
    id = query.value("iduser").toInt();
    login = query.value("login").toString();
    password = query.value("password").toString();

    group = query.value("group").toInt();
    subgroup = query.value("subgroup").toInt();
    program = query.value("program").toString();
    pilot = query.value("pilot").toBool();
    name = query.value("name").toString();
    surname = query.value("surname").toString();
    course = calcCourse(group);

    if (query.value("avatar").isNull()) {
        avatar = ImageUtils::imageToByteArray(QImage(":/icons/icons/png/avatar.png"));
    } else {
        avatar = query.value("avatar").toByteArray();
    }
}

Student::Student(const Student &source)
{
    id = source.getId();
    login = source.getLogin();
    password = source.getPassword();

    group = source.getGroup();
    subgroup = source.getSubgroup();
    program = source.getProgram();
    pilot = source.getPilot();
    name = source.getName();
    surname = source.getSurname();
    course = source.getCourse();

    avatar = source.getAvatar();
}

Student &Student::operator=(const Student &source)
{
    if (this != &source) // not a self-assignment
    {
        id = source.getId();
        login = source.getLogin();
        password = source.getPassword();

        group = source.getGroup();
        subgroup = source.getSubgroup();
        program = source.getProgram();
        pilot = source.getPilot();
        name = source.getName();
        surname = source.getSurname();
        course = source.getCourse();

        avatar = source.getAvatar();
    }

    return *this;
}
