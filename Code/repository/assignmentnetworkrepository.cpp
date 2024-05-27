#include "assignmentnetworkrepository.h"
#include <QByteArray>
#include <QHttpServer>
#include "assignment.h"
#include "assignmentdto.h"
#include "authdata.h"
#include "dto2modelmapper.h"
#include "listdto.h"
#include "responsedto.h"
#include "simplerequester.h"
#include "studentdto.h"

AssignmentNetworkRepository::AssignmentNetworkRepository() {}

const std::shared_ptr<Assignment> AssignmentNetworkRepository::getById(int id)
{
    AssignmentDto dto;
    dto.setId(id);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/assignment/getById",
                                         dto,
                                         headers);

    std::shared_ptr<Assignment> assignmentPtr;
    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        AssignmentDto other(requester.getRawResult());
        assignmentPtr = std::shared_ptr<Assignment>(
            Dto2ModelMapper::instance().convertFromDto(other));

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return assignmentPtr;
}

bool AssignmentNetworkRepository::addAssignment(const Assignment &assignment)
{
    AssignmentDto dto = Dto2ModelMapper::instance().convertFromModel(assignment);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/assignment/addAssignment",
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

const QList<std::shared_ptr<Assignment>> AssignmentNetworkRepository::getByExaminerName(
    const QString &assistantLogin)
{
    AssignmentDto dto;
    dto.setExaminerName(assistantLogin);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/assignment/getByExaminerName",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Assignment>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<AssignmentDto> listDto(requester.getRawResult());

        for (AssignmentDto v : listDto.getList()) {
            result.append(
                std::shared_ptr<Assignment>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

const QList<std::shared_ptr<Assignment>> AssignmentNetworkRepository::getByStudentLogin(
    const QString &studentLogin)
{
    StudentDto dto;
    dto.setLogin(studentLogin);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/assignment/getByStudentLogin",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Assignment>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<AssignmentDto> listDto(requester.getRawResult());

        for (AssignmentDto v : listDto.getList()) {
            result.append(
                std::shared_ptr<Assignment>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

const QList<std::shared_ptr<Assignment>> AssignmentNetworkRepository::getByGroup(int group,
                                                                                 int subGroup)
{
    AssignmentDto dto;
    dto.setGroup(group);
    dto.setSubGroup(subGroup);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/assignment/getByGroup",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Assignment>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<AssignmentDto> listDto(requester.getRawResult());

        for (AssignmentDto v : listDto.getList()) {
            result.append(
                std::shared_ptr<Assignment>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

bool AssignmentNetworkRepository::updateAssignment(const Assignment &assignment)
{
    AssignmentDto dto = Dto2ModelMapper::instance().convertFromModel(assignment);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/assignment/updateAssignment",
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

