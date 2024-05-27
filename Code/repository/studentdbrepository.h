#ifndef STUDENTDBREPOSITORY_H
#define STUDENTDBREPOSITORY_H

#include "studentrepository.h"

class StudentDbRepository : public IStudentRepository
{
public:
    StudentDbRepository();

public:
    const std::shared_ptr<Student> getByLogin(const QString &login) override;
    bool addStudent(const Student &student) override;
    const QList<std::shared_ptr<Student>> getByGroup(int group,
                                                     int subgroup,
                                                     const QString &assistantLogin) override;
    bool updateStudent(const Student &student) override;

private:
    const QString getAllowedLogin();
};

#endif // STUDENTDBREPOSITORY_H
