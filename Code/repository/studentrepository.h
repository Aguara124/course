#ifndef STUDENTREPOSITORY_H
#define STUDENTREPOSITORY_H

#include "student.h"

class IStudentRepository
{
public:
    virtual ~IStudentRepository() {}
    virtual const std::shared_ptr<Student> getByLogin(const QString &login) = 0;

    virtual const QList<std::shared_ptr<Student>> getByGroup(int group,
                                                             int subgroup,
                                                             const QString &assistantLogin)
        = 0;

    virtual bool addStudent(const Student &student) = 0;
    virtual bool updateStudent(const Student &student) = 0;
};

#endif // STUDENTREPOSITORY_H
