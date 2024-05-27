#ifndef STUDENT_H
#define STUDENT_H

#include <QString>
class QSqlQuery;
class QImage;

class Student
{
private:
    int id;
    QString login;
    QString password;

    int group;
    int subgroup;
    QString program;
    bool pilot;
    int course;
    QString name;
    QString surname;
    QByteArray avatar;

public:
    Student() {}
    Student(const QSqlQuery &query);
    Student(const Student &student);

    Student &operator=(const Student &other);

    int getId() const;
    void setId(int newId);
    QString getLogin() const;
    void setLogin(const QString &newLogin);
    QString getPassword() const;
    void setPassword(const QString &newPassword);
    int getGroup() const;
    void setGroup(int newGroup);
    int getSubgroup() const;
    void setSubgroup(int newSubgroup);
    QString getProgram() const;
    void setProgram(const QString &newProgram);
    bool getPilot() const;
    void setPilot(bool newPilot);
    int getCourse() const;
    void setCourse(int newCourse);
    QString getName() const;
    void setName(const QString &newName);
    QString getSurname() const;
    void setSurname(const QString &newSurname);

public:
    static int calcCourse(int group);
    QByteArray getAvatar() const;
    void setAvatar(const QByteArray &newAvatar);
};

#endif // STUDENT_H
