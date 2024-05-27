#ifndef HINT_H
#define HINT_H

#include <QDialog>
#include <QBoxLayout>

namespace Ui {
class hint;
}

class Hint : public QDialog
{
    Q_OBJECT

public:
    explicit Hint(QWidget *parent = nullptr);
    ~Hint();
    void setHint(const QString &hintFile);
    void setHint(const QByteArray &hintFile, bool isPdf);

private slots:
    void on_prevPB_clicked();

    void on_nextPB_clicked();

private:
    Ui::hint *ui;
    QBoxLayout* layout;

    QList<QImage> images;

    void addImageToLayout(const QImage &image);
};

#endif // HINT_H
