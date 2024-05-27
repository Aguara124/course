#ifndef TOKENREPOSITORY_H
#define TOKENREPOSITORY_H

#include "model/token.h"

class ITokenRepository
{
public:
    virtual ~ITokenRepository() {}

    virtual const Token findByToken(const QString &tokenValue) const = 0;
    virtual bool insert(const Token &token) = 0;
    virtual bool clearExpired() = 0;
};

#endif // TOKENREPOSITORY_H
