#ifndef CONSULTATIONCONTROLLER_H
#define CONSULTATIONCONTROLLER_H

#include <QHttpServer>
#include "consultationrepository.h"
#include "controller.h"

class ConsultationController : public Controller
{
public:
    ConsultationController();

    QHttpServerResponse getById(const QHttpServerRequest *request);
    QHttpServerResponse updateConsultationState(const QHttpServerRequest *request);
    QHttpServerResponse getConsultations(const QHttpServerRequest *request);
    QHttpServerResponse addConsultation(const QHttpServerRequest *request);

public:
    QHttpServerResponse handleRequest(const QString &controller,
                                      const QString &method,
                                      const QHttpServerRequest *request,
                                      QLinkedList<Controller *> &chain) override;

    std::unique_ptr<IConsultationRepository> consultationRepository;
};

#endif // CONSULTATIONCONTROLLER_H
