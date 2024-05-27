#ifndef TOKENDBREPOSITORY_H
#define TOKENDBREPOSITORY_H

#include "tokenrepository.h"

class TokenDbRepository : public ITokenRepository
{
public:
    TokenDbRepository();
public:
    const Token findByToken(const QString &tokenValue) const override;
    bool insert(const Token &token) override;
    bool clearExpired() override;
};

#endif // TOKENDBREPOSITORY_H
