#include "studentcontroller.h"
#include "assistantdto.h"
#include "dto2modelmapper.h"
#include "listdto.h"
#include "responsedto.h"
#include "studentdto.h"

QHttpServerResponse StudentController::handleRequest(const QString &controller,
                                                     const QString &method,
                                                     const QHttpServerRequest *request,
                                                     QLinkedList<Controller *> &chain)
{
    if ("student" != QString(controller).toLower()) {
        return Controller::handleRequest(controller, method, request, chain);
    }

    if (!Controller::isJsonRequest(request)) {
        qDebug() << "strange request - expected json on " << request->url().path();
        return QHttpServerResponse("Expected Json POST request",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    if ("getbylogin" == QString(method).toLower()) {
        return getByLogin(request);
    } else if ("addstudent" == QString(method).toLower()) {
        return addStudent(request);
    } else if ("getbygroup" == QString(method).toLower()) {
        return getByGroup(request);
    } else if ("updatestudent" == QString(method).toLower()) {
        return updateStudent(request);
    }

    qDebug() << "unsupported student method: " << method;
    return QHttpServerResponse("Method not valid", QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse StudentController::getByLogin(const QHttpServerRequest *request)
{
    StudentDto studentDto(request->body());
    std::shared_ptr<Student> student = studentRepository->getByLogin(studentDto.getLogin());

    if (student == nullptr) {
        return QHttpServerResponse("Student not found", QHttpServerResponse::StatusCode::NotFound);
    }

    studentDto = Dto2ModelMapper::instance().convertFromModel(*student);

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               studentDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse StudentController::addStudent(const QHttpServerRequest *request)
{
    StudentDto dto(request->body());
    ResponseDto responseDto(
        studentRepository->addStudent(*Dto2ModelMapper::instance().convertFromDto(dto)));

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse StudentController::updateStudent(const QHttpServerRequest *request)
{
    StudentDto dto(request->body());
    ResponseDto responseDto(
        studentRepository->updateStudent(*Dto2ModelMapper::instance().convertFromDto(dto)));

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse StudentController::getByGroup(const QHttpServerRequest *request)
{
    AssistantDto assistantDto(request->body());
    QList<std::shared_ptr<Student>> studentList = studentRepository
                                                      ->getByGroup(assistantDto.getGroup(),
                                                                   assistantDto.getSubGroup(),
                                                                   assistantDto.getLogin());

    QList<StudentDto> resultList;
    for (std::shared_ptr<Student> student : studentList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*student));
    }

    ListDto<StudentDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}
