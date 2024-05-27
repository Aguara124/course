#include "workcontroller.h"
#include "dto2modelmapper.h"
#include "listdto.h"
#include "repositorymanager.h"
#include "responsedto.h"
#include "work.h"
#include "workdto.h"
#include "workrepository.h"

WorkController::WorkController()
    : workRepository(RepositoryManager::instance().workRepository())
{}

QHttpServerResponse WorkController::handleRequest(const QString &controller,
                                                  const QString &method,
                                                  const QHttpServerRequest *request,
                                                  QLinkedList<Controller *> &chain)
{
    if ("work" != QString(controller).toLower()) {
        return Controller::handleRequest(controller, method, request, chain);
    }

    if (!Controller::isJsonRequest(request)) {
        qDebug() << "strange request - expected json on " << request->url().path();
        return QHttpServerResponse("Expected Json POST request",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    if ("getbyid" == QString(method).toLower()) {
        return getById(request);
    } else if ("getbyassignmentid" == QString(method).toLower()) {
        return getByAssignmentId(request);
    } else if ("addwork" == QString(method).toLower()) {
        return addWork(request);
    } else if ("updatework" == QString(method).toLower()) {
        return updateWork(request);
    } else if ("getbystudentlogin" == QString(method).toLower()) {
        return getByStudentLogin(request);
    }

    qDebug() << "unsupported work method: " << method;
    return QHttpServerResponse("Method not valid", QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse WorkController::getById(const QHttpServerRequest *request)
{
    WorkDto workDto(request->body());
    std::shared_ptr<Work> work = workRepository->getById(workDto.getId());

    if (work == nullptr) {
        return QHttpServerResponse("Work not found", QHttpServerResponse::StatusCode::NotFound);
    }

    workDto = Dto2ModelMapper::instance().convertFromModel(*work);

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               workDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse WorkController::updateWork(const QHttpServerRequest *request)
{
    WorkDto dto(request->body());
    ResponseDto responseDto(
        workRepository->updateWork(*Dto2ModelMapper::instance().convertFromDto(dto)));

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse WorkController::getByAssignmentId(const QHttpServerRequest *request)
{
    WorkDto workDto(request->body());
    QList<std::shared_ptr<Work>> workList = workRepository->getByAssignmentId(
        workDto.getAssignmentId());

    QList<WorkDto> resultList;
    for (std::shared_ptr<Work> work : workList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*work));
    }

    ListDto<WorkDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse WorkController::addWork(const QHttpServerRequest *request)
{
    WorkDto dto(request->body());
    ResponseDto responseDto(
        workRepository->addWork(*Dto2ModelMapper::instance().convertFromDto(dto)));

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse WorkController::getByStudentLogin(const QHttpServerRequest *request)
{
    WorkDto workDto(request->body());
    QList<std::shared_ptr<Work>> workList = workRepository->getByStudentLogin(workDto.getLogin());

    QList<WorkDto> resultList;
    for (std::shared_ptr<Work> work : workList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*work));
    }

    ListDto<WorkDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}
