#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QSqlQuery>
#include <QString>

class Assistant
{
private:
    int id;
    QString login;
    QString password;
    QString program;
    int group;
    int subGroup;
    QString teacher;
    QString subject;
    QByteArray avatar;
    QString name;
    QString surname;

public:
    Assistant() {}
    Assistant(const QSqlQuery &query);
    Assistant(const Assistant &other);

    Assistant &operator=(const Assistant &other);
    bool operator==(const Assistant &rhs) { return this->equals(rhs); }

    int getId() const;
    void setId(int newId);
    QString getLogin() const;
    void setLogin(const QString &newLogin);
    QString getPassword() const;
    void setPassword(const QString &newPassword);
    QString getProgram() const;
    void setProgram(const QString &newProgram);
    int getGroup() const;
    void setGroup(int newGroup);
    int getSubGroup() const;
    void setSubGroup(int newSubGroup);
    QString getTeacher() const;
    void setTeacher(const QString &newTeacher);
    QString getSubject() const;
    void setSubject(const QString &newSubject);
    QByteArray getAvatar() const;
    void setAvatar(const QByteArray &newAvatar);

    QString getName() const;
    void setName(const QString &newName);
    QString getSurname() const;
    void setSurname(const QString &newSurname);

    bool equals(const Assistant &source)
    {
        if (this == &source) {
            return true;
        }

        return id == source.getId() && login == source.getLogin() && program == source.getProgram()
               && group == source.getGroup() && subGroup == source.getSubGroup()
               && teacher == source.getTeacher() && subject == source.getSubject()
               && name == source.getName() && surname == source.getSurname();
    }
};

#endif // ASSISTANT_H
