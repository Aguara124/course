#ifndef FCSCONTROLLER_H
#define FCSCONTROLLER_H

#include "controller.h"
#include "fcsrepository.h"
#include "repositorymanager.h"

class FcsController : public Controller
{
private:
    std::unique_ptr<IFcsRepository> fcsRepository;

public:
    FcsController()
        : fcsRepository(RepositoryManager::instance().fcsRepository())
    {}

public:
    QHttpServerResponse handleRequest(const QString &controller,
                                      const QString &method,
                                      const QHttpServerRequest *request,
                                      QLinkedList<Controller *> &chain) override;

private:
    QHttpServerResponse getByFilter(const QHttpServerRequest *request);
    QHttpServerResponse getByLecturer(const QHttpServerRequest *request);
    QHttpServerResponse updateFcs(const QHttpServerRequest *request);
    QHttpServerResponse getById(const QHttpServerRequest *request);
    QHttpServerResponse getFaculty(const QHttpServerRequest *request);
    QHttpServerResponse getProgram(const QHttpServerRequest *request);
};

#endif // FCSCONTROLLER_H
