#ifndef CONSULTATIONDBREPOSITORY_H
#define CONSULTATIONDBREPOSITORY_H

#include "consultationrepository.h"

class ConsultationDbRepository : public IConsultationRepository
{
public:
    ConsultationDbRepository() {}

public:
    const std::shared_ptr<Consultation> getById(int id) override;
    bool updateConsultationState(int consultationId, bool isChecked) override;
    const QList<std::shared_ptr<Consultation> > getConsultations(const QString &assistantLogin,
                                                                 bool checkedOnly) override;
    bool addConsultation(const Consultation &consultation) override;
};

#endif // CONSULTATIONDBREPOSITORY_H
