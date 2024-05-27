#ifndef RESPONSEDTO_H
#define RESPONSEDTO_H

#include "dto.h"

class ResponseDto : public IDto
{
private:
    bool success;
    QString message;

public:
    ResponseDto();
    ResponseDto(bool success) { this->success = success; }
    ResponseDto(bool success, const QString &message)
    {
        this->success = success;
        this->message = message;
    }

    ResponseDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }

public:
    void fromJson(const QByteArray &content) override;
    const QByteArray toJson() const override;
    bool getSuccess() const;
    void setSuccess(bool newSuccess);
    const QString getMessage() const;
    void setMessage(const QString &newMessage);
};

#endif // RESPONSEDTO_H
