#ifndef FCSDBREPOSITORY_H
#define FCSDBREPOSITORY_H

#include "fcsrepository.h"

class FcsDbRepository : public IFcsRepository
{
public:
    FcsDbRepository();

public:
    const QList<std::shared_ptr<Fcs>> getFcsByFilter(const QString &program,
                                                     bool pilot,
                                                     int course) override;

    const QList<std::shared_ptr<Fcs>> getFcsByLecturer(const QString &lecturer,
                                                       const QString &program,
                                                       const QString &subject) override;

    bool update(const Fcs &fcs) override;

    const std::shared_ptr<Fcs> getById(int id) override;

    const QList<std::shared_ptr<Fcs>> getFaculty() override;
    const QList<std::shared_ptr<Fcs>> getProgram(const QString &faculty) override;

private:
    static bool allowedByRole();
};

#endif // FCSDBREPOSITORY_H
