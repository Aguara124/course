#ifndef ASSIGNMENTNETWORKREPOSITORY_H
#define ASSIGNMENTNETWORKREPOSITORY_H

#include "assignment.h"
#include "assignmentdto.h"
#include "assignmentrepository.h"

class AssignmentNetworkRepository : public IAssignmentRepository
{
public:
    AssignmentNetworkRepository();

public:
    const std::shared_ptr<Assignment> getById(int id) override;
    bool addAssignment(const Assignment &assignment) override;
    bool updateAssignment(const Assignment &assignment) override;
    const QList<std::shared_ptr<Assignment> > getByExaminerName(
        const QString &assistantLogin) override;
    const QList<std::shared_ptr<Assignment> > getByStudentLogin(const QString &studentLogin) override;
    const QList<std::shared_ptr<Assignment> > getByGroup(int group, int subGroup) override;
};

#endif // ASSIGNMENTNETWORKREPOSITORY_H
