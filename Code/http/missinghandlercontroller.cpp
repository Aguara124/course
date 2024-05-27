#include "missinghandlercontroller.h"
#include "QHttpServerResponder"

MissingHandlerController::MissingHandlerController() {}

QHttpServerResponse MissingHandlerController::handleRequest(const QString &controller,
                                                            const QString &method,
                                                            const QHttpServerRequest *request,
                                                            QLinkedList<Controller *> &chain)
{
    (void) request;
    (void) chain;

    qDebug() << "missing controller " + controller + " method " + method;
    return QHttpServerResponse("MissingHandlerController",
                               QHttpServerResponder::StatusCode::NotFound);
}
