#ifndef PROFILEDITDIALOG_H
#define PROFILEDITDIALOG_H

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <qtmaterialavatar.h>
#include <qtmaterialcheckbox.h>
#include <qtmaterialdialog.h>
#include <qtmaterialiconbutton.h>
#include <qtmaterialraisedbutton.h>
#include <qtmaterialselectbox.h>
#include <qtmaterialtextfield.h>

#include "avatar/imageedit.h"

class QImage;

class ProfileEditDialog : public QtMaterialDialog
{
    Q_OBJECT
public:
    explicit ProfileEditDialog(QWidget *parent = nullptr);
    void setupForm(const QImage &avatar);
    const QImage getImage() { return avatar->image(); }

protected slots:
    void changeAvatar();
    void acceptAvatar();

    void clickCancel();
    void clickApply();
    void clickHelp();

signals:
    void doApply();
    void doCancel();
    void doHelp();

private:
    QVBoxLayout *mainLayout;

    QtMaterialAvatar *avatar;

    QtMaterialTextField *oldPassword;
    QtMaterialTextField *newPassword;
    QtMaterialTextField *confirmPassword;

    QtMaterialRaisedButton *applyButton;
    QtMaterialRaisedButton *cancelButton;

    QtMaterialIconButton *helpButton;
    std::shared_ptr<ImageEdit> imageEdit;

private:
    static bool meetPasswordCondition(const QString &password);
};

#endif // ProfileEditDialog_H
