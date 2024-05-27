#include "requester.h"

const QString Requester::httpTemplate = "http://%1:%2/api/%3";
const QString Requester::httpsTemplate = "https://%1:%2/api/%3";
const QString Requester::KEY_QNETWORK_REPLY_ERROR = "QNetworkReplyError";
const QString Requester::KEY_QNETWORK_REPLY_ERROR_VALUE =
    "QNetworkReplyErrorValue";
const QString Requester::KEY_CONTENT_NOT_FOUND = "ContentNotFoundError";

Requester::Requester(QObject *parent) : QObject(parent) {
  manager = new QNetworkAccessManager(this);
}

void Requester::initRequester(const QString &host, quint16 port,
                              QSslConfiguration *value) {
  this->host = host;
  this->port = port;
  sslConfig = value;
  if (sslConfig != nullptr)
    pathTemplate = httpsTemplate;
  else
    pathTemplate = httpTemplate;
}

void Requester::sendRequest(const QString &apiStr,
                            const handleFunc &funcSuccess,
                            const handleFunc &funcError, Requester::Type type,
                            const QVariantMap &data,
                            const QMap<QString, QString> &httpHeaders) {
  QNetworkRequest request = createRequest(apiStr, httpHeaders);

  QNetworkReply *reply;
  switch (type) {
    case Type::POST: {
      QByteArray postDataByteArray = variantMapToJson(data);
      reply = manager->post(request, postDataByteArray);
      break;
    }
    case Type::GET: {
      reply = manager->get(request);
      break;
    }
    case Type::DELET: {
      if (data.isEmpty())
        reply = manager->deleteResource(request);
      else
        reply = sendCustomRequest(manager, request, "DELETE", data);
      break;
    }
    case Type::PATCH: {
      reply = sendCustomRequest(manager, request, "PATCH", data);
      break;
    }
    default:
      reply = nullptr;
      Q_ASSERT(false);
  }

  connect(reply, &QNetworkReply::finished, this,
          [this, funcSuccess, funcError, reply]() {
            QJsonObject obj = parseReply(reply);

            if (onFinishRequest(reply)) {
              if (funcSuccess != nullptr) funcSuccess(obj);
            } else {
              if (funcError != nullptr) {
                handleQtNetworkErrors(reply, obj);
                funcError(obj);
              }
            }
            reply->close();
            reply->deleteLater();
          });
}

void Requester::sendMulishGetRequest(
    const QString
        &apiStr,  // а ничего что здесь нигде не проверяется func != nullptr?
    const handleFunc &funcSuccess, const handleFunc &funcError,
    const finishFunc &funcFinish, const QStringMap &httpHeaders) {
  QNetworkRequest request = createRequest(apiStr, httpHeaders);
  //    QNetworkReply *reply;
  qInfo() << "GET REQUEST " << request.url().toString() << "\n";
  auto reply = manager->get(request);

  connect(reply, &QNetworkReply::finished, this,
          [this, funcSuccess, funcError, funcFinish, reply]() {
            QJsonObject obj = parseReply(reply);
            if (onFinishRequest(reply)) {
              if (funcSuccess != nullptr) funcSuccess(obj);
              QString nextPage = obj.value("next").toString();
              if (!nextPage.isEmpty()) {
                QStringList apiMethodWithPage = nextPage.split("api/");
                sendMulishGetRequest(apiMethodWithPage.value(1), funcSuccess,
                                     funcError, funcFinish);
              } else {
                if (funcFinish != nullptr) funcFinish();
              }
            } else {
              handleQtNetworkErrors(reply, obj);
              if (funcError != nullptr) funcError(obj);
            }
            reply->close();
            reply->deleteLater();
          });
}

QString Requester::getToken() const { return token; }

void Requester::setToken(const QString &value) { token = value; }

QByteArray Requester::variantMapToJson(QVariantMap data) {
  QJsonDocument postDataDoc = QJsonDocument::fromVariant(data);
  QByteArray postDataByteArray = postDataDoc.toJson(QJsonDocument::JsonFormat::Compact);

  return postDataByteArray;
}

QNetworkRequest Requester::createRequest(const QString &apiStr,
                                         const QStringMap &httpHeaders) {
  QNetworkRequest request;
  QString url = pathTemplate.arg(host).arg(port).arg(apiStr);
  request.setUrl(QUrl(url));

  request.setRawHeader("Content-Type", "application/json");

  if (!httpHeaders.empty()) {
    for (auto [key, value] : httpHeaders.asKeyValueRange()) {
      request.setRawHeader(key.toUtf8(), value.toUtf8());
    }
  }

  if (!token.isEmpty())
    request.setRawHeader("Authorization",
                         QString("token %1").arg(token).toUtf8());

  if (sslConfig != nullptr) request.setSslConfiguration(*sslConfig);

  return request;
}

QNetworkReply *Requester::sendCustomRequest(QNetworkAccessManager *manager,
                                            QNetworkRequest &request,
                                            const QString &type,
                                            const QVariantMap &data) {
  request.setRawHeader("HTTP", type.toUtf8());
  QByteArray postDataByteArray = variantMapToJson(data);
  QBuffer *buff = new QBuffer;
  buff->setData(postDataByteArray);
  buff->open(QIODevice::ReadOnly);
  QNetworkReply *reply =
      manager->sendCustomRequest(request, type.toUtf8(), buff);
  buff->setParent(reply);
  return reply;
}

QJsonObject Requester::parseReply(QNetworkReply *reply) {
  QJsonObject jsonObj;
  QJsonDocument jsonDoc;
  QJsonParseError parseError;
  auto replyText = reply->readAll();
  jsonDoc = QJsonDocument::fromJson(replyText, &parseError);
  if (parseError.error != QJsonParseError::NoError) {
    qDebug() << replyText;
    qWarning() << "Json parse error: " << parseError.errorString();
  } else {
    if (jsonDoc.isObject())
      jsonObj = jsonDoc.object();
    else if (jsonDoc.isArray())
      jsonObj["non_field_errors"] = jsonDoc.array();
  }
  return jsonObj;
}

bool Requester::onFinishRequest(QNetworkReply *reply) {
  auto replyError = reply->error();
  if (replyError == QNetworkReply::NoError) {
    int code =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if ((code >= 200) && (code < 300)) {
      return true;
    }
  }
  return false;
}

void Requester::handleQtNetworkErrors(QNetworkReply *reply, QJsonObject &obj) {
  auto replyError = reply->error();
  obj[KEY_QNETWORK_REPLY_ERROR_VALUE] = replyError;
  if (!(replyError == QNetworkReply::NoError ||
        replyError == QNetworkReply::ContentNotFoundError ||
        replyError == QNetworkReply::ContentAccessDenied ||
        replyError == QNetworkReply::ProtocolInvalidOperationError)) {
    qDebug() << reply->error();
    obj[KEY_QNETWORK_REPLY_ERROR] = reply->errorString();
  } else if (replyError == QNetworkReply::ContentNotFoundError)
    obj[KEY_CONTENT_NOT_FOUND] = reply->errorString();
}
