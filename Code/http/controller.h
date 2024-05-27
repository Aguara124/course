#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QHttpServer>
#include <QHttpServerRequest>
#include <QLinkedList>

class Controller {

public:
  Controller() {}

  virtual QHttpServerResponse handleRequest(const QString &controller,
                                            const QString &method,
                                            const QHttpServerRequest *request,
                                            QLinkedList<Controller *> &chain)
      = 0;

  protected:
  static bool isJsonRequest(const QHttpServerRequest *request)
  {
      if (request->method() != QHttpServerRequest::Method::Post) {
          return false;
      }

      if (!request->headers().empty()) {
          for (QPair<QByteArray, QByteArray> header : request->headers()) {
              if (QString(header.first).toLower() == "content-type"
                  && !QString(header.second).trimmed().toLower().startsWith("application/json")) {
                  return false;
              }
          }
      }

      return true;
  }

};

#endif // CONTROLLER_H
