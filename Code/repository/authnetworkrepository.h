#ifndef AUTHNETWORKREPOSITORY_H
#define AUTHNETWORKREPOSITORY_H

#include "authdata.h"
#include "authrepository.h"
#include "simplerequester.h"

class AuthNetworkRepository : public IAuthRepository
{
private:
    const QString selectValidApiUrl(QStringList &apiUrls) const;

public:
    AuthNetworkRepository();

public:
    virtual bool authenticate(const QString &login,
                              const QString &password,
                              const Role role,
                              const QString &ipAddr = QString());
    bool checkToken(const QString &token, const QString &ipAddr) override;
    bool checkLogin(const QString &login) override;
};

#endif  // AUTHNETWORKREPOSITORY_H
