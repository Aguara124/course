#ifndef TEACHER_H
#define TEACHER_H

#include <QSqlQuery>
#include <QString>

class Teacher
{
private:
    int id;
    QString login;
    QString password;
    QString program;
    QString subject;
    int group;
    int subGroup;
    bool lecturer;
    QByteArray avatar;
    QString name;
    QString surname;

public:
    Teacher() {}
    Teacher(const QSqlQuery &query);
    Teacher(const Teacher &teacher);

    Teacher &operator=(const Teacher &other);

    int getId() const;
    void setId(int newId);
    QString getLogin() const;
    void setLogin(const QString &newLogin);
    QString getPassword() const;
    void setPassword(const QString &newPassword);
    QString getProgram() const;
    void setProgram(const QString &newProgram);
    QString getSubject() const;
    void setSubject(const QString &newSubject);
    int getGroup() const;
    void setGroup(int newGroup);
    int getSubGroup() const;
    void setSubGroup(int newSubGroup);
    bool getLecturer() const;
    void setLecturer(bool newLecturer);

    QByteArray getAvatar() const;
    void setAvatar(const QByteArray &newAvatar);

    QString getName() const;
    void setName(const QString &newName);
    QString getSurname() const;
    void setSurname(const QString &newSurname);
};

#endif // TEACHER_H
