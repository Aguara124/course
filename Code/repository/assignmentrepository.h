#ifndef ASSIGNMENTREPOSITORY_H
#define ASSIGNMENTREPOSITORY_H

#include "assignment.h"

class IAssignmentRepository
{
public:
    virtual ~IAssignmentRepository() {}
    virtual const std::shared_ptr<Assignment> getById(int id) = 0;
    virtual const QList<std::shared_ptr<Assignment>> getByExaminerName(const QString &assistantLogin)
        = 0;
    virtual bool addAssignment(const Assignment &assignment) = 0;
    virtual bool updateAssignment(const Assignment &assignment) = 0;
    virtual const QList<std::shared_ptr<Assignment>> getByStudentLogin(const QString &studentLogin)
        = 0;
    virtual const QList<std::shared_ptr<Assignment>> getByGroup(int group, int subGroup) = 0;
};

#endif // ASSIGNMENTREPOSITORY_H
