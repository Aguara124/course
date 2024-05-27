#include "registrationformdialog.h"
#include <QFileDialog>
#include <QRegularExpression>
#include "QRegularExpressionValidator"
#include "avatar/imageCropper.h"
#include "avatar/imageedit.h"
#include "fcsrepository.h"
#include "imageutils.h"
#include "mainappwindow.h"
#include "repositorymanager.h"
#include "studentrepository.h"
#include "workers/fcsgetfaculty.h"
#include "workers/fcsgetprogram.h"

RegistrationFormDialog::RegistrationFormDialog(QWidget *parent)
    : QtMaterialDialog(parent)
    , mainLayout(new QVBoxLayout)
    , login(new QtMaterialTextField)
    , avatar(new QtMaterialAvatar(parent))
    , firstName(new QtMaterialTextField)
    , lastName(new QtMaterialTextField)

    , faculty(new QtMaterialSelectBox)
    , program(new QtMaterialSelectBox)
    , isPilotGroup(new QtMaterialCheckBox)
    , groupYearNumber(new QtMaterialSelectBox)
    , groupIndexNumber(new QtMaterialSelectBox)
    , subGroupNumber(new QtMaterialSelectBox)

    , password(new QtMaterialTextField)
    , confirmPassword(new QtMaterialTextField)
    , registerButton(new QtMaterialRaisedButton)
    , cancelButton(new QtMaterialRaisedButton)
    , helpButton(
          new QtMaterialIconButton(QIcon(":/icons/icons/action/svg/production/ic_help_24px.svg"),
                                   parent))
    , authRepository(RepositoryManager::instance().authRepository())
    , studentRepository(RepositoryManager::instance().studentRepository())
    , fcsRepository(RepositoryManager::instance().fcsRepository())
    , imageEdit(new ImageEdit)
{

    FcsGetFaculty *fcsGetFaculty = new FcsGetFaculty();
    connect(fcsGetFaculty,
            SIGNAL(result(const QList<std::shared_ptr<Fcs>> &)),
            this,
            SLOT(updateFaculty(const QList<std::shared_ptr<Fcs>> &)));
    fcsGetFaculty->start();
}

void RegistrationFormDialog::setupForm()
{
    mainLayout->addSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QImage avatarImage(":/icons/icons/png/anonymous_128.png");
    avatar->setImage(avatarImage);
    avatar->setSize(128);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addSpacing(20);
    hBoxLayout->addWidget(avatar.get());

    connect(avatar.get(), SIGNAL(clicked()), this, SLOT(changeAvatar()));

    QVBoxLayout *loginGroupVBoxLayout = new QVBoxLayout;
    login->setLabel("Login");
    login->setMinimumWidth(300);
    login->setMaximumWidth(300);
    login->setPlaceholderText("Enter login name...");
    login->setInkColor(QColor(160, 198, 232));
    login->setEchoMode(QLineEdit::EchoMode::Normal);

    loginGroupVBoxLayout->addWidget(login.get(), 1, Qt::AlignCenter);

    QHBoxLayout *loginGroupHBoxLayout = new QHBoxLayout;
    firstName->setLabel("First name");
    firstName->setMinimumWidth(150);
    firstName->setMaximumWidth(150);
    firstName->setInkColor(QColor(160, 198, 232));
    firstName->setPlaceholderText("Enter first name...");

    loginGroupHBoxLayout->addWidget(firstName.get(), 1, Qt::AlignCenter);

    lastName->setLabel("Last name");
    lastName->setMinimumWidth(150);
    lastName->setMaximumWidth(150);
    lastName->setInkColor(QColor(160, 198, 232));
    lastName->setPlaceholderText("Enter last name...");

    loginGroupHBoxLayout->addWidget(lastName.get(), 1, Qt::AlignCenter);
    loginGroupVBoxLayout->addLayout(loginGroupHBoxLayout);

    hBoxLayout->addSpacing(20);
    hBoxLayout->addLayout(loginGroupVBoxLayout);

    mainLayout->addLayout(hBoxLayout);
    mainLayout->addSpacing(30);

    faculty->setLabel("Faculty");
    faculty->setPlaceholderText("Select faculty...");
    faculty->setInkColor(QColor(160, 198, 232));
    faculty->setMinimumHeight(50);

    mainLayout->addWidget(faculty.get());

    connect(faculty.get(), SIGNAL(itemSelected(QString)), this, SLOT(facultyChanged(QString)));

    program->setDataSource(QStringList());
    program->setLabel("Program");
    program->setPlaceholderText("Select program...");
    program->setInkColor(QColor(160, 198, 232));
    program->setDisabled(true);

    mainLayout->addWidget(program.get());

    connect(program.get(), SIGNAL(itemSelected(QString)), this, SLOT(programChanged(QString)));

    QHBoxLayout *groupHBoxLayout = new QHBoxLayout;

    groupYearNumber->setDataSource(getYears(5));
    groupYearNumber->setLabel("Year of admission");
    groupYearNumber->setPlaceholderText("Select year of admission...");
    groupYearNumber->setMinimumWidth(100);
    groupYearNumber->setMaximumWidth(150);
    groupYearNumber->setMenuMaxHeight(150);
    groupYearNumber->setInkColor(QColor(160, 198, 232));

    groupHBoxLayout->addWidget(groupYearNumber.get());

    groupIndexNumber->setDataSource(getRange(5, 1));
    groupIndexNumber->setLabel("Group number");
    groupIndexNumber->setPlaceholderText("Select number...");
    groupIndexNumber->setMinimumWidth(100);
    groupIndexNumber->setMaximumWidth(150);
    groupIndexNumber->setMenuMaxHeight(150);
    groupIndexNumber->setInkColor(QColor(160, 198, 232));

    groupHBoxLayout->addWidget(groupIndexNumber.get());

    subGroupNumber->setDataSource(getRange(15, 1));
    subGroupNumber->setLabel("Subgroup");
    subGroupNumber->setPlaceholderText("Select subgroup...");
    subGroupNumber->setMinimumWidth(100);
    subGroupNumber->setMaximumWidth(150);
    subGroupNumber->setMenuMaxHeight(150);
    subGroupNumber->setText("1");
    subGroupNumber->setInkColor(QColor(160, 198, 232));

    groupHBoxLayout->addWidget(subGroupNumber.get());

    mainLayout->addSpacing(20);
    mainLayout->addLayout(groupHBoxLayout);

    isPilotGroup->setText("Are you in pilot group?");
    isPilotGroup->setLabelPosition(QtMaterialCheckable::LabelPositionRight);
    isPilotGroup->hide();
    isPilotGroup->setCheckedColor(QColor(207, 226, 243));

    mainLayout->addWidget(isPilotGroup.get());
    mainLayout->addSpacing(30);

    QHBoxLayout *passwordHBoxLayout = new QHBoxLayout;

    password->setLabel("Password");
    password->setMinimumWidth(200);
    password->setMaximumWidth(200);
    password->setMaximumHeight(50);
    password->setPlaceholderText("Enter password...");
    password->setInkColor(QColor(160, 198, 232));
    password->setEchoMode(QLineEdit::EchoMode::Password);

    passwordHBoxLayout->addWidget(password.get());

    confirmPassword->setLabel("Password confirmation");
    confirmPassword->setMinimumWidth(200);
    confirmPassword->setMaximumWidth(200);
    confirmPassword->setMaximumHeight(50);
    confirmPassword->setPlaceholderText("Enter password...");
    confirmPassword->setInkColor(QColor(160, 198, 232));
    confirmPassword->setEchoMode(QLineEdit::EchoMode::Password);

    passwordHBoxLayout->addWidget(confirmPassword.get());
    mainLayout->addLayout(passwordHBoxLayout);
    mainLayout->addSpacing(30);

    QHBoxLayout *buttonHBoxLayout = new QHBoxLayout;

    registerButton->setText("Register");
    registerButton->setMinimumWidth(100);
    registerButton->setMaximumWidth(150);
    registerButton->setHaloVisible(true);
    registerButton->setRole(Material::Role::Primary);
    registerButton->useThemeColors();
    registerButton->setOverlayStyle(Material::OverlayStyle::TintedOverlay);
    registerButton->setRippleStyle(Material::RippleStyle::PositionedRipple);
    registerButton->setBackgroundColor(QColor(204, 204, 204));
    registerButton->setForegroundColor(QColor(89, 89, 89));

    buttonHBoxLayout->addWidget(registerButton.get(), 1, Qt::AlignRight | Qt::AlignVCenter);

    cancelButton->setText("Cancel");
    cancelButton->setMinimumWidth(100);
    cancelButton->setMaximumWidth(150);
    cancelButton->setHaloVisible(true);
    cancelButton->setRole(Material::Role::Default);
    cancelButton->useThemeColors();
    cancelButton->setOverlayStyle(Material::OverlayStyle::TintedOverlay);
    cancelButton->setRippleStyle(Material::RippleStyle::PositionedRipple);
    cancelButton->setBackgroundColor(QColor(204, 204, 204));
    cancelButton->setForegroundColor(QColor(89, 89, 89));

    buttonHBoxLayout->addWidget(cancelButton.get(), 1, Qt::AlignLeft | Qt::AlignVCenter);

    mainLayout->addLayout(buttonHBoxLayout);

    helpButton->setIconSize(QSize(16, 16));
    helpButton->setOpacity(0.5);

    mainLayout->addWidget(helpButton.get(), 1, Qt::AlignRight | Qt::AlignBottom);

    setWindowLayout(mainLayout.get());

    connect(registerButton.get(), SIGNAL(clicked()), this, SLOT(clickRegister()));
    connect(cancelButton.get(), SIGNAL(clicked()), this, SLOT(clickLogin()));
    connect(helpButton.get(), SIGNAL(clicked()), this, SLOT(clickHelp()));
}

void RegistrationFormDialog::programChanged(QString programName)
{
    if (programName == "Computer Science") {
        isPilotGroup->show();
    } else {
        isPilotGroup->hide();
    }
}

void RegistrationFormDialog::updateFaculty(const QList<std::shared_ptr<Fcs>> &facultyList)
{
    QStringList facultyNames;
    for (std::shared_ptr<Fcs> fcsPtr : facultyList) {
        if (!facultyNames.contains(fcsPtr->getFacultyName())) {
            facultyNames.append(fcsPtr->getFacultyName());
        }
    }

    faculty->setDataSource(facultyNames);
}

void RegistrationFormDialog::updateProgram(const QList<std::shared_ptr<Fcs>> &programList)
{
    this->programList = programList;

    QStringList programNames;
    for (std::shared_ptr<Fcs> fcsPtr : programList) {
        if (!programNames.contains(fcsPtr->getProgramName())) {
            programNames.append(fcsPtr->getProgramName());
        }
    }

    program->setDataSource(programNames);
    program->setDisabled(false);
}

void RegistrationFormDialog::changeAvatar()
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

void RegistrationFormDialog::acceptAvatar()
{
    avatar->setImage(imageEdit->getImage());
}

void RegistrationFormDialog::facultyChanged(QString faculty)
{
    FcsGetProgram *worker = new FcsGetProgram(faculty);
    connect(worker,
            SIGNAL(result(const QList<std::shared_ptr<Fcs>> &)),
            this,
            SLOT(updateProgram(const QList<std::shared_ptr<Fcs>> &)));
    worker->start();
}

bool RegistrationFormDialog::meetPasswordCondition(const QString &password)
{
    QRegularExpression re("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$");
    QRegularExpressionValidator validator(re);

    int pos = 0;
    QString validatedPassword = password;
    return validator.validate(validatedPassword, pos)
           == QRegularExpressionValidator::State::Acceptable;
}

void RegistrationFormDialog::clickRegister()
{
    if (login->text().isNull() || login->text().isEmpty()) {
        MainAppWindow::instance().postMessage("Login is empty!",
                                              QtMaterialSnackbar::MessageQualifier::Error,
                                              true);

        login->setFocus();
        return;
    }

    if (firstName->text().isNull() || firstName->text().isEmpty()) {
        MainAppWindow::instance().postMessage("First name is empty!",
                                              QtMaterialSnackbar::MessageQualifier::Error,
                                              true);
        firstName->setFocus();
        return;
    }

    if (lastName->text().isNull() || lastName->text().isEmpty()) {
        MainAppWindow::instance().postMessage("Last name is empty!",
                                              QtMaterialSnackbar::MessageQualifier::Error,
                                              true);
        lastName->setFocus();
        return;
    }

    if (faculty->text().isNull() || faculty->text().isEmpty()) {
        MainAppWindow::instance().postMessage("Select faculty first!",
                                              QtMaterialSnackbar::MessageQualifier::Error,
                                              true);
        faculty->setFocus();
        return;
    }

    if (program->text().isNull() || program->text().isEmpty()) {
        MainAppWindow::instance().postMessage("Select program first!",
                                              QtMaterialSnackbar::MessageQualifier::Error,
                                              true);
        program->setFocus();
        return;
    }

    if (groupYearNumber->text().isNull() || groupYearNumber->text().isEmpty()) {
        MainAppWindow::instance().postMessage("Select year of admission!",
                                              QtMaterialSnackbar::MessageQualifier::Error,
                                              true);
        groupYearNumber->setFocus();
        return;
    }

    if (groupIndexNumber->text().isNull() || groupIndexNumber->text().isEmpty()) {
        MainAppWindow::instance().postMessage("Select group number!",
                                              QtMaterialSnackbar::MessageQualifier::Error,
                                              true);
        groupIndexNumber->setFocus();
        return;
    }

    if (!meetPasswordCondition(password->text())) {
        MainAppWindow::instance().postMessage(
            "Your password does not meet security condition. Password should contain at "
            "least 8 symbols, both small and big latin letters, numbers and special characters",
            QtMaterialSnackbar::MessageQualifier::Error,
            true);
        password->setFocus();
        return;
    }

    if (password->text() != confirmPassword->text()) {
        MainAppWindow::instance().postMessage("Passwords not coincide",
                                              QtMaterialSnackbar::MessageQualifier::Error,

                                              true);
        password->setFocus();
        return;
    }

    if (authRepository->checkLogin(login->text())) {
        MainAppWindow::instance().postMessage("This login already used. Try another login.",
                                              QtMaterialSnackbar::MessageQualifier::Error,
                                              true);

        login->setFocus();
        return;
    }

    Student student;
    student.setLogin(login->text());
    student.setPassword(password->text());

    for (std::shared_ptr<Fcs> fcsPtr : programList) {
        if (fcsPtr->getProgramName() == program->text()) {
            student.setProgram(fcsPtr->getProgram());
            break;
        }
    }

    student.setGroup((groupYearNumber->text() + groupIndexNumber->text()).toInt());
    student.setPilot(isPilotGroup->isChecked());
    student.setSubgroup(subGroupNumber->text().toInt());
    student.setName(firstName->text());
    student.setSurname(lastName->text());
    student.setCourse(Student::calcCourse(student.getGroup()));
    student.setAvatar(ImageUtils::imageToByteArray(avatar->image()));

    if (studentRepository->addStudent(student)) {
        MainAppWindow::instance().postMessage("Registration success.");
    } else {
        MainAppWindow::instance()
            .postMessage("Registration unsuccessfull. Check course.log file for reason or "
                         "ask system administrator for help.",
                         QtMaterialSnackbar::MessageQualifier::Error,
                         true);
    }

    emit doLogin(login->text());
}

void RegistrationFormDialog::clickLogin()
{
    emit doLogin(QString());
}

void RegistrationFormDialog::clickHelp()
{
    emit doHelp();
}

QStringList RegistrationFormDialog::getYears(int numYear)
{
    int currentYear = QDate::currentDate().year() - 2000;
    QStringList result;
    for (int c = 0; c < numYear; c++) {
        result.append(QString("%1").arg(currentYear - c));
    }

    return result;
}

QStringList RegistrationFormDialog::getRange(int number, int start)
{
    QStringList result;
    for (int c = start; c < number; c++) {
        result.append(QString("%1").arg(c));
    }

    return result;
}
