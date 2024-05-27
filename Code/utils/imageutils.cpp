#include "imageutils.h"
#include <QByteArray>
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QTemporaryFile>

ImageUtils::ImageUtils() {}

QByteArray ImageUtils::imageToByteArray(const QImage &image)
{
    try {
        QString tmpFileName;
        QTemporaryFile tmpFile;
        if (tmpFile.open()) {
            tmpFileName = tmpFile.fileName();
            tmpFile.close();
        }

        image.save(tmpFileName, "PNG");

        QByteArray result;
        QFile file(tmpFileName);
        if (file.open(QIODevice::ReadOnly)) {
            result = file.readAll();
            file.close();
        }

        return result;
    } catch (...) {
        return QByteArray();
    }
}

QImage ImageUtils::byteArrayToImage(const QByteArray &arr)
{
    if (arr.isNull() || arr.isEmpty()) {
        return QImage();
    }

    QImage img;
    img.loadFromData(arr);

    return img;
}

QIcon ImageUtils::makeShadedIcon(const QString &iconPath, int alpha, const QSize &iconSize)
{
    QPixmap pixmap = QIcon(iconPath).pixmap(iconSize);
    QPainter p(&pixmap);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(QRect(0, 0, iconSize.width(), iconSize.height()), QColor(255, 255, 255, alpha));
    p.end();

    QIcon icon;
    icon.addPixmap(pixmap);
    return icon;
}
