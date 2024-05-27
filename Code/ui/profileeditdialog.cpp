#include "ProfileEditDialog.h"
#include <QFileDialog>
#include <QRegularExpression>
#include "QRegularExpressionValidator"
#include "assistantrepository.h"
#include "authdata.h"
#include "avatar/imageCropper.h"
#include "avatar/imageedit.h"
#include "fcsrepository.h"
#include "imageutils.h"
#include "mainappwindow.h"
#include "repositorymanager.h"
#include "studentrepository.h"
#include "teacherrepository.h"

ProfileEditDialog::ProfileEditDialog(QWidget *parent)
    : QtMaterialDialog(parent)
    , mainLayout(new QVBoxLayout)
    , avatar(new QtMaterialAvatar(this))
    , oldPassword(new QtMaterialTextField(this))
    , newPassword(new QtMaterialTextField(this))
    , confirmPassword(new QtMaterialTextField(this))
    , applyButton(new QtMaterialRaisedButton(this))
    , cancelButton(new QtMaterialRaisedButton(this))
    , helpButton(
          new QtMaterialIconButton(QIcon(":/icons/icons/action/svg/production/ic_help_24px.svg"),
                                   this))
    , imageEdit(new ImageEdit)
{
}

void ProfileEditDialog::setupForm(const QImage &avatarImage)
{
    mainLayout->addSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    avatar->setImage(avatarImage);
    avatar->setSize(128);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addSpacing(20);
    hBoxLayout->addWidget(avatar);

    connect(avatar, SIGNAL(clicked()), this, SLOT(changeAvatar()));

    QVBoxLayout *loginGroupVBoxLayout = new QVBoxLayout;

    oldPassword->setLabel("Old Password");
    oldPassword->setMinimumWidth(200);
    oldPassword->setMaximumWidth(200);
    oldPassword->setMaximumHeight(50);
    oldPassword->setPlaceholderText("Enter password...");
    oldPassword->setEchoMode(QLineEdit::EchoMode::Password);

    newPassword->setLabel("New Password");
    newPassword->setMinimumWidth(200);
    newPassword->setMaximumWidth(200);
    newPassword->setMaximumHeight(50);
    newPassword->setPlaceholderText("Enter password...");
    newPassword->setEchoMode(QLineEdit::EchoMode::Password);

    confirmPassword->setLabel("Password confirmation");
    confirmPassword->setMinimumWidth(200);
    confirmPassword->setMaximumWidth(200);
    confirmPassword->setMaximumHeight(50);
    confirmPassword->setPlaceholderText("Enter password...");
    confirmPassword->setEchoMode(QLineEdit::EchoMode::Password);

    loginGroupVBoxLayout->addSpacing(10);
    loginGroupVBoxLayout->addWidget(oldPassword);
    loginGroupVBoxLayout->addSpacing(50);
    loginGroupVBoxLayout->addWidget(newPassword);
    loginGroupVBoxLayout->addWidget(confirmPassword);

    hBoxLayout->addSpacing(20);
    hBoxLayout->addLayout(loginGroupVBoxLayout);

    mainLayout->addLayout(hBoxLayout);
    mainLayout->addSpacing(30);

    QHBoxLayout *buttonHBoxLayout = new QHBoxLayout;

    applyButton->setText("Apply");
    applyButton->setMinimumWidth(100);
    applyButton->setMaximumWidth(150);
    applyButton->setHaloVisible(true);
    applyButton->setRole(Material::Role::Primary);
    applyButton->useThemeColors();
    applyButton->setOverlayStyle(Material::OverlayStyle::TintedOverlay);
    applyButton->setRippleStyle(Material::RippleStyle::PositionedRipple);
    applyButton->setBackgroundColor(QColor(160, 198, 232));

    buttonHBoxLayout->addWidget(applyButton, 1, Qt::AlignRight | Qt::AlignVCenter);

    cancelButton->setText("Cancel");
    cancelButton->setMinimumWidth(100);
    cancelButton->setMaximumWidth(150);
    cancelButton->setHaloVisible(true);
    cancelButton->setRole(Material::Role::Default);
    cancelButton->useThemeColors();
    cancelButton->setOverlayStyle(Material::OverlayStyle::TintedOverlay);
    cancelButton->setRippleStyle(Material::RippleStyle::PositionedRipple);
    cancelButton->setBackgroundColor(QColor(160, 198, 232));

    buttonHBoxLayout->addWidget(cancelButton, 1, Qt::AlignLeft | Qt::AlignVCenter);

    mainLayout->addLayout(buttonHBoxLayout);

    helpButton->setIconSize(QSize(16, 16));
    helpButton->setOpacity(0.5);

    mainLayout->addWidget(helpButton, 1, Qt::AlignRight | Qt::AlignBottom);

    setWindowLayout(mainLayout);

    connect(applyButton, SIGNAL(clicked()), this, SLOT(clickApply()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(clickCancel()));
    connect(helpButton, SIGNAL(clicked()), this, SLOT(clickHelp()));
}

void ProfileEditDialog::changeAvatar()
{
    QString userFile = QFileDialog::getOpenFileName(this,
                                                    "Open data file",
                                                    AppSettings::instance().getLastDirectory(),
                                                    "Images (*.png *.xpm *.jpg *.pdf)");
    if (!userFile.isEmpty()) {
        AppSettings::instance().setLastDirectory(userFile);
        imageEdit = std::shared_ptr<ImageEdit>(new ImageEdit);
        imageEdit->processImage(userFile);

        connect(imageEdit.get(), SIGNAL(accepted()), this, SLOT(acceptAvatar()));

        imageEdit->show();
        imageEdit->exec();
    }
}

void ProfileEditDialog::acceptAvatar()
{
    avatar->setImage(imageEdit->getImage());
}

bool ProfileEditDialog::meetPasswordCondition(const QString &password)
{
    QRegularExpression re("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$");
    QRegularExpressionValidator validator(re);

    int pos = 0;
    QString validatedPassword = password;
    return validator.validate(validatedPassword, pos)
           == QRegularExpressionValidator::State::Acceptable;
}

void ProfileEditDialog::clickApply()
{
    if (!meetPasswordCondition(newPassword->text())) {
        MainAppWindow::instance().postError(
            "Your password does not meet security condition. Password should contain at "
            "least 8 symbols, both small and big latin letters, numbers and special characters");
        newPassword->setFocus();
        return;
    }

    if (newPassword->text() != confirmPassword->text()) {
        MainAppWindow::instance().postError("Passwords not coincide");
        newPassword->setFocus();
        return;
    }

    switch (AuthData::instance().getRole()) {
    case IAuthRepository::STUDENT: {
        Student student;
        student.setAvatar(ImageUtils::imageToByteArray(avatar->image()));
        student.setLogin(AuthData::instance().getLogin());
        student.setPassword(newPassword->text());
        student.setProgram(oldPassword->text());
        if (RepositoryManager::instance().studentRepository()->updateStudent(student)) {
            MainAppWindow::instance().postMessage("Update success.");
            emit doApply();
        } else {
            MainAppWindow::instance().postError(
                "Update unsuccessfull. Check course.log file for reason or "
                "ask system administrator for help.");
        }
        break;
    }
    case IAuthRepository::TA: {
        Assistant assistant;
        assistant.setAvatar(ImageUtils::imageToByteArray(avatar->image()));
        assistant.setLogin(AuthData::instance().getLogin());
        assistant.setPassword(newPassword->text());
        assistant.setProgram(oldPassword->text());
        if (RepositoryManager::instance().assistantRepository()->updateAssistant(assistant)) {
            MainAppWindow::instance().postMessage("Update success.");
            emit doApply();
        } else {
            MainAppWindow::instance().postError(
                "Update unsuccessfull. Check course.log file for reason or "
                "ask system administrator for help.");
        }
        break;
    }
    case IAuthRepository::TEACHER: {
        Teacher teacher;
        teacher.setAvatar(ImageUtils::imageToByteArray(avatar->image()));
        teacher.setLogin(AuthData::instance().getLogin());
        teacher.setPassword(newPassword->text());
        //temp, make separate dto for this stuff after
        teacher.setProgram(oldPassword->text());
        if (RepositoryManager::instance().teacherRepository()->updateTeacher(teacher)) {
            MainAppWindow::instance().postMessage("Update success.");
            emit doApply();
        } else {
            MainAppWindow::instance().postError(
                "Update unsuccessfull. Check course.log file for reason or "
                "ask system administrator for help.");
        }

    } break;
    case IAuthRepository::UNKNOWN:
        MainAppWindow::instance().postError("Update unsuccessfull. Unknown role.");

        break;
    }
}

void ProfileEditDialog::clickCancel()
{
    emit doCancel();
}

void ProfileEditDialog::clickHelp()
{
    emit doHelp();
}

