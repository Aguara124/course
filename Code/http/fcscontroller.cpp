#include "fcscontroller.h"
#include "dto2modelmapper.h"
#include "fcs.h"
#include "fcsdto.h"
#include "fcsfilterrequestdto.h"
#include "listdto.h"
#include "responsedto.h"

QHttpServerResponse FcsController::handleRequest(const QString &controller,
                                                 const QString &method,
                                                 const QHttpServerRequest *request,
                                                 QLinkedList<Controller *> &chain)
{
    if ("fcs" != QString(controller).toLower()) {
        return Controller::handleRequest(controller, method, request, chain);
    }

    if (!Controller::isJsonRequest(request)) {
        qDebug() << "strange request - expected json on " << request->url().path();
        return QHttpServerResponse("Expected Json POST request",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    if ("filter" == QString(method).toLower()) {
        return getByFilter(request);
    } else if ("getbylecturer" == QString(method).toLower()) {
        return getByLecturer(request);
    } else if ("update" == QString(method).toLower()) {
        return updateFcs(request);
    } else if ("faculty" == QString(method).toLower()) {
        return getFaculty(request);
    } else if ("program" == QString(method).toLower()) {
        return getProgram(request);
    }

    qDebug() << "unsupported fcs method: " << method;
    return QHttpServerResponse("Method not valid", QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse FcsController::getByFilter(const QHttpServerRequest *request)
{
    FcsFilterRequestDto filterDto(request->body());

    const QList<std::shared_ptr<Fcs>> fcsList = fcsRepository->getFcsByFilter(filterDto.getProgram(),
                                                                              filterDto.getPilot(),
                                                                              filterDto.getCourse());

    QList<FcsDto> resultList;
    for (std::shared_ptr<Fcs> fcs : fcsList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*fcs));
    }

    ListDto<FcsDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse FcsController::getByLecturer(const QHttpServerRequest *request)
{
    FcsDto filterDto(request->body());

    const QList<std::shared_ptr<Fcs>> fcsList = fcsRepository
                                                    ->getFcsByLecturer(filterDto.getLecturer(),
                                                                       filterDto.getProgram(),
                                                                       filterDto.getSubject());

    QList<FcsDto> resultList;
    for (std::shared_ptr<Fcs> fcs : fcsList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*fcs));
    }

    ListDto<FcsDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse FcsController::updateFcs(const QHttpServerRequest *request)
{
    FcsDto dto(request->body());
    std::shared_ptr<Fcs> fcs = std::shared_ptr<Fcs>(Dto2ModelMapper::instance().convertFromDto(dto));
    ResponseDto responseDto(fcsRepository->update(*fcs));
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse FcsController::getById(const QHttpServerRequest *request)
{
    FcsDto fcsDto(request->body());
    std::shared_ptr<Fcs> fcs = fcsRepository->getById(fcsDto.getId());

    if (fcs == nullptr) {
        return QHttpServerResponse("Fcs not found", QHttpServerResponse::StatusCode::NotFound);
    }

    fcsDto = Dto2ModelMapper::instance().convertFromModel(*fcs);

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               fcsDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse FcsController::getFaculty(const QHttpServerRequest *request)
{
    FcsDto filterDto(request->body());

    const QList<std::shared_ptr<Fcs>> fcsList = fcsRepository->getFaculty();

    QList<FcsDto> resultList;
    for (std::shared_ptr<Fcs> fcs : fcsList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*fcs));
    }

    ListDto<FcsDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse FcsController::getProgram(const QHttpServerRequest *request)
{
    FcsDto filterDto(request->body());

    const QList<std::shared_ptr<Fcs>> fcsList = fcsRepository->getProgram(filterDto.getFaculty());

    QList<FcsDto> resultList;
    for (std::shared_ptr<Fcs> fcs : fcsList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*fcs));
    }

    ListDto<FcsDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}
