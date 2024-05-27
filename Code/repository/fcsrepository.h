#ifndef FCSREPOSITORY_H
#define FCSREPOSITORY_H

#include "fcs.h"

class IFcsRepository
{
public:
    virtual ~IFcsRepository() {}

    virtual const std::shared_ptr<Fcs> getById(int id) = 0;

    virtual const QList<std::shared_ptr<Fcs>> getFcsByFilter(const QString &program,
                                                             bool pilot,
                                                             int course)
        = 0;

    virtual const QList<std::shared_ptr<Fcs>> getFcsByLecturer(const QString &lecturer,
                                                               const QString &program,
                                                               const QString &subject)
        = 0;

    virtual bool update(const Fcs &fcs) = 0;

    virtual const QList<std::shared_ptr<Fcs>> getFaculty() = 0;
    virtual const QList<std::shared_ptr<Fcs>> getProgram(const QString &faculty) = 0;
};

#endif // FCSREPOSITORY_H
