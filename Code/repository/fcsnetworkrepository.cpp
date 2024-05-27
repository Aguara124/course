#include "fcsnetworkrepository.h"
#include "dto2modelmapper.h"
#include "fcsdto.h"
#include "fcsfilterrequestdto.h"
#include "listdto.h"
#include "responsedto.h"

#include "simplerequester.h"
#include <QHttpServerResponse>

const QList<std::shared_ptr<Fcs>> FcsNetworkRepository::getFcsByFilter(const QString &program,
                                                                       bool pilot,
                                                                       int course)
{
    FcsFilterRequestDto dto;
    dto.setProgram(program);
    dto.setCourse(course);
    dto.setPilot(pilot);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/fcs/filter",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Fcs>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<FcsDto> fcsListDto(requester.getRawResult());

        for (FcsDto v : fcsListDto.getList()) {
            result.append(std::shared_ptr<Fcs>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

const QList<std::shared_ptr<Fcs>> FcsNetworkRepository::getFcsByLecturer(const QString &lecturer,
                                                                         const QString &program,
                                                                         const QString &subject)
{
    FcsDto dto;
    dto.setLecturer(lecturer);
    dto.setProgram(program);
    dto.setSubject(subject);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/fcs/getByLecturer",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Fcs>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<FcsDto> fcsListDto(requester.getRawResult());

        for (FcsDto v : fcsListDto.getList()) {
            result.append(std::shared_ptr<Fcs>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

bool FcsNetworkRepository::update(const Fcs &fcs)
{
    FcsDto dto = Dto2ModelMapper::instance().convertFromModel(fcs);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/fcs/update",
                                         dto,
                                         headers);
    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }

        ResponseDto responseDto(requester.getRawResult());
        return responseDto.getSuccess();
    }

    return false;
}

const std::shared_ptr<Fcs> FcsNetworkRepository::getById(int id)
{
    FcsDto dto;
    dto.setId(id);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/fcs/getById",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Assistant>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        FcsDto fcsDto(requester.getRawResult());

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }

        return std::shared_ptr<Fcs>(Dto2ModelMapper::instance().convertFromDto(fcsDto));
    }

    return nullptr;
}

const QList<std::shared_ptr<Fcs>> FcsNetworkRepository::getFaculty()
{
    FcsDto dummyDto;

    SimpleRequester requester;
    QStringMap headers;

    if (!AuthData::instance().getToken().isNull()) {
        headers.insert("Token", AuthData::instance().getToken());
    }

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/fcs/faculty",
                                         dummyDto,
                                         headers);
    QList<std::shared_ptr<Fcs>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<FcsDto> fcsListDto(requester.getRawResult());

        for (FcsDto v : fcsListDto.getList()) {
            result.append(std::shared_ptr<Fcs>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

const QList<std::shared_ptr<Fcs>> FcsNetworkRepository::getProgram(const QString &faculty)
{
    FcsDto dto;
    dto.setFaculty(faculty);

    SimpleRequester requester;
    QStringMap headers;

    if (!AuthData::instance().getToken().isNull()) {
        headers.insert("Token", AuthData::instance().getToken());
    }

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/fcs/program",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Fcs>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<FcsDto> fcsListDto(requester.getRawResult());

        for (FcsDto v : fcsListDto.getList()) {
            result.append(std::shared_ptr<Fcs>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}
