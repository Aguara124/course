#ifndef WORKCONTROLLER_H
#define WORKCONTROLLER_H

#include "controller.h"
#include "repositorymanager.h"
#include "workrepository.h"

class WorkController : public Controller
{
public:
    WorkController();

public:
    QHttpServerResponse handleRequest(const QString &controller,
                                      const QString &method,
                                      const QHttpServerRequest *request,
                                      QLinkedList<Controller *> &chain) override;

private:
    QHttpServerResponse getById(const QHttpServerRequest *request);
    QHttpServerResponse updateWork(const QHttpServerRequest *request);
    QHttpServerResponse getByAssignmentId(const QHttpServerRequest *request);
    QHttpServerResponse addWork(const QHttpServerRequest *request);
    QHttpServerResponse getByStudentLogin(const QHttpServerRequest *request);

    std::unique_ptr<IWorkRepository> workRepository;
};

#endif // WORKCONTROLLER_H
