#ifndef REQUESTER_H
#define REQUESTER_H

#include <QBuffer>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <functional>

using QStringMap = QMap<QString, QString>;

class Requester : public QObject {
  Q_OBJECT
 public:
  typedef std::function<void(const QJsonObject &)> handleFunc;
  typedef std::function<void()> finishFunc;

  static const QString KEY_QNETWORK_REPLY_ERROR;
  static const QString KEY_QNETWORK_REPLY_ERROR_VALUE;
  static const QString KEY_CONTENT_NOT_FOUND;

  enum class Type { POST, GET, PATCH, DELET };

  explicit Requester(QObject *parent = 0);

  void initRequester(const QString &host, quint16 port,
                     QSslConfiguration *value);

  void sendRequest(const QString &apiStr, const handleFunc &funcSuccess,
                   const handleFunc &funcError, Type type = Type::GET,
                   const QVariantMap &data = QVariantMap(),
                   const QStringMap &httpHeaders = QStringMap());

  void sendMulishGetRequest(const QString &apiStr,
                            const handleFunc &funcSuccess,
                            const handleFunc &funcError,
                            const finishFunc &funcFinish,
                            const QStringMap &httpHeaders = QStringMap());

  QString getToken() const;
  void setToken(const QString &value);

 private:
  static const QString httpTemplate;
  static const QString httpsTemplate;

  QString host;
  quint16 port;
  QString token;
  QSslConfiguration *sslConfig;

  QString pathTemplate;

  QByteArray variantMapToJson(QVariantMap data);

  QNetworkRequest createRequest(const QString &apiStr,
                                const QStringMap &httpHeaders);

  QNetworkReply *sendCustomRequest(QNetworkAccessManager *manager,
                                   QNetworkRequest &request,
                                   const QString &type,
                                   const QVariantMap &data);

  QJsonObject parseReply(QNetworkReply *reply);

  bool onFinishRequest(QNetworkReply *reply);

  void handleQtNetworkErrors(QNetworkReply *reply, QJsonObject &obj);
  QNetworkAccessManager *manager;

 signals:
  void networkError();

 public slots:
};

#endif  // REQUESTER_H
