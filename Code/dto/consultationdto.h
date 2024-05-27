#ifndef CONSULTATIONDTO_H
#define CONSULTATIONDTO_H

#include <QDate>
#include <QTime>
#include "dto.h"

class ConsultationDto : public IDto
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

    void initFromJsonObject(const QJsonObject &object);

public:
    ConsultationDto() {}
    ConsultationDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }
    ConsultationDto(const QJsonObject &object);

public:
    void fromJson(const QByteArray &content) override;
    const QByteArray toJson() const override;

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

#endif // CONSULTATIONDTO_H
