#include "consultationcontroller.h"
#include "consultation.h"
#include "consultationdto.h"
#include "dto2modelmapper.h"
#include "listdto.h"
#include "repositorymanager.h"
#include "responsedto.h"

ConsultationController::ConsultationController()
    : consultationRepository(RepositoryManager::instance().consultationRepository())
{}

QHttpServerResponse ConsultationController::getById(const QHttpServerRequest *request)
{
    ConsultationDto consultationDto(request->body());
    std::shared_ptr<Consultation> consultation = consultationRepository->getById(
        consultationDto.getId());

    if (consultation == nullptr) {
        return QHttpServerResponse("Consultation not found",
                                   QHttpServerResponse::StatusCode::NotFound);
    }

    consultationDto = Dto2ModelMapper::instance().convertFromModel(*consultation);

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               consultationDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ConsultationController::updateConsultationState(const QHttpServerRequest *request)
{
    ConsultationDto dto(request->body());
    ResponseDto responseDto(
        consultationRepository->updateConsultationState(dto.getId(), dto.getChecked()));
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ConsultationController::getConsultations(const QHttpServerRequest *request)
{
    ConsultationDto consultationDto(request->body());
    QList<std::shared_ptr<Consultation>> consultationList
        = consultationRepository->getConsultations(consultationDto.getExaminerName(),
                                                   consultationDto.getChecked());

    QList<ConsultationDto> resultList;
    for (std::shared_ptr<Consultation> consultation : consultationList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*consultation));
    }

    ListDto<ConsultationDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ConsultationController::addConsultation(const QHttpServerRequest *request)
{
    ConsultationDto dto(request->body());
    ResponseDto responseDto(
        consultationRepository->addConsultation(*Dto2ModelMapper::instance().convertFromDto(dto)));
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ConsultationController::handleRequest(const QString &controller,
                                                          const QString &method,
                                                          const QHttpServerRequest *request,
                                                          QLinkedList<Controller *> &chain)
{
    if ("consultation" != QString(controller).toLower()) {
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
    } else if ("updateconsultationstate" == lowMethod) {
        return updateConsultationState(request);
    } else if ("getconsultations" == lowMethod) {
        return getConsultations(request);
    } else if ("addconsultation" == lowMethod) {
        return addConsultation(request);
    }

    qDebug() << "unsupported consultation method: " << method;
    return QHttpServerResponse("Method not valid", QHttpServerResponse::StatusCode::NotFound);
}
