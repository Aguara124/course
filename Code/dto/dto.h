#ifndef DTO_H
#define DTO_H

#include <QString>

class IDto
{
public:
    virtual ~IDto() {}
    virtual void fromJson(const QByteArray &content) = 0;
    virtual const QByteArray toJson() const = 0;
};

#endif // DTO_H
