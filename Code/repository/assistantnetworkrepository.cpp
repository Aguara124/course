#include "assistantnetworkrepository.h"
#include <QHttpServer>
#include "assistant.h"
#include "assistantdto.h"
#include "authdata.h"
#include "dto2modelmapper.h"
#include "listdto.h"
#include "responsedto.h"
#include "simplerequester.h"

const QList<std::shared_ptr<Assistant>> AssistantNetworkRepository::getByLogin(
    const QString &assistantLogin)
{
    AssistantDto dto;
    dto.setLogin(assistantLogin);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/assistant/getByLogin",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Assistant>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<AssistantDto> listDto(requester.getRawResult());

        for (AssistantDto other : listDto.getList()) {
            result.append(
                std::shared_ptr<Assistant>(Dto2ModelMapper::instance().convertFromDto(other)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

const QList<std::shared_ptr<Assistant>> AssistantNetworkRepository::getByGroup(int group,
                                                                               int subgroup)
{
    AssistantDto dto;
    dto.setGroup(group);
    dto.setSubGroup(subgroup);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/assistant/getByGroup",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Assistant>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<AssistantDto> listDto(requester.getRawResult());

        for (AssistantDto other : listDto.getList()) {
            result.append(
                std::shared_ptr<Assistant>(Dto2ModelMapper::instance().convertFromDto(other)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

const QList<std::shared_ptr<Assistant>> AssistantNetworkRepository::getByTeacher(
    const QString &teacherLogin)
{
    AssistantDto dto;
    dto.setTeacher(teacherLogin);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/assistant/getByTeacher",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Assistant>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<AssistantDto> listDto(requester.getRawResult());

        for (AssistantDto other : listDto.getList()) {
            result.append(
                std::shared_ptr<Assistant>(Dto2ModelMapper::instance().convertFromDto(other)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

bool AssistantNetworkRepository::updateAssistant(const Assistant &assistant)
{
    AssistantDto dto = Dto2ModelMapper::instance().convertFromModel(assistant);

    SimpleRequester requester;
    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/assistant/updateStudent",
                                         dto);

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }

        ResponseDto responseDto(requester.getRawResult());
        qDebug() << "assistant update message: " << responseDto.getMessage();
        return responseDto.getSuccess();
    }

    return false;
}
