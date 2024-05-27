#ifndef ASSIGNMENTDBREPOSITORY_H
#define ASSIGNMENTDBREPOSITORY_H

#include "assignmentrepository.h"

class AssignmentDbRepository : public IAssignmentRepository
{
public:
    AssignmentDbRepository();

public:
    const std::shared_ptr<Assignment> getById(int id) override;
    bool addAssignment(const Assignment &assignment) override;
    bool updateAssignment(const Assignment &assignment) override;
    const QList<std::shared_ptr<Assignment> > getByExaminerName(
        const QString &assistantLogin) override;
    const QList<std::shared_ptr<Assignment>> getByStudentLogin(const QString &studentLogin);
    const QList<std::shared_ptr<Assignment>> getByGroup(int group, int subGroup) override;
};

#endif // ASSIGNMENTDBREPOSITORY_H
