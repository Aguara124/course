#ifndef CONSULTATIONNETWORKREPOSITORY_H
#define CONSULTATIONNETWORKREPOSITORY_H

#include "consultationdto.h"
#include "consultationrepository.h"

class ConsultationNetworkRepository : public IConsultationRepository
{
public:
    ConsultationNetworkRepository();

public:
    const std::shared_ptr<Consultation> getById(int id) override;
    bool updateConsultationState(int consultationId, bool isChecked) override;
    const QList<std::shared_ptr<Consultation> > getConsultations(const QString &assistantLogin,
                                                                 bool checkedOnly) override;

    bool addConsultation(const Consultation &consultation) override;
};

#endif // CONSULTATIONNETWORKREPOSITORY_H
