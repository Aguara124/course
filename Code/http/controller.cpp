#include "controller.h"
#include <QHttpServer>
#include <QLinkedList>

QHttpServerResponse Controller::handleRequest(const QString &controller,
                                              const QString &method,
                                              const QHttpServerRequest *request,
                                              QLinkedList<Controller *> &chain)
{
    if (!chain.empty()) {
        Controller *next = chain.front();
        chain.pop_front();

        return next->handleRequest(controller, method, request, chain);
    }

    return QHttpServerResponse("MissingHandler", QHttpServerResponder::StatusCode::BadRequest);
}
