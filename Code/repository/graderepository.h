#ifndef GRADEREPOSITORY_H
#define GRADEREPOSITORY_H

#include "grade.h"

class IGradeRepository
{
public:
    virtual ~IGradeRepository() {}

    virtual const QList<std::shared_ptr<Grade>> getByFilter(
        const QString &nameOfStudent = QString(),
        const QString &name = QString(),
        const QString &surName = QString(),
        const QString &subject = QString(),
        const QStringList modules = QStringList(),
        const QString &elementOfControl = QString(),
        const QString &examinerLogin = QString())
        = 0;

    const QList<std::shared_ptr<Grade>> getByStudent(const QString &nameOfStudent,
                                                     const QString &examinerLogin = QString())
    {
        return getByFilter(nameOfStudent,
                           QString(),
                           QString(),
                           QString(),
                           QStringList(),
                           QString(),
                           examinerLogin);
    }

    const QList<std::shared_ptr<Grade>> getByExaminer(const QString &examinerLogin)
    {
        return getByFilter(QString(),
                           QString(),
                           QString(),
                           QString(),
                           QStringList(),
                           QString(),
                           examinerLogin);
    }

    virtual bool addGrade(const Grade &grade) = 0;
    virtual bool updateGrade(const Grade &grade) = 0;
    virtual bool removeGrade(const Grade &grade) = 0;

    virtual const QList<std::shared_ptr<Grade>> getByAssignmentId(int assignmentId) = 0;
};

#endif // GRADEREPOSITORY_H
