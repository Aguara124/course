#ifndef GRADEDBREPOSITORY_H
#define GRADEDBREPOSITORY_H

#include <QList>
#include "graderepository.h"
#include "grade.h"

class GradeDbRepository : public IGradeRepository
{
public:
    GradeDbRepository() {}

public:
    const QList<std::shared_ptr<Grade> > getByFilter(const QString &nameOfStudent,
                                                     const QString &name,
                                                     const QString &surName,
                                                     const QString &subject,
                                                     const QStringList modules,
                                                     const QString &elementOfControl,
                                                     const QString &examinerLogin) override;
    bool addGrade(const Grade &grade) override;
    bool updateGrade(const Grade &grade) override;
    bool removeGrade(const Grade &grade) override;
    const QList<std::shared_ptr<Grade> > getByAssignmentId(int assignmentId) override;

private:
    static bool allowedByRole();
    static const QString currentLogin();
};

#endif // GRADEDBREPOSITORY_H
