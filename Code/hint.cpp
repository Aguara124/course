#include "hint.h"
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include "qlayout.h"
#include "qmessagebox.h"
#include "ui_hint.h"
#include <poppler/qt6/poppler-qt6.h>

Hint::Hint(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::hint)
{
    ui->setupUi(this);
    layout = new QHBoxLayout(ui->widget);
}

Hint::~Hint()
{
    delete ui;
}

void Hint::setHint(const QString &hintFile)
{
    if (images.size() != 0) {
        images.clear();
    }

    bool isPdf = hintFile.endsWith(".pdf");
    if (isPdf) {
        std::unique_ptr<Poppler::Document> doc = Poppler::Document::load(hintFile);
        doc->setRenderHint(Poppler::Document::TextAntialiasing);
        for (int i = 0; i < doc->numPages(); i++) {
            std::unique_ptr<Poppler::Page> pdfPage = doc->page(i);
            if (pdfPage == nullptr) {
                QMessageBox messageBox(QMessageBox::Icon::Critical,
                                       "Data access",
                                       "Failed get page from PDF");
                messageBox.exec();
                return;
            }

            QImage image = pdfPage->renderToImage(72.0,
                                                  72.0,
                                                  0,
                                                  0,
                                                  pdfPage->pageSize().width(),
                                                  pdfPage->pageSize().height());
            images.push_back(image);

            if (i == 0) {
                addImageToLayout(image);
            }
        }

    } else {
        QImage image(hintFile);
        images.push_back(image);

        addImageToLayout(image);
    }

    ui->lineEdit->setText("1");
    ui->nextPB->setEnabled(images.size() > 1);
}

void Hint::setHint(const QByteArray &hintFile, bool isPdf)
{
    if (!isPdf) {
        QImage image;
        image.loadFromData(hintFile);
        images.push_back(image);

        addImageToLayout(image);

    } else {
        std::unique_ptr<Poppler::Document> doc = Poppler::Document::loadFromData(hintFile);
        if (doc != nullptr) {
            doc->setRenderHint(Poppler::Document::TextAntialiasing);
            for (int i = 0; i < doc->numPages(); i++) {
                std::unique_ptr<Poppler::Page> pdfPage = doc->page(i);
                if (pdfPage == nullptr) {
                    QMessageBox messageBox(QMessageBox::Icon::Critical,
                                           "Data access",
                                           "Failed get page from PDF");
                    messageBox.exec();
                    break;
                }

                QImage image = pdfPage->renderToImage(72.0,
                                                      72.0,
                                                      0,
                                                      0,
                                                      pdfPage->pageSize().width(),
                                                      pdfPage->pageSize().height());
                images.push_back(image);
                if (i == 0) {
                    addImageToLayout(image);
                }
            }
        }
    }
}

void Hint::addImageToLayout(const QImage &image)
{
    if (layout != nullptr && layout->count() > 0) {
        layout->removeItem(layout->takeAt(0));
    }

    QWidget *widget = new QWidget;
    widget->setLayout(new QVBoxLayout());

    QLabel *label = new QLabel;
    label->setScaledContents(true);
    label->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    label->setMinimumHeight(614);
    label->setMinimumWidth(400);
    label->setPixmap(QPixmap::fromImage(image));
    label->setAlignment(Qt::AlignCenter);
    widget->layout()->addWidget(label);

    layout->addWidget(widget, 0, Qt::AlignCenter);
}

void Hint::on_prevPB_clicked()
{
    int switchToPage = (ui->lineEdit->text()).toInt() - 1;

    addImageToLayout(images.at(switchToPage - 1));

    ui->lineEdit->setText(QString::number(switchToPage));
    if (switchToPage < 2) {
        ui->prevPB->setEnabled(false);
    }
    if (switchToPage < int(images.size()))
        ui->nextPB->setEnabled(true);
}

void Hint::on_nextPB_clicked()
{
    int switchToPage = (ui->lineEdit->text()).toInt() + 1;
    addImageToLayout(images.at(switchToPage - 1));

    ui->lineEdit->setText(QString::number(switchToPage));
    if (switchToPage > 1)
        ui->prevPB->setEnabled(true);
    if (switchToPage == int(images.size()))
        ui->nextPB->setEnabled(false);
}
