#ifndef WORKDBREPOSITORY_H
#define WORKDBREPOSITORY_H

#include "workrepository.h"

class WorkDbRepository : public IWorkRepository
{
public:
    WorkDbRepository();

public:
    const std::shared_ptr<Work> getById(int id) override;
    bool updateWork(const Work &work) override;
    const QList<std::shared_ptr<Work> > getByAssignmentId(int assignmentId) override;
    bool addWork(const Work &work) override;
    const QList<std::shared_ptr<Work> > getByStudentLogin(const QString &studentLogin) override;

private:
    const QString currentLogin();
    bool allowedByRole();
};

#endif // WORKDBREPOSITORY_H
