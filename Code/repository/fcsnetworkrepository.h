#ifndef FCSNETWORKREPOSITORY_H
#define FCSNETWORKREPOSITORY_H

#include "authdata.h"
#include "fcsrepository.h"

class FcsNetworkRepository : public IFcsRepository
{
private:
public:
    FcsNetworkRepository() {}

public:
    const QList<std::shared_ptr<Fcs> > getFcsByFilter(const QString &program,
                                                      bool pilot,
                                                      int course) override;

    const QList<std::shared_ptr<Fcs> > getFcsByLecturer(const QString &lecturer,
                                                        const QString &program,
                                                        const QString &subject) override;
    bool update(const Fcs &fcs) override;

    const std::shared_ptr<Fcs> getById(int id) override;

    const QList<std::shared_ptr<Fcs>> getFaculty() override;
    const QList<std::shared_ptr<Fcs>> getProgram(const QString &faculty) override;
};

#endif // FCSNETWORKREPOSITORY_H
