#ifndef ASSIGNMENTDTO_H
#define ASSIGNMENTDTO_H

#include <QDateTime>
#include "dto.h"

class AssignmentDto : public IDto
{
private:
    int id;
    int group;
    int subGroup;
    QDateTime deadLine;
    QByteArray assign;
    QString subject;
    QString control;
    QString comment;
    QByteArray gradingTable;
    QString examinerName;
    QString name;
    bool isPdf;

    void initFromJsonObject(const QJsonObject &object);

public:
    AssignmentDto() {}
    AssignmentDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }
    AssignmentDto(const QJsonObject &object);

public:
    void fromJson(const QByteArray &content) override;
    const QByteArray toJson() const override;

    int getId() const;
    void setId(int newId);
    int getGroup() const;
    void setGroup(int newGroup);
    int getSubGroup() const;
    void setSubGroup(int newSubGroup);
    QDateTime getDeadLine() const;
    void setDeadLine(const QDateTime &newDeadLine);
    QByteArray getAssign() const;
    void setAssign(const QByteArray &newAssign);
    QString getSubject() const;
    void setSubject(const QString &newSubject);
    QString getControl() const;
    void setControl(const QString &newControl);
    QString getComment() const;
    void setComment(const QString &newComment);
    QByteArray getGradingTable() const;
    void setGradingTable(const QByteArray &newGradingTable);
    QString getExaminerName() const;
    void setExaminerName(const QString &newExaminerName);
    QString getName() const;
    void setName(const QString &newName);
    bool getIsPdf() const;
    void setIsPdf(bool newIsPdf);
};

#endif // ASSIGNMENTDTO_H
