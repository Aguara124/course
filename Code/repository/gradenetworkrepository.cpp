#include "gradenetworkrepository.h"
#include <QHttpServerResponse>
#include "dto2modelmapper.h"
#include "grade.h"
#include "gradedto.h"
#include "gradefilterrequestdto.h"
#include "listdto.h"
#include "responsedto.h"
#include "simplerequester.h"

const QList<std::shared_ptr<Grade>> GradeNetworkRepository::getByFilter(
    const QString &nameOfStudent,
    const QString &name,
    const QString &surName,
    const QString &subject,
    const QStringList modules,
    const QString &elementOfControl,
    const QString &examinerLogin)
{
    GradeFilterRequestDto dto;
    dto.setElementOfControl(elementOfControl);
    dto.setExaminerLogin(examinerLogin);
    dto.setModules(modules);
    dto.setName(name);
    dto.setNameOfStudent(nameOfStudent);
    dto.setSubject(subject);
    dto.setSurName(surName);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/grade/filter",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Grade>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<GradeDto> gradeListDto(requester.getRawResult());

        for (GradeDto v : gradeListDto.getList()) {
            result.append(std::shared_ptr<Grade>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

const QList<std::shared_ptr<Grade>> GradeNetworkRepository::getByAssignmentId(int assignmentId)
{
    GradeDto dto;
    dto.setAssignmentId(assignmentId);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl()
                                             + "/grade/getByAssignmentId",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Grade>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<GradeDto> gradeListDto(requester.getRawResult());

        for (GradeDto v : gradeListDto.getList()) {
            result.append(std::shared_ptr<Grade>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

bool GradeNetworkRepository::addGrade(const Grade &grade)
{
    GradeDto dto = Dto2ModelMapper::instance().convertFromModel(grade);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/grade/addGrade",
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

bool GradeNetworkRepository::updateGrade(const Grade &grade)
{
    GradeDto dto = Dto2ModelMapper::instance().convertFromModel(grade);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/grade/updateGrade",
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

bool GradeNetworkRepository::removeGrade(const Grade &grade)
{
    GradeDto dto;

    dto.setId(grade.getId());

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/grade/removeGrade",
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
