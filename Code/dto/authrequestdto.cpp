#include "authrequestdto.h"
#include <QJsonDocument>
#include <QJsonObject>

const QString AuthRequestDTO::getLogin() const
{
    return login;
}

void AuthRequestDTO::setLogin(const QString &newLogin)
{
    login = newLogin;
}

const QString AuthRequestDTO::getPassword() const
{
    return password;
}

void AuthRequestDTO::setPassword(const QString &newPassword)
{
    password = newPassword;
}

IAuthRepository::Role AuthRequestDTO::getRole() const
{
    return role;
}

void AuthRequestDTO::setRole(IAuthRepository::Role newRole)
{
    role = newRole;
}

void AuthRequestDTO::fromJson(const QByteArray &json)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(json, &jsonError);

    if (jsonError.error != QJsonParseError::ParseError::NoError) {
        qDebug() << jsonError.errorString() << " error: " << jsonError.error;
        return;
    };

    QJsonObject obj = doc.object();

    login = obj.value("login").toString();
    password = obj.value("password").toString();
    role = static_cast<IAuthRepository::Role>(obj.value("role").toInt());
}

const QByteArray AuthRequestDTO::toJson() const
{
    QJsonObject obj;

    obj["login"] = getLogin();
    obj["password"] = getPassword();
    obj["role"] = getRole();

    QJsonDocument jsonDoc(obj);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}
