#ifndef AUTHREQUESTDTO_H
#define AUTHREQUESTDTO_H

#include "dto.h"
#include "authrepository.h"

class AuthRequestDTO : public IDto
{
private:
    QString login;
    QString password;
    IAuthRepository::Role role;

public:
    AuthRequestDTO() {}
    AuthRequestDTO(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }

public:
    void fromJson(const QByteArray &content) override;
    const QByteArray toJson() const override;

    const QString getLogin() const;
    void setLogin(const QString &newLogin);
    const QString getPassword() const;
    void setPassword(const QString &newPassword);
    IAuthRepository::Role getRole() const;
    void setRole(IAuthRepository::Role newRole);
};

#endif // AUTHREQUESTDTO_H
