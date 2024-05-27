#ifndef TEACHERDTO_H
#define TEACHERDTO_H

#include <QJsonDocument>
#include <QString>
#include "dto.h"

class TeacherDto : public IDto
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

    void initFromJsonObject(const QJsonObject &object);

public:
    TeacherDto() {}
    TeacherDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }
    TeacherDto(const QJsonObject &object);

public:
    void fromJson(const QByteArray &content) override;
    const QByteArray toJson() const override;

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

#endif // TEACHERDTO_H
