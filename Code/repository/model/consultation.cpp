#include "consultation.h"
#include <QSqlQuery>

int Consultation::getId() const
{
    return id;
}

void Consultation::setId(int newId)
{
    id = newId;
}

QString Consultation::getLogin() const
{
    return login;
}

void Consultation::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString Consultation::getName() const
{
    return name;
}

void Consultation::setName(const QString &newName)
{
    name = newName;
}

QString Consultation::getSurName() const
{
    return surName;
}

void Consultation::setSurName(const QString &newSurName)
{
    surName = newSurName;
}

QString Consultation::getSubject() const
{
    return subject;
}

void Consultation::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

QString Consultation::getExaminerName() const
{
    return examinerName;
}

void Consultation::setExaminerName(const QString &newExaminerName)
{
    examinerName = newExaminerName;
}

QString Consultation::getQuestion() const
{
    return question;
}

void Consultation::setQuestion(const QString &newQuestion)
{
    question = newQuestion;
}

QDate Consultation::getDate() const
{
    return date;
}

void Consultation::setDate(const QDate &newDate)
{
    date = newDate;
}

QTime Consultation::getTime() const
{
    return time;
}

void Consultation::setTime(const QTime &newTime)
{
    time = newTime;
}

bool Consultation::getChecked() const
{
    return checked;
}

void Consultation::setChecked(bool newChecked)
{
    checked = newChecked;
}

int Consultation::getGroup() const
{
    return group;
}

void Consultation::setGroup(int newGroup)
{
    group = newGroup;
}

int Consultation::getSubGroup() const
{
    return subGroup;
}

void Consultation::setSubGroup(int newSubGroup)
{
    subGroup = newSubGroup;
}

Consultation::Consultation(const QSqlQuery &query)
{
    id = query.value("idconsultations").toInt();
    group = query.value("group").toInt();
    subGroup = query.value("subgroup").toInt();

    login = query.value("login").toString();
    name = query.value("name").toString();
    surName = query.value("surname").toString();
    subject = query.value("subject").toString();
    examinerName = query.value("examinerName").toString();
    question = query.value("question").toString();
    date = query.value("date").toDate();
    time = query.value("time").toTime();
    checked = query.value("flag").toBool();
}

Consultation::Consultation(const Consultation &other)
{
    id = other.getId();
    group = other.getGroup();
    subGroup = other.getSubGroup();

    login = other.getLogin();
    name = other.getName();
    surName = other.getSurName();
    subject = other.getSubject();
    examinerName = other.getExaminerName();
    question = other.getQuestion();
    date = other.getDate();
    time = other.getTime();
    checked = other.getChecked();
}

Consultation &Consultation::operator=(const Consultation &other)
{
    if (this != &other)
    {
        id = other.getId();
        group = other.getGroup();
        subGroup = other.getSubGroup();

        login = other.getLogin();
        name = other.getName();
        surName = other.getSurName();
        subject = other.getSubject();
        examinerName = other.getExaminerName();
        question = other.getQuestion();
        date = other.getDate();
        time = other.getTime();
        checked = other.getChecked();
    }

    return *this;
}
