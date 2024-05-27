#ifndef AUTHREPOSITORY_H
#define AUTHREPOSITORY_H

#include <QString>

class IAuthRepository
{
public:
    enum Role { UNKNOWN = 0, STUDENT, TA, TEACHER };

    virtual ~IAuthRepository() {}
    virtual bool authenticate(const QString &login,
                              const QString &password,
                              const Role role,
                              const QString &ipAddr = QString())
        = 0;
    virtual bool checkToken(const QString &token, const QString &ipAddr) = 0;

    /**
     * @brief checkLogin - check login on existence
     * @param login - checked login
     * @return bool - true login exist, false login name is free 
     */
    virtual bool checkLogin(const QString &login) = 0;
};

#endif  // AUTHREPOSITORY_H
