#include <openssl/aes.h>
#include <openssl/evp.h>
#include "encryption.h"


uchar const KEY_CRYPT[] = "-UKfdjdskf--**fsnx";
uchar const KEY_IV[] = "Compl3xES3quence";

const QByteArray EncryptionHelper::deencode(const QByteArray &source, bool encrypt)
{
    int out_len;
    EVP_CIPHER_CTX * ctx = EVP_CIPHER_CTX_new();

    EVP_CipherInit(ctx, EVP_aes_128_cbc(), KEY_CRYPT, KEY_IV , encrypt);
    int blocksize = EVP_CIPHER_CTX_block_size(ctx);
    QByteArray cipher_buf;
    cipher_buf.resize(source.length() + blocksize);

    EVP_CipherUpdate(ctx,
                     reinterpret_cast<uchar *>(cipher_buf.data()),
                     &out_len,
                     reinterpret_cast<const uchar *>(source.data()),
                     source.length());
    QByteArray output(cipher_buf.data(), out_len);

    EVP_CipherFinal(ctx, reinterpret_cast<uchar *>(cipher_buf.data()), &out_len);
    output.append(cipher_buf.data(), out_len);

    EVP_CIPHER_CTX_free(ctx);

    return output;
}
