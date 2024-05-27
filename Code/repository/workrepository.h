#ifndef WORKREPOSITORY_H
#define WORKREPOSITORY_H

#include "work.h"

class IWorkRepository
{
public:
    virtual ~IWorkRepository() {}
    virtual const std::shared_ptr<Work> getById(int id) = 0;
    virtual const QList<std::shared_ptr<Work>> getByAssignmentId(int assignmentId) = 0;
    virtual const QList<std::shared_ptr<Work>> getByStudentLogin(const QString &studentLogin) = 0;

    virtual bool updateWork(const Work &work) = 0;
    virtual bool addWork(const Work &work) = 0;
};

#endif // WORKREPOSITORY_H
