#ifndef SIMPLEREQUESTER_H
#define SIMPLEREQUESTER_H

#include <QJsonObject>
#include "curl/curl.h"
#include "dto.h"

using QStringMap = QMap<QString, QString>;

class SimpleRequester
{
private:
    CURL *curl;
    char errorBuffer[CURL_ERROR_SIZE];
    const std::string buffer;
    static int trace(CURL *handle, curl_infotype type, char *data, size_t size, void *clientp);
    static void dump(const char *text, FILE *stream, unsigned char *ptr, size_t size);

    const char *toJson(const QVariantMap &data);

    static size_t writeFunc(char *data, size_t size, size_t nmemb, std::string *buffer);
    static size_t headerFunc(char *header, size_t size, size_t nitems, void *userdata);

public:
    SimpleRequester();
    ~SimpleRequester();

    enum HttpMethod { GET, POST };

    bool sendRequest(const QString &apiStr,
                     const IDto &data,
                     const QStringMap &httpHeaders = QStringMap());

    bool sendRequest(const QString &apiStr,
                     const QVariantMap &data,
                     const QStringMap &httpHeaders = QStringMap());

    bool sendRequest(const QString &apiStr,
                     HttpMethod method = HttpMethod::POST,
                     const QByteArray &data = QByteArray(),
                     const QStringMap &httpHeaders = QStringMap());

    const QJsonObject getResult();
    const QByteArray getRawResult();
    int getStatusCode();
    const QString getErrorString();
    const QStringMap getHeaders();
};

#endif // SIMPLEREQUESTER_H
