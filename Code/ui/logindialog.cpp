#include "logindialog.h"
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QWidget>
#include "imageutils.h"
#include "mainappwindow.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QtMaterialDialog(parent)
    , vertLayout(new QVBoxLayout)
    , horzLayout(new QHBoxLayout)
    , loginLabel(new QLabel)
    , login(new QtMaterialTextField)
    , password(new QtMaterialTextField)
    , loginButton(new QtMaterialRaisedButton)
    , registerButton(new QtMaterialRaisedButton)
    , helpButton(
          new QtMaterialIconButton(QIcon(":/icons/icons/action/svg/production/ic_help_24px.svg"),
                                   parent))
{}

void LoginDialog::passwordEchoToggle()
{
    if (password->echoMode() == QLineEdit::EchoMode::Password) {
        password->setEchoMode(QLineEdit::EchoMode::Normal);
        passwordEchoAction->setIcon(ImageUtils::makeShadedIcon(
            ":/icons/icons/action/svg/production/ic_visibility_off_24px.svg"));
    } else {
        password->setEchoMode(QLineEdit::EchoMode::Password);
        passwordEchoAction->setIcon(ImageUtils::makeShadedIcon(
            ":/icons/icons/action/svg/production/ic_visibility_24px.svg"));
    }
}

void LoginDialog::clickLogin()
{
    qDebug() << "login attempt " << login->text();
    emit doLogin(login->text(), password->text());
}

void LoginDialog::clickRegister()
{
    qDebug() << "registration form requested";

    emit doRegister();
}

void LoginDialog::clickHelp()
{
    qDebug() << "help requested";

    emit doHelp();
}

void LoginDialog::setupForm()
{
    QIcon loginIcon(":/icons/icons/png/enter.png");
    loginLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    loginLabel->setMinimumHeight(128);
    loginLabel->setMinimumWidth(128);
    loginLabel->setPixmap(loginIcon.pixmap(QSize(128, 128)));

    vertLayout->addSpacing(30);
    vertLayout->addWidget(loginLabel.get(), 1, Qt::AlignCenter);

    login->setLabel("Login");
    login->setMinimumWidth(200);
    login->setMaximumWidth(200);
    login->setMaximumHeight(50);
    login->setPlaceholderText("Enter your login...");
    login->setInkColor(QColor(160, 198, 232));
    vertLayout->addWidget(login.get(), 1, Qt::AlignCenter);

    password->setLabel("Password");
    password->setMinimumWidth(200);
    password->setMaximumWidth(200);
    password->setMaximumHeight(50);
    password->setPlaceholderText("Enter password...");
    password->setInkColor(QColor(160, 198, 232));
    password->setEchoMode(QLineEdit::EchoMode::Password);

    passwordEchoAction
        = password->addAction(ImageUtils::makeShadedIcon(
                                  ":/icons/icons/action/svg/production/ic_visibility_24px.svg"),
                              QLineEdit::TrailingPosition);

    vertLayout->addWidget(password.get(), 1, Qt::AlignCenter);
    vertLayout->addSpacing(30);

    loginButton->setText("Login");
    loginButton->setMinimumWidth(100);
    loginButton->setMaximumWidth(150);
    loginButton->setHaloVisible(true);
    loginButton->setRole(Material::Role::Primary);
    loginButton->setBackgroundColor(QColor(160, 198, 232));

    horzLayout->addWidget(loginButton.get(), 1, Qt::AlignRight | Qt::AlignVCenter);

    registerButton->setText("Register");
    registerButton->setMinimumWidth(100);
    registerButton->setMaximumWidth(150);
    registerButton->setHaloVisible(true);
    registerButton->setRole(Material::Role::Default);
    registerButton->useThemeColors();
    registerButton->setOverlayStyle(Material::OverlayStyle::TintedOverlay);
    registerButton->setRippleStyle(Material::RippleStyle::PositionedRipple);
    registerButton->setBackgroundColor(QColor(89, 89, 89));

    horzLayout->addWidget(registerButton.get(), 1, Qt::AlignLeft | Qt::AlignVCenter);

    vertLayout->addLayout(horzLayout.get());

    helpButton->setIconSize(QSize(16, 16));
    helpButton->setOpacity(0.5);

    vertLayout->addWidget(helpButton.get(), 1, Qt::AlignRight | Qt::AlignBottom);

    setWindowLayout(vertLayout.get());

    connect(passwordEchoAction, &QAction::triggered, this, &LoginDialog::passwordEchoToggle);

    connect(registerButton.get(), SIGNAL(clicked()), this, SLOT(clickRegister()));
    connect(loginButton.get(), SIGNAL(clicked()), this, SLOT(clickLogin()));
    connect(helpButton.get(), SIGNAL(clicked()), this, SLOT(clickHelp()));
}

void LoginDialog::setLogin(const QString &loginValue)
{
    if (!loginValue.isNull() && !loginValue.isEmpty()) {
        login->setText(loginValue);
        login->setFocus();
    }
}

void LoginDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return) {
        loginButton->click();
    }

    QtMaterialDialog::keyPressEvent(e);
}
