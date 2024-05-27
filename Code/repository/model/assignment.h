#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <QDateTime>
#include <QSqlQuery>

class Assignment
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

public:
    Assignment() {}
    Assignment(const QSqlQuery &query);
    Assignment(const Assignment &other);

    Assignment &operator=(const Assignment &other);

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

#endif // ASSIGNMENT_H
