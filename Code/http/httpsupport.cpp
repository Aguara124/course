#include "httpsupport.h"

#include <QHttpServer>
#include <QLinkedList>
#include <QSsl>
#include <QSslKey>
#include <QtConcurrent/QtConcurrent>

#include "appsettings.h"
#include "authinfo.h"

HttpSupport::HttpSupport() {
    httpServer.setMissingHandler(missingHandler);
    port = AppSettings::instance().getServerPort();

    setupSsl();
    setupAfterRequest();
    setupRoutes();
}

void HttpSupport::addController(Controller *controller) {
  if (!controllersChain.contains(controller)) {
    controllersChain.append(controller);
  }
}

void HttpSupport::startServer() {
    httpServer.listen(QHostAddress::Any, port);
    qDebug() << QCoreApplication::translate("hse grade", "Listening on any:%1").arg(port);
}

QHttpServerResponse HttpSupport::asyncExecute(const QString &controller,
                                              const QString &method,
                                              const QHttpServerRequest *serverRequest,
                                              QLinkedList<Controller *> chain)
{
    if (AppSettings::instance().traceServerRequests()) {
        uint64_t threadId = (uint64_t) QThread::currentThreadId();

        qDebug() << QString("%1: Api request from %2:%3")
                        .arg(threadId)
                        .arg(serverRequest->remoteAddress().toString())
                        .arg(serverRequest->remotePort());
        qDebug() << QString("%1: Api request method %2, path %3, query %4")
                        .arg(threadId)
                        .arg((int) serverRequest->method())
                        .arg(serverRequest->url().path())
                        .arg(serverRequest->query().toString());
        if (!serverRequest->headers().empty()) {
            for (QPair<QByteArray, QByteArray> header : serverRequest->headers()) {
                qDebug() << QString("%1: Api request header %2:%3")
                                .arg(threadId)
                                .arg(header.first)
                                .arg(header.second);
            }
        }

        if (serverRequest->method() == QHttpServerRequest::Method::Post) {
            const QByteArray reqBody = serverRequest->body();
            if (reqBody != nullptr) {
                qDebug() << QString("%1: Api request body %2").arg(threadId).arg(reqBody);
            } else {
                qDebug() << QString("%1: Api request body is empty").arg(threadId);
            }
        }
    }

    QHttpServerResponse httpResponse = QHttpServerResponse("OK",
                                                           QHttpServerResponder::StatusCode::Ok);

    try {
        if (!chain.empty()) {
            Controller *next = chain.front();
            chain.removeFirst();

            httpResponse = next->handleRequest(controller, method, serverRequest, chain);
        }
    } catch (...) {
        qDebug() << "exception while processing controllers";
    }

    QThreadStorage<AuthInfo *> threadStorage;
    if (threadStorage.hasLocalData()) {
        AuthInfo *authInfo = threadStorage.localData();
        if (authInfo != nullptr && !authInfo->getToken().isNull()
            && !authInfo->getToken().isEmpty()) {
            httpResponse.setHeader("Token", authInfo->getToken().toUtf8());

            threadStorage.setLocalData(nullptr);
        }
    }

    return httpResponse;
}

void HttpSupport::setupSsl()
{
    if (!AppSettings::instance().isUseSSL()) {
        qDebug() << "No SSL enabled";
        return;
    }

    qDebug() << "Initializing SSL";
    qDebug() << "CA: " << AppSettings::instance().getSslRootPath();
    qDebug() << "SRV: " << AppSettings::instance().getSslCertPath();
    qDebug() << "KEY: " << AppSettings::instance().getSslKeyPath();

    auto config = QSslConfiguration::defaultConfiguration();
    config.setCaCertificates(QSslCertificate::fromPath(AppSettings::instance().getSslRootPath()));

    QFile certificateFile(AppSettings::instance().getSslCertPath());
    if (certificateFile.open(QIODevice::ReadOnly | QIODevice::Text))
        config.setLocalCertificate(QSslCertificate(certificateFile.readAll(), QSsl::Pem));
    else
        qFatal("Could not open certificate file");

    QFile keyFile(AppSettings::instance().getSslKeyPath());
    if (keyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QSslKey key(keyFile.readAll(), QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
        //        QSslKey key(keyFile.readAll(), QSsl::Rsa);
        if (key.isNull())
            qFatal("Key is not valid");
        config.setPrivateKey(key);
    } else {
        qFatal("Could not open key file");
    }

    config.setPeerVerifyMode(QSslSocket::VerifyNone);

    QSslConfiguration::setDefaultConfiguration(config);
    httpServer.sslSetup(config);
}

void missingHandler(const QHttpServerRequest &request, QHttpServerResponder &&responder)
{
    qWarning() << "invalid http request" << request.url().path();
    responder.writeBody("Oh no! Invalid request!");
    responder.write(QHttpServerResponder::StatusCode::NotFound);
}

void HttpSupport::setupAfterRequest()
{
    httpServer.afterRequest([](QHttpServerResponse &&resp) {
        qDebug() << "request finished with " << resp.statusCode() << " and data " << resp.data();
        return std::move(resp);
    });
}

void HttpSupport::setupRoutes()
{
    httpServer.route("/health",
                     QHttpServerRequest::Method::Get,
                     [](const QHttpServerRequest &request) {
                         qDebug() << "Health request from " << request.remoteAddress() << ":"
                                  << request.remotePort();
                         qDebug() << "Health request from thread " << QThread::currentThreadId();
                         return QHttpServerResponse("OK", QHttpServerResponder::StatusCode::Ok);
                     });

    httpServer.route("/api/<arg>/<arg>",
                     QHttpServerRequest::Method::Post,
                     [=](const QString &controller,
                         const QString &method,
                         const QHttpServerRequest &request) {
                         qDebug() << QString("api request %1 %2 %3")
                                         .arg(controller)
                                         .arg(method)
                                         .arg(request.url().path());
                         return QtConcurrent::run(asyncExecute,
                                                  controller,
                                                  method,
                                                  &request,
                                                  QLinkedList<Controller *>(controllersChain));
                     });
}
