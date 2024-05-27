#include "assignmentcontroller.h"
#include "assignment.h"
#include "assignmentdto.h"
#include "dto2modelmapper.h"
#include "listdto.h"
#include "repositorymanager.h"
#include "responsedto.h"
#include "studentdto.h"

AssignmentController::AssignmentController()
    : assignmentRepository(RepositoryManager::instance().assignmentRepository())
{}

QHttpServerResponse AssignmentController::handleRequest(const QString &controller,
                                                        const QString &method,
                                                        const QHttpServerRequest *request,
                                                        QLinkedList<Controller *> &chain)
{
    if ("assignment" != QString(controller).toLower()) {
        return Controller::handleRequest(controller, method, request, chain);
    }

    if (!Controller::isJsonRequest(request)) {
        qDebug() << "strange request - expected json on " << request->url().path();
        return QHttpServerResponse("Expected Json POST request",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    QString lowMethod = QString(method).toLower();
    if ("getbyid" == lowMethod) {
        return getById(request);
    } else if ("addassignment" == lowMethod) {
        return addAssignment(request);
    } else if ("updateassignment" == lowMethod) {
        return updateAssignment(request);
    } else if ("getbyexaminername" == lowMethod) {
        return getByExaminerName(request);
    } else if ("getbystudentlogin" == lowMethod) {
        return getByStudentLogin(request);
    } else if ("getbygroup" == lowMethod) {
        return getByGroup(request);
    }

    qDebug() << "unsupported assignment method: " << method;
    return QHttpServerResponse("Method not valid", QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse AssignmentController::getById(const QHttpServerRequest *request)
{
    AssignmentDto assignmentDto(request->body());
    std::shared_ptr<Assignment> assignment = assignmentRepository->getById(assignmentDto.getId());

    if (assignment == nullptr) {
        return QHttpServerResponse("Assignment not found",
                                   QHttpServerResponse::StatusCode::NotFound);
    }

    assignmentDto = Dto2ModelMapper::instance().convertFromModel(*assignment);

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               assignmentDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse AssignmentController::getByExaminerName(const QHttpServerRequest *request)
{
    AssignmentDto assignmentDto(request->body());
    QList<std::shared_ptr<Assignment>> assignmentList = assignmentRepository->getByExaminerName(
        assignmentDto.getExaminerName());

    QList<AssignmentDto> resultList;
    for (std::shared_ptr<Assignment> assignment : assignmentList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*assignment));
    }

    ListDto<AssignmentDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse AssignmentController::getByStudentLogin(const QHttpServerRequest *request)
{
    StudentDto studentDto(request->body());
    QList<std::shared_ptr<Assignment>> assignmentList = assignmentRepository->getByStudentLogin(
        studentDto.getLogin());

    QList<AssignmentDto> resultList;
    for (std::shared_ptr<Assignment> assignment : assignmentList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*assignment));
    }

    ListDto<AssignmentDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse AssignmentController::getByGroup(const QHttpServerRequest *request)
{
    AssignmentDto assignmentDto(request->body());
    QList<std::shared_ptr<Assignment>> assignmentList
        = assignmentRepository->getByGroup(assignmentDto.getGroup(), assignmentDto.getSubGroup());

    QList<AssignmentDto> resultList;
    for (std::shared_ptr<Assignment> assignment : assignmentList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*assignment));
    }

    ListDto<AssignmentDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse AssignmentController::addAssignment(const QHttpServerRequest *request)
{
    AssignmentDto dto(request->body());
    std::shared_ptr<Assignment> assignment = std::shared_ptr<Assignment>(
        Dto2ModelMapper::instance().convertFromDto(dto));
    ResponseDto responseDto(assignmentRepository->addAssignment(*assignment));
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse AssignmentController::updateAssignment(const QHttpServerRequest *request)
{
    AssignmentDto dto(request->body());
    std::shared_ptr<Assignment> assignment = std::shared_ptr<Assignment>(
        Dto2ModelMapper::instance().convertFromDto(dto));
    ResponseDto responseDto(assignmentRepository->updateAssignment(*assignment));
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}
