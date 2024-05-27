#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

class QByteArray;
class QImage;
class QString;
class QIcon;

#include <QSize>

class ImageUtils
{
public:
    ImageUtils();

    static QByteArray imageToByteArray(const QImage &image);
    static QImage byteArrayToImage(const QByteArray &arr);
    static QIcon makeShadedIcon(const QString &iconPath,
                                int alpha = 75,
                                const QSize &iconSize = QSize(24, 24));
};

#endif // IMAGEUTILS_H
