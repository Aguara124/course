#ifndef TEACHERDBREPOSITORY_H
#define TEACHERDBREPOSITORY_H

#include "teacherrepository.h"

class TeacherDbRepository : public ITeacherRepository
{
public:
    TeacherDbRepository();
    const QList<std::shared_ptr<Teacher>> getByLogin(const QString &teacherLogin) override;

private:
    const QString getAllowedLogin();

public:
    bool updateTeacher(const Teacher &teacher) override;
};

#endif // TEACHERDBREPOSITORY_H
