#ifndef GRADENETWORKREPOSITORY_H
#define GRADENETWORKREPOSITORY_H

#include <QList>
#include "graderepository.h"
#include "authdata.h"

class GradeNetworkRepository : public IGradeRepository
{
private:
public:
    GradeNetworkRepository() {}

public:
    const QList<std::shared_ptr<Grade> > getByFilter(const QString &nameOfStudent,
                                                     const QString &name,
                                                     const QString &surName,
                                                     const QString &subject,
                                                     const QStringList modules,
                                                     const QString &elementOfControl,
                                                     const QString &examinerLogin) override;
    const QList<std::shared_ptr<Grade> > getByAssignmentId(int assignmentId) override;

public:
    bool addGrade(const Grade &grade) override;
    bool updateGrade(const Grade &grade) override;
    bool removeGrade(const Grade &grade) override;
};

#endif // GRADENETWORKREPOSITORY_H
