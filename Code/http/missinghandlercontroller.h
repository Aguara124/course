#ifndef MISSINGHANDLERCONTROLLER_H
#define MISSINGHANDLERCONTROLLER_H

#include "controller.h"

class MissingHandlerController : public Controller {
public:
  MissingHandlerController();

  QHttpServerResponse handleRequest(const QString &controller,
                                    const QString &method,
                                    const QHttpServerRequest *request,
                                    QLinkedList<Controller *> &chain) override;
};

#endif // MISSINGHANDLERCONTROLLER_H
