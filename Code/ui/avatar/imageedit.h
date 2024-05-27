#ifndef IMAGEEDIT_H
#define IMAGEEDIT_H

#include <QBoxLayout>
#include <QDialog>
#include <QLabel>
#include <qtmaterialflatbutton.h>

class ImageCropper;

class ImageEdit : public QDialog
{
    Q_OBJECT

public:
    explicit ImageEdit(QWidget *parent = nullptr);
    ~ImageEdit();

    void processImage(const QString &file);
    QImage getImage() { return resultImage; }

private slots:
    void crop();

private:
    QBoxLayout *layout;
    QImage resultImage;
    std::unique_ptr<ImageCropper> imageCropper;
    std::unique_ptr<QLabel> croppedImage;

    std::unique_ptr<QtMaterialFlatButton> cropBtn;
    std::unique_ptr<QtMaterialFlatButton> acceptBtn;
    std::unique_ptr<QtMaterialFlatButton> cancelBtn;
};

#endif // IMAGEEDIT_H
