#include "gradecontroller.h"
#include "dto2modelmapper.h"
#include "grade.h"
#include "gradedto.h"
#include "gradefilterrequestdto.h"
#include "listdto.h"
#include "responsedto.h"

QHttpServerResponse GradeController::handleRequest(const QString &controller,
                                                   const QString &method,
                                                   const QHttpServerRequest *request,
                                                   QLinkedList<Controller *> &chain)
{
    if ("grade" != QString(controller).toLower()) {
        return Controller::handleRequest(controller, method, request, chain);
    }

    if (!Controller::isJsonRequest(request)) {
        qDebug() << "strange request - expected json on " << request->url().path();
        return QHttpServerResponse("Expected Json POST request",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    QString lowMethod = QString(method).toLower();
    if ("filter" == lowMethod) {
        return getByFilter(request);
    } else if ("addgrade" == lowMethod) {
        return addGrade(request);
    } else if ("updategrade" == lowMethod) {
        return updateGrade(request);
    } else if ("removegrade" == lowMethod) {
        return removeGrade(request);
    } else if ("getbyassignmentid" == lowMethod) {
        return getByAssignmentId(request);
    }

    qDebug() << "unsupported fcs method: " << method;
    return QHttpServerResponse("Method not valid", QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse GradeController::addGrade(const QHttpServerRequest *request)
{
    GradeDto dto(request->body());
    std::shared_ptr<Grade> grade = std::shared_ptr<Grade>(
        Dto2ModelMapper::instance().convertFromDto(dto));
    ResponseDto responseDto(gradeRepository->addGrade(*grade));
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse GradeController::updateGrade(const QHttpServerRequest *request)
{
    GradeDto dto(request->body());
    std::shared_ptr<Grade> grade = std::shared_ptr<Grade>(
        Dto2ModelMapper::instance().convertFromDto(dto));
    ResponseDto responseDto(gradeRepository->updateGrade(*grade));
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse GradeController::removeGrade(const QHttpServerRequest *request)
{
    GradeDto dto(request->body());
    std::shared_ptr<Grade> grade = std::shared_ptr<Grade>(
        Dto2ModelMapper::instance().convertFromDto(dto));
    ResponseDto responseDto(gradeRepository->removeGrade(*grade));
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse GradeController::getByFilter(const QHttpServerRequest *request)
{
    GradeFilterRequestDto filterDto(request->body());
    const QList<std::shared_ptr<Grade>> grades = gradeRepository
                                                     ->getByFilter(filterDto.getNameOfStudent(),
                                                                   filterDto.getName(),
                                                                   filterDto.getSurName(),
                                                                   filterDto.getSubject(),
                                                                   filterDto.getModules(),
                                                                   filterDto.getElementOfControl(),
                                                                   filterDto.getExaminerLogin());

    QList<GradeDto> gradeList;
    for (std::shared_ptr<Grade> gradePtr : grades) {
        GradeDto gradeDto = Dto2ModelMapper::convertFromModel(*gradePtr);
        gradeList.append(gradeDto);
    }

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               ListDto<GradeDto>(gradeList).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse GradeController::getByAssignmentId(const QHttpServerRequest *request)
{
    GradeDto dto(request->body());
    const QList<std::shared_ptr<Grade>> grades = gradeRepository->getByAssignmentId(
        dto.getAssignmentId());

    QList<GradeDto> gradeList;
    for (std::shared_ptr<Grade> gradePtr : grades) {
        GradeDto gradeDto = Dto2ModelMapper::convertFromModel(*gradePtr);
        gradeList.append(gradeDto);
    }

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               ListDto<GradeDto>(gradeList).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}
