#ifndef STUDENTCONTROLLER_H
#define STUDENTCONTROLLER_H

#include "controller.h"
#include "repositorymanager.h"
#include "studentrepository.h"

class StudentController : public Controller
{
private:
    std::unique_ptr<IStudentRepository> studentRepository;

public:
    StudentController()
        : studentRepository(RepositoryManager::instance().studentRepository())
    {}

public:
    QHttpServerResponse handleRequest(const QString &controller,
                                      const QString &method,
                                      const QHttpServerRequest *request,
                                      QLinkedList<Controller *> &chain) override;

private:
    QHttpServerResponse getByLogin(const QHttpServerRequest *request);
    QHttpServerResponse addStudent(const QHttpServerRequest *request);
    QHttpServerResponse updateStudent(const QHttpServerRequest *request);
    QHttpServerResponse getByGroup(const QHttpServerRequest *request);
};

#endif // STUDENTCONTROLLER_H
