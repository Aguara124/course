#ifndef GRADECONTROLLER_H
#define GRADECONTROLLER_H

#include "controller.h"
#include "graderepository.h"
#include "repositorymanager.h"

class GradeController : public Controller
{
private:
    std::unique_ptr<IGradeRepository> gradeRepository;

public:
    GradeController()
        : gradeRepository(RepositoryManager::instance().gradeRepository())
    {}

public:
    QHttpServerResponse handleRequest(const QString &controller,
                                      const QString &method,
                                      const QHttpServerRequest *request,
                                      QLinkedList<Controller *> &chain) override;

private:
    QHttpServerResponse getByFilter(const QHttpServerRequest *request);
    QHttpServerResponse addGrade(const QHttpServerRequest *request);
    QHttpServerResponse updateGrade(const QHttpServerRequest *request);
    QHttpServerResponse removeGrade(const QHttpServerRequest *request);
    QHttpServerResponse getByAssignmentId(const QHttpServerRequest *request);
};

#endif // GRADECONTROLLER_H
