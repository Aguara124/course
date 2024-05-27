#include "consultationnetworkrepository.h"
#include <QHttpServer>
#include <QList>
#include "authdata.h"
#include "consultation.h"
#include "consultationdto.h"
#include "dto2modelmapper.h"
#include "listdto.h"
#include "responsedto.h"
#include "simplerequester.h"

ConsultationNetworkRepository::ConsultationNetworkRepository() {}

bool ConsultationNetworkRepository::addConsultation(const Consultation &consultation)
{
    ConsultationDto dto = Dto2ModelMapper::instance().convertFromModel(consultation);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/consultation/addConsultation",
                                         dto,
                                         headers);

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }

        return ResponseDto(requester.getRawResult()).getSuccess();
    }

    return false;
}

const std::shared_ptr<Consultation> ConsultationNetworkRepository::getById(int id)
{
    ConsultationDto dto;
    dto.setId(id);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/consultation/getById",
                                         dto,
                                         headers);

    std::shared_ptr<Consultation> consultation;
    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ConsultationDto consultationDto(requester.getRawResult());
        consultation = std::shared_ptr<Consultation>(
            Dto2ModelMapper::instance().convertFromDto(consultationDto));

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return consultation;
}

bool ConsultationNetworkRepository::updateConsultationState(int consultationId, bool isChecked)
{
    ConsultationDto dto;
    dto.setId(consultationId);
    dto.setChecked(isChecked);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/consultation/updateConsultationState",
                                         dto,
                                         headers);

    std::shared_ptr<Consultation> consultation;
    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }

        return ResponseDto(requester.getRawResult()).getSuccess();
    }

    return false;
}

const QList<std::shared_ptr<Consultation>> ConsultationNetworkRepository::getConsultations(
    const QString &assistantLogin, bool checkedOnly)
{
    ConsultationDto dto;
    dto.setExaminerName(assistantLogin);
    dto.setChecked(checkedOnly);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/consultation/getConsultations",
                                         dto,
                                         headers);

    QList<std::shared_ptr<Consultation>> consultationList;
    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<ConsultationDto> listDto(requester.getRawResult());

        for (ConsultationDto v : listDto.getList()) {
            consultationList.append(
                std::shared_ptr<Consultation>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return consultationList;
}
