#ifndef WORKNETWORKREPOSITORY_H
#define WORKNETWORKREPOSITORY_H

#include "workrepository.h"

class WorkNetworkRepository : public IWorkRepository
{
public:
    WorkNetworkRepository();

public:
    const std::shared_ptr<Work> getById(int id) override;
    bool updateWork(const Work &work) override;
    const QList<std::shared_ptr<Work> > getByAssignmentId(int assignmentId) override;
    bool addWork(const Work &work) override;
    const QList<std::shared_ptr<Work> > getByStudentLogin(const QString &studentLogin) override;
};

#endif // WORKNETWORKREPOSITORY_H
