#ifndef HTTPSUPPORT_H
#define HTTPSUPPORT_H

#include <QHttpServer>
#include <QList>

#include "controller.h"
#include "authdbrepository.h"

class HttpSupport {
 private:
  void setupSsl();
  QHttpServer::MissingHandler missingHandler;

  QHttpServer httpServer;
  QLinkedList<Controller *> controllersChain;
  qint16 port;
  AuthDbRepository dbRepository;

  static QHttpServerResponse asyncExecute(const QString &controller,
                                          const QString &method,
                                          const QHttpServerRequest *serverRequest,
                                          QLinkedList<Controller *> chain);

  void setupRoutes();
  void setupAfterRequest();

  public:
  HttpSupport();

  void addController(Controller *controller);
  void startServer();
};

#endif  // HTTPSUPPORT_H
