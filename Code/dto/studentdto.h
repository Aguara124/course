#ifndef STUDENTDTO_H
#define STUDENTDTO_H

#include <QJsonObject>
#include "dto.h"

class StudentDto : public IDto
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

    void initFromJsonObject(const QJsonObject &object);

public:
    StudentDto() {}
    StudentDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }
    StudentDto(const QJsonObject &object);

public:
    void fromJson(const QByteArray &content) override;
    const QByteArray toJson() const override;
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

    QByteArray getAvatar() const;
    void setAvatar(const QByteArray &newAvatar);
};

#endif // STUDENTDTO_H
