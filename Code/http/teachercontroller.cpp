#include "teachercontroller.h"
#include "dto2modelmapper.h"
#include "listdto.h"
#include "repositorymanager.h"
#include "responsedto.h"
#include "teacher.h"
#include "teacherdto.h"

TeacherController::TeacherController()
    : teacherRepository(RepositoryManager::instance().teacherRepository())
{}

QHttpServerResponse TeacherController::handleRequest(const QString &controller,
                                                     const QString &method,
                                                     const QHttpServerRequest *request,
                                                     QLinkedList<Controller *> &chain)
{
    if ("teacher" != QString(controller).toLower()) {
        return Controller::handleRequest(controller, method, request, chain);
    }

    if (!Controller::isJsonRequest(request)) {
        qDebug() << "strange request - expected json on " << request->url().path();
        return QHttpServerResponse("Expected Json POST request",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    if ("getbylogin" == QString(method).toLower()) {
        return getByLogin(request);
    } else if ("updateteacher" == QString(method).toLower()) {
        return updateTeacher(request);
    }

    qDebug() << "unsupported teacher method: " << method;
    return QHttpServerResponse("Method not valid", QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse TeacherController::getByLogin(const QHttpServerRequest *request)
{
    TeacherDto teacherDto(request->body());
    QList<std::shared_ptr<Teacher>> teacherList = teacherRepository->getByLogin(
        teacherDto.getLogin());

    if (teacherList.isEmpty()) {
        return QHttpServerResponse("Teacher not found", QHttpServerResponse::StatusCode::NotFound);
    }

    QList<TeacherDto> resultList;
    for (std::shared_ptr<Teacher> teacher : teacherList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*teacher));
    }

    ListDto<TeacherDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse TeacherController::updateTeacher(const QHttpServerRequest *request)
{
    TeacherDto dto(request->body());
    ResponseDto responseDto(
        teacherRepository->updateTeacher(*Dto2ModelMapper::instance().convertFromDto(dto)));

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}
