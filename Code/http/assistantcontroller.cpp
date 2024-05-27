#include "assistantcontroller.h"
#include "assistant.h"
#include "assistantdto.h"
#include "dto2modelmapper.h"
#include "listdto.h"
#include "repositorymanager.h"
#include "responsedto.h"

AssistantController::AssistantController()
    : assistantRepository(RepositoryManager::instance().assistantRepository())
{}

QHttpServerResponse AssistantController::getByLogin(const QHttpServerRequest *request)
{
    AssistantDto assistantDto(request->body());
    QList<std::shared_ptr<Assistant>> assistantList = assistantRepository->getByLogin(
        assistantDto.getLogin());

    QList<AssistantDto> resultList;
    for (std::shared_ptr<Assistant> assistant : assistantList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*assistant));
    }

    ListDto<AssistantDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse AssistantController::getByGroup(const QHttpServerRequest *request)
{
    AssistantDto assistantDto(request->body());
    QList<std::shared_ptr<Assistant>> assistantList
        = assistantRepository->getByGroup(assistantDto.getGroup(), assistantDto.getSubGroup());

    QList<AssistantDto> resultList;
    for (std::shared_ptr<Assistant> assistant : assistantList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*assistant));
    }

    ListDto<AssistantDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse AssistantController::getByTeacher(const QHttpServerRequest *request)
{
    AssistantDto assistantDto(request->body());
    QList<std::shared_ptr<Assistant>> assistantList = assistantRepository->getByTeacher(
        assistantDto.getTeacher());

    QList<AssistantDto> resultList;
    for (std::shared_ptr<Assistant> assistant : assistantList) {
        resultList.append(Dto2ModelMapper::instance().convertFromModel(*assistant));
    }

    ListDto<AssistantDto> listDto(resultList);
    return QHttpServerResponse(QString("application/json").toUtf8(),
                               listDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse AssistantController::updateAssistant(const QHttpServerRequest *request)
{
    AssistantDto dto(request->body());
    ResponseDto responseDto(
        assistantRepository->updateAssistant(*Dto2ModelMapper::instance().convertFromDto(dto)));

    return QHttpServerResponse(QString("application/json").toUtf8(),
                               responseDto.toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse AssistantController::handleRequest(const QString &controller,
                                                       const QString &method,
                                                       const QHttpServerRequest *request,
                                                       QLinkedList<Controller *> &chain)
{
    if ("assistant" != QString(controller).toLower()) {
        return Controller::handleRequest(controller, method, request, chain);
    }

    if (!Controller::isJsonRequest(request)) {
        qDebug() << "strange request - expected json on " << request->url().path();
        return QHttpServerResponse("Expected Json POST request",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    QString lowMethod = QString(method).toLower();
    if ("getbylogin" == lowMethod) {
        return getByLogin(request);
    } else if ("getbygroup" == lowMethod) {
        return getByGroup(request);
    } else if ("getbyteacher" == lowMethod) {
        return getByTeacher(request);
    } else if ("updateassistant" == lowMethod) {
        return updateAssistant(request);
    }

    qDebug() << "unsupported assistant method: " << method;
    return QHttpServerResponse("Method not valid", QHttpServerResponse::StatusCode::NotFound);
}
