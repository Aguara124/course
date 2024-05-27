#ifndef TEACHERCONTROLLER_H
#define TEACHERCONTROLLER_H

#include "controller.h"
#include "repositorymanager.h"
#include "teacherrepository.h"

class TeacherController : public Controller
{
public:
    TeacherController();

public:
    QHttpServerResponse handleRequest(const QString &controller,
                                      const QString &method,
                                      const QHttpServerRequest *request,
                                      QLinkedList<Controller *> &chain) override;

private:
    QHttpServerResponse getByLogin(const QHttpServerRequest *request);
    QHttpServerResponse updateTeacher(const QHttpServerRequest *request);

    std::unique_ptr<ITeacherRepository> teacherRepository;
};

#endif // TEACHERCONTROLLER_H
