#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <qtmaterialdialog.h>
#include <qtmaterialiconbutton.h>
#include <qtmaterialraisedbutton.h>
#include <qtmaterialtextfield.h>

class LoginDialog : public QtMaterialDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    void setupForm();
    void setLogin(const QString &login);

protected slots:
    void passwordEchoToggle();

    void clickLogin();
    void clickRegister();
    void clickHelp();
signals:
    void doLogin(const QString &login, const QString &password);
    void doRegister();
    void doHelp();

private:
    std::unique_ptr<QVBoxLayout> vertLayout;
    std::unique_ptr<QHBoxLayout> horzLayout;

    std::unique_ptr<QLabel> loginLabel;

    std::unique_ptr<QtMaterialTextField> login;
    std::unique_ptr<QtMaterialTextField> password;

    std::unique_ptr<QtMaterialRaisedButton> loginButton;
    std::unique_ptr<QtMaterialRaisedButton> registerButton;
    std::unique_ptr<QtMaterialIconButton> helpButton;

    QAction *passwordEchoAction;

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // LOGINDIALOG_H
