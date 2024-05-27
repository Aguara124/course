#include "registrationcontroller.h"
#include "responsedto.h"
#include "studentdto.h"

QHttpServerResponse RegistrationController::handleRequest(const QString &controller,
                                                          const QString &method,
                                                          const QHttpServerRequest *request,
                                                          QLinkedList<Controller *> &chain)
{
    if (!Controller::isJsonRequest(request)) {
        qDebug() << "strange request - expected json on " << request->url().path();
        return QHttpServerResponse("Expected Json POST request",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    QLinkedList<Controller *> emptyChain;

    if ("auth" == controller.toLower() && "check" == method.toLower()) {
        return authController->handleRequest(controller, method, request, emptyChain);
    } else if ("student" == QString(controller).toLower()) {
        if ("addstudent" == method.toLower()) {
            return studentController->handleRequest(controller, method, request, emptyChain);
        }
    } else if ("fcs" == QString(controller).toLower()) {
        if ("program" == method.toLower() || "faculty" == method.toLower()) {
            return fcsController->handleRequest(controller, method, request, emptyChain);
        }
    }

    return Controller::handleRequest(controller, method, request, chain);
}
