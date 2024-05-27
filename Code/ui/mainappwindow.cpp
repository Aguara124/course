#include "mainappwindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include "QGraphicsDropShadowEffect"

#include "repositorymanager.h"
#include <qtmaterialdialog.h>

#include "authdata.h"
#include "authinfo.h"
#include "hint.h"
#include "logindialog.h"
#include "registrationformdialog.h"

#include "student/studentform.h"
#include "teacher/teacherform.h"

#include "assistant/assistantform.h"

MainAppWindow::MainAppWindow(QWidget *parent)
    : QMainWindow{parent}
    , authRepository(RepositoryManager::instance().authRepository())
    , mainWidget(new QWidget)
    , mainWidgetLayout(new QVBoxLayout)
    , snackbar(new QtMaterialSnackbar)
    , loginDialog(new LoginDialog)
    , regFormDialog(new RegistrationFormDialog)

{
    setStyleSheet("background-color: white;");

    restoreWindow();
    prepareSnackbar();
}

MainAppWindow::~MainAppWindow()
{
    qDebug() << "destructing";
}

void MainAppWindow::registerHelp()
{
    help("dummyhelp.pdf");
}

void MainAppWindow::loginHelp()
{
    help("dummyhelp.pdf");
}

void MainAppWindow::showRegistrationForm()
{
    loginDialog->hideDialog();

    mainWidgetLayout->removeWidget(loginDialog.get());
    mainWidgetLayout->addWidget(regFormDialog.get());

    regFormDialog->showDialog();
}

void MainAppWindow::showLoginForm(const QString &login)
{
    regFormDialog->hideDialog();

    mainWidgetLayout->removeWidget(regFormDialog.get());
    mainWidgetLayout->addWidget(loginDialog.get());

    loginDialog->setLogin(login);
    loginDialog->showDialog();
}

void MainAppWindow::postError(const QString &message)
{
    snackbar->addInstantMessage(message, QtMaterialSnackbar::MessageQualifier::Error);
    snackbar->raise();
}

void MainAppWindow::showDialog(QtMaterialDialog *dlg)
{
    if (dlg != nullptr) {
        mainWidgetLayout->addWidget((QWidget *) dlg, 1);

        switch (AuthData::instance().getRole()) {
        case IAuthRepository::STUDENT:
            dlg->setParent(studentForm.get());
            break;
        case IAuthRepository::TA:
            dlg->setParent(assistantForm.get());
            break;
        case IAuthRepository::TEACHER:
            dlg->setParent(teacherForm.get());
            break;

        default:
            postError("Something was wrong, no role received. Try relogin!");
            showLoginForm(QString(""));
            break;
        }

        dlg->showDialog();
    }
}

void MainAppWindow::hideDialog(QtMaterialDialog *dlg)
{
    if (dlg != nullptr) {
        dlg->setParent(nullptr);
        dlg->hideDialog();
        mainWidgetLayout->removeWidget((QWidget *) dlg);
    }
}

void MainAppWindow::help(const QString &helpSource)
{
    std::shared_ptr<Hint> hint = std::shared_ptr<Hint>(new Hint(this));
    hint->setHint(helpSource);
    hint->show();
    hint->exec();
}

void MainAppWindow::postMessage(const QString &message,
                                QtMaterialSnackbar::MessageQualifier qualifier,
                                bool instant)
{
    if (instant) {
        snackbar->addInstantMessage(message, qualifier);
    } else {
        snackbar->addMessage(message, qualifier);
    }

    snackbar->raise();
}

void MainAppWindow::tryLogin(const QString &login, const QString &password)
{
    if (!authRepository->authenticate(login, password, IAuthRepository::Role::UNKNOWN)) {
        qDebug() << "Auth for user " << login << " was unsuccessfull";
        postMessage("Authentication attempt was unsuccesfull!",
                    QtMaterialSnackbar::MessageQualifier::Error,
                    true);
        return;
    }

    loginDialog->hideDialog();
    mainWidgetLayout->removeWidget(loginDialog.get());

    switch (AuthData::instance().getRole()) {
    case IAuthRepository::STUDENT:
        showStudent(login);
        break;
    case IAuthRepository::TA:
        showAssistant(login);
        break;
    case IAuthRepository::TEACHER:
        showTeacher(login);
        break;

    default:
        postMessage("Something was wrong, no role received. Try relogin!",
                    QtMaterialSnackbar::MessageQualifier::Error,
                    true);
        showLoginForm(QString(""));
        break;
    }
}

void MainAppWindow::prepareSnackbar()
{
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;

    effect->setBlurRadius(15);
    effect->setOffset(QPointF(0, -1));
    effect->setColor(QColor(0, 0, 0, 75));

    snackbar->setGraphicsEffect(effect);

    snackbar->setParent(mainWidget.get());
    snackbar->setClickToDismissMode(true);
    snackbar->setBackgroundOpacity(0.9);
    snackbar->setBackgroundColor(QColor(166, 202, 233));
}

void MainAppWindow::restoreWindow()
{
    QSettings *pAppSettings = AppSettings::instance().getSettings();
    restoreGeometry(pAppSettings->value("Main/geometry").toByteArray());
    restoreState(pAppSettings->value("Main/windowState").toByteArray());

    loginDialog->setupForm();
    loginDialog->setMinimumWidth(250);
    loginDialog->setMinimumHeight(300);

    mainWidgetLayout->addWidget(loginDialog.get());

    mainWidget->setLayout(mainWidgetLayout.get());
    mainWidget->setContentsMargins(0, 0, 0, 0);
    mainWidgetLayout->setContentsMargins(0, 0, 0, 0);

    mainWidget->setStyleSheet("QWidget { background: white; }");
    setCentralWidget(mainWidget.get());

    connect(loginDialog.get(), SIGNAL(doHelp()), this, SLOT(loginHelp()));
    connect(loginDialog.get(), SIGNAL(doRegister()), this, SLOT(showRegistrationForm()));
    connect(loginDialog.get(),
            SIGNAL(doLogin(const QString &, const QString &)),
            this,
            SLOT(tryLogin(const QString &, const QString &)));

    regFormDialog->setupForm();
    regFormDialog->setMinimumWidth(400);
    regFormDialog->setMinimumHeight(400);

    connect(regFormDialog.get(), SIGNAL(doHelp()), this, SLOT(registerHelp()));
    connect(regFormDialog.get(),
            SIGNAL(doLogin(const QString &)),
            this,
            SLOT(showLoginForm(const QString &)));

    loginDialog->showDialog();
}

void MainAppWindow::showTeacher(const QString &login)
{
    int widgetCount = mainWidgetLayout->count();
    for (int i = 0; i < widgetCount; i++) {
        mainWidgetLayout->removeWidget(mainWidgetLayout->itemAt(0)->widget());
    }

    teacherForm = std::unique_ptr<TeacherForm>(new TeacherForm(login));
    teacherForm->setupForm();

    mainWidgetLayout->addWidget(teacherForm.get());
}

void MainAppWindow::showAssistant(const QString &login)
{
    int widgetCount = mainWidgetLayout->count();
    for (int i = 0; i < widgetCount; i++) {
        mainWidgetLayout->removeWidget(mainWidgetLayout->itemAt(0)->widget());
    }

    assistantForm = std::unique_ptr<AssistantForm>(new AssistantForm(login));
    assistantForm->setupForm();

    mainWidgetLayout->addWidget(assistantForm.get());
}


void MainAppWindow::showStudent(const QString &login)
{
    int widgetCount = mainWidgetLayout->count();
    for (int i = 0; i < widgetCount; i++) {
        mainWidgetLayout->removeWidget(mainWidgetLayout->itemAt(0)->widget());
    }

    studentForm = std::unique_ptr<StudentForm>(new StudentForm(login));
    studentForm->setupForm();

    mainWidgetLayout->addWidget(studentForm.get());
}

void MainAppWindow::closeEvent(QCloseEvent *event)
{
    QSettings *pAppSettings = AppSettings::instance().getSettings();

    pAppSettings->setValue("Main/geometry", saveGeometry());
    pAppSettings->setValue("Main/windowState", saveState());
    pAppSettings->sync();

    QMainWindow::closeEvent(event);
}
