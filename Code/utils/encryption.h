#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QByteArray>


class EncryptionHelper {

private:
    const static QByteArray deencode(const QByteArray &source, bool encrypt);

public:
    const static QByteArray encrypt(const QByteArray &source) { return deencode(source, true); }

    const static QByteArray decrypt(const QByteArray &source) { return deencode(source, false); }
};


#endif // ENCRYPTION_H
