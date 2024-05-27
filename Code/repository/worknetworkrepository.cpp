#include "worknetworkrepository.h"
#include <QHttpServer>
#include "authdata.h"
#include "dto2modelmapper.h"
#include "listdto.h"
#include "responsedto.h"
#include "simplerequester.h"
#include "work.h"
#include "workdto.h"

WorkNetworkRepository::WorkNetworkRepository() {}

const std::shared_ptr<Work> WorkNetworkRepository::getById(int id)
{
    WorkDto dto;
    dto.setId(id);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/work/getById",
                                         dto,
                                         headers);

    std::shared_ptr<Work> workPtr;
    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        WorkDto other(requester.getRawResult());
        workPtr = std::shared_ptr<Work>(Dto2ModelMapper::convertFromDto(other));

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return workPtr;
}

bool WorkNetworkRepository::updateWork(const Work &work)
{
    WorkDto dto = Dto2ModelMapper::instance().convertFromModel(work);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/work/updateWork",
                                         dto,
                                         headers);

    std::shared_ptr<Work> workPtr;
    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ResponseDto response(requester.getRawResult());

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }

        return response.getSuccess();
    }

    return false;
}

const QList<std::shared_ptr<Work>> WorkNetworkRepository::getByAssignmentId(int assignmentId)
{
    WorkDto dto;
    dto.setAssignmentId(assignmentId);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/work/getByAssignmentId",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Work>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<WorkDto> listDto(requester.getRawResult());

        for (WorkDto v : listDto.getList()) {
            result.append(std::shared_ptr<Work>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

bool WorkNetworkRepository::addWork(const Work &work)
{
    WorkDto dto = Dto2ModelMapper::instance().convertFromModel(work);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/work/addWork",
                                         dto,
                                         headers);

    std::shared_ptr<Work> workPtr;
    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ResponseDto response(requester.getRawResult());

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }

        return response.getSuccess();
    }

    return false;
}

const QList<std::shared_ptr<Work>> WorkNetworkRepository::getByStudentLogin(
    const QString &studentLogin)
{
    WorkDto dto;
    dto.setLogin(studentLogin);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/work/getByStudentLogin",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Work>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<WorkDto> listDto(requester.getRawResult());

        for (WorkDto v : listDto.getList()) {
            result.append(std::shared_ptr<Work>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}
