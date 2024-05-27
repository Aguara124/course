#ifndef GRADE_H
#define GRADE_H

#include <QDate>
#include <QDateTime>
#include <QSqlQuery>
#include <QString>

class Grade
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

public:
    Grade() {}

    Grade(const QSqlQuery &query);
    Grade(const Grade &student);

    Grade &operator=(const Grade &other);
    bool operator==(const Grade &rhs) { return this->equals(rhs); }

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

    bool equals(const Grade &source)
    {
        if (this == &source) {
            return true;
        }

        return id == source.getId() && nameOfStudent == source.getNameOfStudent()
               && subject == source.getSubject() && mark == source.getMark()
               && date == source.getDate() && gradeTime == source.getGradeTime()
               && comment == source.getComment() && elementOfControl == source.getElementOfControl()
               && module == source.getModule() && examinerLogin == source.getExaminerLogin()
               && name == source.getName() && surName == source.getSurName()
               && assignmentId == source.getAssignmentId()
               && assignmentName == source.getAssignmentName() && assigned == source.getAssigned();
    }
};

#endif // GRADE_H
