#ifndef WORKDTO_H
#define WORKDTO_H

#include <QDateTime>
#include "dto.h"

class WorkDto : public IDto
{
private:
    int id;
    QString login;
    QString name;
    QString surName;
    QDateTime dateTime;
    QByteArray assign;
    bool isPdf;
    int group;
    int subGroup;

    int assignmentId;
    QString comment;
    bool isLate;
    bool isChecked;

    void initFromJsonObject(const QJsonObject &object);

public:
    WorkDto() {}
    WorkDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }
    WorkDto(const QJsonObject &object);

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
    QDateTime getDateTime() const;
    void setDateTime(const QDateTime &newDateTime);
    QByteArray getAssign() const;
    void setAssign(const QByteArray &newAssign);
    bool getIsPdf() const;
    void setIsPdf(bool newIsPdf);
    int getGroup() const;
    void setGroup(int newGroup);
    int getSubGroup() const;
    void setSubGroup(int newSubGroup);
    int getAssignmentId() const;
    void setAssignmentId(int newAssigmentId);
    QString getComment() const;
    void setComment(const QString &newComment);
    bool getIsLate() const;
    void setIsLate(bool newIsLate);
    bool getIsChecked() const;
    void setIsChecked(bool newIsChecked);
};

#endif // WORKDTO_H
