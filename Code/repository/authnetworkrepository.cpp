#include "authnetworkrepository.h"

#include <QMessageBox>
#include <QNetworkReply>
#include <QSslKey>
#include <QtConcurrent/QtConcurrent>

#include <QHttpServerResponse>
#include "appsettings.h"
#include "authdata.h"
#include "authrequestdto.h"
#include "encryption.h"
#include "responsedto.h"
#include "simplerequester.h"

AuthNetworkRepository::AuthNetworkRepository()
{
    QSettings *settings = AppSettings::instance().getSettings();
    qDebug() << "networkRepository::init: " << settings->allKeys();
    qDebug() << "networkRepository::init: " << settings->value("Rest/apiUrls");

    QStringList apiUrls = settings->value("Rest/apiUrls").toStringList();
    if (apiUrls.isEmpty()) {
        QMessageBox messageBox(QMessageBox::Icon::Critical, "Configuration", "Missing api settings");
        messageBox.exec();

        qFatal("Incorrent configuration - missing api settings");
    }

    AuthData::instance().setApiUrl(apiUrls.size() > 1 ? selectValidApiUrl(apiUrls)
                                                      : apiUrls.first());
}

const QString AuthNetworkRepository::selectValidApiUrl(QStringList &apiUrls) const
{
    QList<QFuture<QString>> futures;
    for (QString url : apiUrls) {
        QString trimmed = url.trimmed();

        QFuture<QString> future = QtConcurrent::run([trimmed]() {
            SimpleRequester requester;
            if (requester.sendRequest(trimmed + "/auth/authenticate")
                && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
                       == QHttpServerResponse::StatusCode::Unauthorized) {
                return trimmed;
            }

            qDebug() << requester.getErrorString();
            return QString();
        });

        futures.append(future);
    }

    QFutureSynchronizer sync(futures.first());
    for (auto f : futures) {
        sync.addFuture(f);
    }

    sync.waitForFinished();

    for (auto f : futures) {
        if (f.isFinished() && !f.result().isEmpty()) {
            qDebug() << "Found valid API Url: " << f.result();
            return f.result();
        }
    }

    qDebug() << "Detection for valid API Url FAIL";
    return apiUrls.first();
}

bool AuthNetworkRepository::authenticate(const QString &login,
                                     const QString &password,
                                     const Role role,
                                     const QString &ipAddr)
{
    (void) ipAddr;

    AuthRequestDTO authDto;
    authDto.setLogin(login);
    authDto.setPassword(password);
    authDto.setRole(role);

    SimpleRequester requester;
    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/auth/authenticate",
                                         authDto);

    if (!success
        || static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               != QHttpServerResponse::StatusCode::Ok
        || !requester.getHeaders().contains("Token")) {
        return false;
    }

    QStringMap headers = requester.getHeaders();
    const QString token = headers["Token"];

    QList<QByteArray> tokenList = EncryptionHelper::decrypt(QByteArray::fromBase64(token.toUtf8()))
                                      .split('-');
    if (tokenList.size() != 4 || QString(tokenList.at(1).toLower()) != login.toLower()) {
        return false;
    }

    const IAuthRepository::Role resultRole = static_cast<IAuthRepository::Role>(
        QString(tokenList.at(2)).toInt());
    qDebug() << "Auth token: " << token << " Role: " << resultRole;

    AuthData::instance().setToken(token);
    AuthData::instance().setRole(resultRole);
    AuthData::instance().setLogin(login);

    return true;
}

bool AuthNetworkRepository::checkToken(const QString &token, const QString &ipAddr)
{
    (void) ipAddr;
    //stub
    qDebug() << "checkToken on network repository :): " << token;
    return false;
}

bool AuthNetworkRepository::checkLogin(const QString &login)
{
    AuthRequestDTO authDto;
    authDto.setLogin(login);

    SimpleRequester requester;
    bool success = requester.sendRequest(AuthData::instance().getApiUrl() + "/auth/check", authDto);

    if (success
        && static_cast<QHttpServerResponse::StatusCode>(requester.getStatusCode())
               == QHttpServerResponse::StatusCode::Ok) {
        ResponseDto responseDto(requester.getRawResult());
        return responseDto.getSuccess();
    }

    return true;
}
