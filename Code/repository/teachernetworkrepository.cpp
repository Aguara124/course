#include "teachernetworkrepository.h"
#include <QHttpServer>
#include "authdata.h"
#include "dto2modelmapper.h"
#include "listdto.h"
#include "responsedto.h"
#include "simplerequester.h"
#include "teacherdto.h"

TeacherNetworkRepository::TeacherNetworkRepository() {}

const QList<std::shared_ptr<Teacher>> TeacherNetworkRepository::getByLogin(
    const QString &teacherLogin)
{
    TeacherDto dto;
    dto.setLogin(teacherLogin);

    SimpleRequester requester;
    QStringMap headers;

    headers.insert("Token", AuthData::instance().getToken());

    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/teacher/getByLogin",
                                         dto,
                                         headers);

    QList<std::shared_ptr<Teacher>> result;

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ListDto<TeacherDto> listDto(requester.getRawResult());

        for (TeacherDto v : listDto.getList()) {
            result.append(std::shared_ptr<Teacher>(Dto2ModelMapper::instance().convertFromDto(v)));
        }

        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }
    }

    return result;
}

bool TeacherNetworkRepository::updateTeacher(const Teacher &teacher)
{
    TeacherDto dto = Dto2ModelMapper::instance().convertFromModel(teacher);

    SimpleRequester requester;
    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/teacher/updateTeacher",
                                         dto);

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        QString token = requester.getHeaders()["Token"];
        if (!token.isNull()) {
            AuthData::instance().setToken(token);
        }

        ResponseDto responseDto(requester.getRawResult());
        qDebug() << "teacher update message: " << responseDto.getMessage();
        return responseDto.getSuccess();
    }

    return false;
}
