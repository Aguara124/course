#ifndef TEACHERNETWORKREPOSITORY_H
#define TEACHERNETWORKREPOSITORY_H

#include "teacherrepository.h"

class TeacherNetworkRepository : public ITeacherRepository
{
public:
    TeacherNetworkRepository();

public:
    const QList<std::shared_ptr<Teacher>> getByLogin(const QString &teacherLogin) override;
    bool updateTeacher(const Teacher &teacher) override;
};

#endif // TEACHERNETWORKREPOSITORY_H
