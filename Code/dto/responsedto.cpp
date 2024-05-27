#include "responsedto.h"
#include <QJsonDocument>
#include <QJsonObject>

bool ResponseDto::getSuccess() const
{
    return success;
}

void ResponseDto::setSuccess(bool newSuccess)
{
    success = newSuccess;
}

const QString ResponseDto::getMessage() const
{
    return message;
}

void ResponseDto::setMessage(const QString &newMessage)
{
    message = newMessage;
}

ResponseDto::ResponseDto() {}

void ResponseDto::fromJson(const QByteArray &content)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);

    if (jsonError.error != QJsonParseError::ParseError::NoError) {
        qDebug() << jsonError.errorString() << " error: " << jsonError.error;
        return;
    };

    QJsonObject obj = doc.object();

    success = obj.value("success").toBool();
    message = obj.value("message").toString();
}

const QByteArray ResponseDto::toJson() const
{
    QJsonObject obj;

    obj["success"] = getSuccess();
    obj["message"] = getMessage();

    QJsonDocument jsonDoc(obj);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}
