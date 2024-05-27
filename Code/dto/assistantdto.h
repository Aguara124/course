#ifndef ASSISTANTDTO_H
#define ASSISTANTDTO_H

#include <QJsonObject>
#include "dto.h"

class AssistantDto : public IDto
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

    void initFromJsonObject(const QJsonObject &object);

public:
    AssistantDto() {}
    AssistantDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }
    AssistantDto(const QJsonObject &object);

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
};

#endif // ASSISTANTDTO_H
