#ifndef MAINAPPWINDOW_H
#define MAINAPPWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QVBoxLayout>
#include <QWidget>

#include <QThreadStorage>
#include "appsettings.h"
#include "authrepository.h"
#include "qtmaterialsnackbar.h"
#include "singleton.h"

class LoginDialog;
class RegistrationFormDialog;
class TeacherForm;
class AssistantForm;
class StudentForm;
class AuthInfo;

#include "qtmaterialdialog.h"

class MainAppWindow : public QMainWindow, public Singleton<MainAppWindow>
{
    Q_OBJECT
public:
    explicit MainAppWindow(QWidget *parent = nullptr);
    ~MainAppWindow();

    void postMessage(
        const QString &message,
        QtMaterialSnackbar::MessageQualifier qualifier = QtMaterialSnackbar::MessageQualifier::Info,
        bool instant = false);
    void postError(const QString &message);

    void showDialog(QtMaterialDialog *dialog);
    void hideDialog(QtMaterialDialog *dialog);

    void help(const QString &helpSource);

protected slots:
    void registerHelp();
    void loginHelp();
    void showRegistrationForm();
    void showLoginForm(const QString &login);
    void tryLogin(const QString &login, const QString &password);

signals:

protected:
private:
    void prepareSnackbar();
    void restoreWindow();

    void showTeacher(const QString &login);
    void showAssistant(const QString &login);
    void showStudent(const QString &login);

private:
    std::unique_ptr<IAuthRepository> authRepository;

    std::unique_ptr<QWidget> mainWidget;
    std::unique_ptr<QVBoxLayout> mainWidgetLayout;
    std::unique_ptr<QtMaterialSnackbar> snackbar;
    std::unique_ptr<LoginDialog> loginDialog;
    std::unique_ptr<RegistrationFormDialog> regFormDialog;
    std::unique_ptr<TeacherForm> teacherForm;
    std::unique_ptr<AssistantForm> assistantForm;
    std::unique_ptr<StudentForm> studentForm;

    QThreadStorage<AuthInfo *> tlsStorage;

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINAPPWINDOW_H
