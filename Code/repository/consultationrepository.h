#ifndef CONSULTATIONREPOSITORY_H
#define CONSULTATIONREPOSITORY_H

#include <QList>
#include "consultation.h"

class IConsultationRepository
{
public:
    virtual ~IConsultationRepository() {}
    virtual const std::shared_ptr<Consultation> getById(int id) = 0;
    virtual bool updateConsultationState(int consulationId, bool isChecked) = 0;
    virtual const QList<std::shared_ptr<Consultation>>
    getConsultations(const QString &assistantLogin, bool checkedOnly = false) = 0;

    virtual bool addConsultation(const Consultation &consultation) = 0;
};

#endif // CONSULTATIONREPOSITORY_H
