#include "authcontroller.h"
#include <QThreadStorage>
#include "authinfo.h"
#include "authrequestdto.h"
#include "responsedto.h"

QHttpServerResponse AuthController::handleRequest(const QString &controller,
                                                  const QString &method,
                                                  const QHttpServerRequest *request,
                                                  QLinkedList<Controller *> &chain)

{
    const QString requestUri = request->url().path();

    if (controller.toLower() != "auth") {
        if (!request->headers().isEmpty()) {
            for (QPair<QByteArray, QByteArray> p : request->headers()) {
                if (QString(p.first).toLower() == "token") {
                    authRepository->checkToken(p.second, request->remoteAddress().toString());
                    break;
                }
            }
        }

        QThreadStorage<AuthInfo *> threadStorage;
        if (!threadStorage.hasLocalData() || threadStorage.localData() == nullptr) {
            return QHttpServerResponse("Forbidden", QHttpServerResponder::StatusCode::Forbidden);
        }

        return Controller::handleRequest(controller, method, request, chain);
    }

    if (!isJsonRequest(request)) {
        return QHttpServerResponse(QHttpServerResponse::StatusCode::Unauthorized);
    }

    AuthRequestDTO requestDto(request->body());

    if (method.toLower() == "check") {
        ResponseDto responseDto(authRepository->checkLogin(requestDto.getLogin()));
        return QHttpServerResponse(QString("application/json").toUtf8(),
                                   responseDto.toJson(),
                                   QHttpServerResponse::StatusCode::Ok);
    } else if (method.toLower() == "authenticate"
               && authRepository->authenticate(requestDto.getLogin(),
                                               requestDto.getPassword(),
                                               requestDto.getRole(),
                                               request->remoteAddress().toString())) {
        ResponseDto responseDto(true, "Auth OK");
        return QHttpServerResponse(QString("application/json").toUtf8(),
                                   responseDto.toJson(),
                                   QHttpServerResponse::StatusCode::Ok);
    } else {
        return QHttpServerResponse(QHttpServerResponse::StatusCode::Unauthorized);
    }
}
