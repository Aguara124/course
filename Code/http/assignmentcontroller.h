#ifndef ASSIGNMENTCONTROLLER_H
#define ASSIGNMENTCONTROLLER_H

#include <QHttpServer>
#include "assignmentrepository.h"
#include "controller.h"

class AssignmentController : public Controller
{
public:
    AssignmentController();

public:
    QHttpServerResponse handleRequest(const QString &controller,
                                      const QString &method,
                                      const QHttpServerRequest *request,
                                      QLinkedList<Controller *> &chain) override;

private:
    QHttpServerResponse addAssignment(const QHttpServerRequest *request);
    QHttpServerResponse updateAssignment(const QHttpServerRequest *request);

    QHttpServerResponse getById(const QHttpServerRequest *request);
    QHttpServerResponse getByExaminerName(const QHttpServerRequest *request);
    QHttpServerResponse getByStudentLogin(const QHttpServerRequest *request);
    QHttpServerResponse getByGroup(const QHttpServerRequest *request);

    std::unique_ptr<IAssignmentRepository> assignmentRepository;
};

#endif // ASSIGNMENTCONTROLLER_H
