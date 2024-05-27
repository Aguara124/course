#ifndef STUDENTNETWORKREPOSITORY_H
#define STUDENTNETWORKREPOSITORY_H

#include <QSharedDataPointer>
#include "authdata.h"
#include "studentrepository.h"

class StudentNetworkRepository : public IStudentRepository
{
private:

public:
    StudentNetworkRepository();

public:
    const std::shared_ptr<Student> getByLogin(const QString &login) override;
    bool addStudent(const Student &student) override;
    const QList<std::shared_ptr<Student>> getByGroup(int group,
                                                     int subgroup,
                                                     const QString &assistantLogin) override;

    bool updateStudent(const Student &student) override;
};

#endif // STUDENTNETWORKREPOSITORY_H
