#ifndef GRADEDTO_H
#define GRADEDTO_H

#include <QJsonObject>
#include "dto.h"

class GradeDto : public IDto
{
private:
    int id;
    QString nameOfStudent;
    QString subject;
    double mark;
    QDate date;
    QTime gradeTime;
    QString comment;
    QString elementOfControl;
    QString module;
    QString examinerLogin;
    QString name;
    QString surName;

    int assignmentId;
    QString assignmentName;
    bool assigned;

    void initFromJsonObject(const QJsonObject &object);

public:
    GradeDto();
    GradeDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }
    GradeDto(const QJsonObject &object);

public:
    void fromJson(const QByteArray &content) override;
    const QByteArray toJson() const override;
    int getId() const;
    void setId(int newId);
    QString getNameOfStudent() const;
    void setNameOfStudent(const QString &newNameOfStudent);
    QString getSubject() const;
    void setSubject(const QString &newSubject);
    double getMark() const;
    void setMark(double newMark);
    QDate getDate() const;
    void setDate(const QDate &newDate);
    QTime getGradeTime() const;
    void setGradeTime(const QTime &newGradeTime);
    QString getComment() const;
    void setComment(const QString &newComment);
    QString getElementOfControl() const;
    void setElementOfControl(const QString &newElementOfControl);
    QString getModule() const;
    void setModule(const QString &newModule);
    QString getExaminerLogin() const;
    void setExaminerLogin(const QString &newExaminerLogin);
    QString getName() const;
    void setName(const QString &newName);
    QString getSurName() const;
    void setSurName(const QString &newSurName);
    int getAssignmentId() const;
    void setAssignmentId(int newAssigmentId);
    QString getAssignmentName() const;
    void setAssignmentName(const QString &newAssigmentName);
    bool getAssigned() const;
    void setAssigned(bool newAssigned);
};

#endif // GRADEDTO_H
