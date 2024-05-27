#ifndef ASSISTANTCONTROLLER_H
#define ASSISTANTCONTROLLER_H

#include <QHttpServer>
#include "assistantrepository.h"
#include "controller.h"

class AssistantController : public Controller
{
public:
    AssistantController();

    QHttpServerResponse getByLogin(const QHttpServerRequest *request);
    QHttpServerResponse getByGroup(const QHttpServerRequest *request);
    QHttpServerResponse getByTeacher(const QHttpServerRequest *request);
    QHttpServerResponse updateAssistant(const QHttpServerRequest *request);

    std::unique_ptr<IAssistantRepository> assistantRepository;

public:
    QHttpServerResponse handleRequest(const QString &controller,
                                      const QString &method,
                                      const QHttpServerRequest *request,
                                      QLinkedList<Controller *> &chain) override;
};

#endif // ASSISTANTCONTROLLER_H
