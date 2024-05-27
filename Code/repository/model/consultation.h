#ifndef CONSULTATION_H
#define CONSULTATION_H

#include <QDate>
#include <QSqlQuery>
#include <QString>
#include <QTime>

class Consultation
{
private:
    int id;
    QString login;
    QString name;
    QString surName;
    QString subject;

    QString examinerName;
    QString question;
    QDate date;
    QTime time;
    bool checked;
    int group;
    int subGroup;

public:
    Consultation() {}
    Consultation(const QSqlQuery &query);
    Consultation(const Consultation &other);

    Consultation &operator=(const Consultation &other);

    int getId() const;
    void setId(int newId);
    QString getLogin() const;
    void setLogin(const QString &newLogin);
    QString getName() const;
    void setName(const QString &newName);
    QString getSurName() const;
    void setSurName(const QString &newSurName);
    QString getSubject() const;
    void setSubject(const QString &newSubject);
    QString getExaminerName() const;
    void setExaminerName(const QString &newExaminerName);
    QString getQuestion() const;
    void setQuestion(const QString &newQuestion);
    QDate getDate() const;
    void setDate(const QDate &newDate);
    QTime getTime() const;
    void setTime(const QTime &newTime);
    bool getChecked() const;
    void setChecked(bool newChecked);
    int getGroup() const;
    void setGroup(int newGroup);
    int getSubGroup() const;
    void setSubGroup(int newSubGroup);
};

#endif // CONSULTATION_H
