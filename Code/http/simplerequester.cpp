#include "simplerequester.h"
#include <QFile>
#include <QHttpHeaders>
#include <QHttpServerResponse>
#include <QJsonDocument>
#include <QSslConfiguration>
#include <QSslKey>

#include "appsettings.h"

SimpleRequester::SimpleRequester()
{
    curl = curl_easy_init();
    if (curl != nullptr) {
        errorBuffer[0] = 0;
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);

        if (AppSettings::instance().isUseSSL()
            && AppSettings::instance().getMode() == AppSettings::AppMode::REMOTE) {
            QFile caCert(AppSettings::instance().getSslRootPath());
            if (caCert.open(QIODevice::ReadOnly | QIODevice::Text)) {
                struct curl_blob blob;
                const char *certData = caCert.readAll().data();
                blob.data = (void *) certData;
                blob.len = strlen(certData);
                blob.flags = CURL_BLOB_COPY;
                curl_easy_setopt(curl, CURLOPT_CAINFO_BLOB, &blob);
                caCert.close();
            }

            curl_easy_setopt(curl, CURLOPT_SSLCERT, AppSettings::instance().getSslCertPath());
            curl_easy_setopt(curl, CURLOPT_SSLKEY, AppSettings::instance().getSslKeyPath());

            curl_easy_setopt(curl, CURLOPT_KEYPASSWD, AppSettings::instance().getSslKeyPassword());
            curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
        }

        if (AppSettings::instance().traceClientRequests()) {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, trace);
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerFunc);
        }

        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 10000);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 300000);
    }
}

SimpleRequester::~SimpleRequester()
{
    if (curl != nullptr) {
        curl_easy_cleanup(curl);
    }
}

size_t SimpleRequester::writeFunc(char *data, size_t size, size_t nmemb, std::string *buffer)
{
    size_t result = 0;

    if (buffer != NULL) {
        buffer->append(data, size * nmemb);
        result = size * nmemb;
    }
    return result;
}

const char *SimpleRequester::toJson(const QVariantMap &data)
{
    if (data.empty()) {
        return "{}";
    }

    QJsonDocument jsonDoc(QJsonObject::fromVariantMap(data));
    return QString(jsonDoc.toJson(QJsonDocument::Compact)).toUtf8().toStdString().c_str();
}

bool SimpleRequester::sendRequest(const QString &apiStr,
                                  HttpMethod method,
                                  const QByteArray &data,
                                  const QStringMap &httpHeaders)
{
    if (curl == nullptr) {
        return false;
    }

    qDebug() << "quering " << apiStr << "with " << data;

    struct curl_slist *headers = NULL;
    curl_easy_setopt(curl, CURLOPT_URL, apiStr.toStdString().c_str());

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method == HttpMethod::GET ? "GET" : "POST");
    const QByteArray utf8Data = QString(data).toUtf8();
    if (method == HttpMethod::POST) {
        headers = curl_slist_append(headers, "Content-Type: application/json; charset: utf-8");

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, utf8Data.data());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, utf8Data.size());

    } else if (method == HttpMethod::GET) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        headers = curl_slist_append(headers, "Accept: application/json");
    }

    headers = curl_slist_append(headers, "Accept: application/json");

    if (!httpHeaders.empty()) {
        for (auto [key, value] : httpHeaders.asKeyValueRange()) {
            headers = curl_slist_append(headers, QString("%1: %2").arg(key).arg(value).toUtf8());
        }
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // выполнить запрос
    CURLcode curlResult = curl_easy_perform(curl);
    return curlResult == CURLE_OK;
}

bool SimpleRequester::sendRequest(const QString &apiStr,
                                  const QVariantMap &data,
                                  const QStringMap &httpHeaders)
{
    return sendRequest(apiStr, HttpMethod::POST, toJson(data), httpHeaders);
}

bool SimpleRequester::sendRequest(const QString &apiStr,
                                  const IDto &data,
                                  const QStringMap &httpHeaders)
{
    return sendRequest(apiStr, HttpMethod::POST, data.toJson(), httpHeaders);
}

const QJsonObject SimpleRequester::getResult()
{
    if (curl != nullptr && getStatusCode() == (int) QHttpServerResponse::StatusCode::Ok) {
        return QJsonDocument::fromJson(QString(buffer.c_str()).toUtf8()).object();
    }

    return QJsonObject();
}

const QByteArray SimpleRequester::getRawResult()
{
    if (curl != nullptr && getStatusCode() == (int) QHttpServerResponse::StatusCode::Ok) {
        return QString(buffer.c_str()).toUtf8();
    }

    return QByteArray();
}

int SimpleRequester::getStatusCode()
{
    if (curl != nullptr) {
        long responseCode;
        if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode) == CURLE_OK) {
            return responseCode;
        }
    }

    return -1;
}

const QString SimpleRequester::getErrorString()
{
    return QString(errorBuffer);
}

const QStringMap SimpleRequester::getHeaders()
{
    struct curl_header *prev = NULL;
    struct curl_header *h;
    QStringMap headers;

    if (curl != nullptr) {
        while ((h = curl_easy_nextheader(curl, CURLH_HEADER, 0, prev))) {
            headers.insert(h->name, h->value);
            prev = h;
        }
    }

    return headers;
}

void SimpleRequester::dump(const char *text, FILE *stream, unsigned char *ptr, size_t size)
{
    size_t i;
    size_t c;
    unsigned int width = 0x10;

    fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n", text, (long) size, (long) size);

    for (i = 0; i < size; i += width) {
        fprintf(stream, "%4.4lx: ", (long) i);

        for (c = 0; c < width; c++) {
            if (i + c < size)
                fprintf(stream, "%02x ", ptr[i + c]);
            else
                fputs("   ", stream);
        }

        /* show data on the right */
        for (c = 0; (c < width) && (i + c < size); c++) {
            char x = (ptr[i + c] >= 0x20 && ptr[i + c] < 0x80) ? ptr[i + c] : '.';
            fputc(x, stream);
        }

        fputc('\n', stream); /* newline */
    }
}

int SimpleRequester::trace(CURL *handle, curl_infotype type, char *data, size_t size, void *clientp)
{
    const char *text = nullptr;
    (void) handle; /* prevent compiler warning */
    (void) clientp;

    FILE *stream = fopen("course.log", "a");
    if (stream == nullptr) {
        stream = stderr;
    }

    switch (type) {
    case CURLINFO_TEXT:
        fputs("== Info: ", stream);
        fwrite(data, size, 1, stream);
        break;
    case CURLINFO_HEADER_OUT:
        text = "=> Send header";
        break;
    case CURLINFO_DATA_OUT:
        text = "=> Send data";
        break;
    case CURLINFO_SSL_DATA_OUT:
        text = "=> Send SSL data";
        break;
    case CURLINFO_HEADER_IN:
        text = "<= Recv header";
        break;
    case CURLINFO_DATA_IN:
        text = "<= Recv data";
        break;
    case CURLINFO_SSL_DATA_IN:
        text = "<= Recv SSL data";
        break;
    default: /* in case a new one is introduced to shock us */
        text = QString("Unknown type %1").arg(type).toStdString().c_str();
        return 0;
    }

    if (AppSettings::instance().dumpClientRequests()) {
        dump(text, stream, (unsigned char *) data, size);
    }

    if (stream != stderr) {
        fclose(stream);
    }

    return 0;
}

size_t SimpleRequester::headerFunc(char *header, size_t size, size_t nitems, void *userdata)
{
    (void) userdata;

    FILE *stream = fopen("course.log", "a");
    if (stream == nullptr) {
        stream = stderr;
    }

    fputs("== Header: ", stream);
    fwrite(header, size, nitems, stream);

    if (stream != stderr) {
        fclose(stream);
    }

    return size * nitems;
}
