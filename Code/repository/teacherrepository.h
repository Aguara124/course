#ifndef TEACHERREPOSITORY_H
#define TEACHERREPOSITORY_H

#include "teacher.h"

class ITeacherRepository
{
public:
    virtual ~ITeacherRepository() {}
    virtual const QList<std::shared_ptr<Teacher>> getByLogin(const QString &teacherLogin) = 0;
    virtual bool updateTeacher(const Teacher &teacher) = 0;
};

#endif // TEACHERREPOSITORY_H
