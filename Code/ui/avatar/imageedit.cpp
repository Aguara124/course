#include "imageedit.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include "imagecropper.h"
#include "qlayout.h"
#include "qmessagebox.h"
#include "qtmaterialflatbutton.h"

ImageEdit::ImageEdit(QWidget *parent)
    : QDialog(parent)
    , imageCropper(new ImageCropper(this))
    , croppedImage(new QLabel(this))
    , cropBtn(new QtMaterialFlatButton("Crop", this))
    , acceptBtn(new QtMaterialFlatButton("Accept", this))
    , cancelBtn(new QtMaterialFlatButton("Cancel", this))

{
    layout = new QVBoxLayout;
}

ImageEdit::~ImageEdit()
{
}

void ImageEdit::processImage(const QString &file)
{
    imageCropper->resize(600, 600);
    imageCropper->setProportion(QSize(4, 4));
    imageCropper->setImage(QPixmap(file));
    imageCropper->setBackgroundColor(Qt::lightGray);
    imageCropper->setCroppingRectBorderColor(Qt::magenta);
    imageCropper->setProportionFixed(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(imageCropper.get());
    layout->addWidget(cropBtn.get());
    layout->addWidget(croppedImage.get(), 0, Qt::AlignCenter);

    connect(cropBtn.get(), SIGNAL(clicked()), this, SLOT(crop()));
    cropBtn->setRole(Material::Role::Default);
    connect(acceptBtn.get(), SIGNAL(clicked()), this, SLOT(accept()));
    acceptBtn->setRole(Material::Role::Primary);
    connect(cancelBtn.get(), SIGNAL(clicked()), this, SLOT(reject()));
    cancelBtn->setRole(Material::Role::Default);

    QHBoxLayout *buttonLayout = new QHBoxLayout;

    acceptBtn->setDisabled(true);

    buttonLayout->addWidget(acceptBtn.get());
    buttonLayout->addWidget(cancelBtn.get());

    layout->addLayout(buttonLayout);
}

void ImageEdit::crop()
{
    croppedImage->setPixmap(imageCropper->cropImage());
    resultImage = imageCropper->cropImage().toImage();

    acceptBtn->setDisabled(false);
}
