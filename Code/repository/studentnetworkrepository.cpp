#include "studentnetworkrepository.h"
#include <QHttpServerResponse>
#include "simplerequester.h"

#include <QMessageBox>
#include "assistantdto.h"
#include "listdto.h"
#include "responsedto.h"
#include "student.h"
#include "studentdto.h"

#include "dto2modelmapper.h"

StudentNetworkRepository::StudentNetworkRepository() {}

const std::shared_ptr<Student> StudentNetworkRepository::getByLogin(const QString &login)
{
    StudentDto dto;
    dto.setLogin(login);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/student/getByLogin",
                                         dto,
                                         headers);
    std::shared_ptr<Student> student;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        StudentDto studentDto(requester.getRawResult());
        student = std::shared_ptr<Student>(Dto2ModelMapper::instance().convertFromDto(studentDto));

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return student;
}

bool StudentNetworkRepository::addStudent(const Student &student)
{
    StudentDto dto = Dto2ModelMapper::instance().convertFromModel(student);

    SimpleRequester requester;
    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/student/addStudent",
                                         dto);

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }

        ResponseDto responseDto(requester.getRawResult());
        qDebug() << "student registration message: " << responseDto.getMessage();
        return responseDto.getSuccess();
    }

    return false;
}

const QList<std::shared_ptr<Student>> StudentNetworkRepository::getByGroup(
    int group, int subgroup, const QString &assistantLogin)
{
    AssistantDto dto;
    dto.setLogin(assistantLogin);
    dto.setGroup(group);
    dto.setSubGroup(subgroup);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/student/getByGroup",
                                         dto,
                                         headers);
    QList<std::shared_ptr<Student>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<StudentDto> listDto(requester.getRawResult());

        for (StudentDto v : listDto.getList()) {
            StudentDto studentDto(requester.getRawResult());
            result.append(
                std::shared_ptr<Student>(Dto2ModelMapper::instance().convertFromDto(studentDto)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

bool StudentNetworkRepository::updateStudent(const Student &student)
{
    StudentDto dto = Dto2ModelMapper::instance().convertFromModel(student);

    SimpleRequester requester;
    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/student/updateStudent",
                                         dto);

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }

        ResponseDto responseDto(requester.getRawResult());
        qDebug() << "student update message: " << responseDto.getMessage();
        return responseDto.getSuccess();
    }

    return false;
}
