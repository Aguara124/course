#ifndef REGISTRATIONFORMDIALOG_H
#define REGISTRATIONFORMDIALOG_H

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

#include "authrepository.h"
#include "avatar/imageedit.h"
#include "fcsrepository.h"
#include "studentrepository.h"

class RegistrationFormDialog : public QtMaterialDialog
{
    Q_OBJECT
public:
    explicit RegistrationFormDialog(QWidget *parent = nullptr);
    void setupForm();

protected slots:
    void programChanged(QString programName);
    void updateFaculty(const QList<std::shared_ptr<Fcs>> &facultyList);
    void updateProgram(const QList<std::shared_ptr<Fcs>> &programList);
    void changeAvatar();
    void acceptAvatar();
    void facultyChanged(QString faculty);

    void clickRegister();
    void clickLogin();
    void clickHelp();

signals:
    void doLogin(const QString &login);
    void doHelp();

private:
    std::unique_ptr<QVBoxLayout> mainLayout;

    std::unique_ptr<QtMaterialTextField> login;
    std::unique_ptr<QtMaterialAvatar> avatar;

    std::unique_ptr<QtMaterialTextField> firstName;
    std::unique_ptr<QtMaterialTextField> lastName;

    std::unique_ptr<QtMaterialSelectBox> faculty;
    std::unique_ptr<QtMaterialSelectBox> program;

    std::unique_ptr<QtMaterialCheckBox> isPilotGroup;

    std::unique_ptr<QtMaterialSelectBox> groupYearNumber;
    std::unique_ptr<QtMaterialSelectBox> groupIndexNumber;
    std::unique_ptr<QtMaterialSelectBox> subGroupNumber;

    std::unique_ptr<QtMaterialTextField> password;
    std::unique_ptr<QtMaterialTextField> confirmPassword;

    std::unique_ptr<QtMaterialRaisedButton> registerButton;
    std::unique_ptr<QtMaterialRaisedButton> cancelButton;

    std::unique_ptr<QtMaterialIconButton> helpButton;

    std::unique_ptr<IAuthRepository> authRepository;
    std::unique_ptr<IStudentRepository> studentRepository;
    std::unique_ptr<IFcsRepository> fcsRepository;

    std::shared_ptr<ImageEdit> imageEdit;
    QList<std::shared_ptr<Fcs>> programList;

private:
    static QStringList getYears(int numYear);
    static QStringList getRange(int number, int start);
    static bool meetPasswordCondition(const QString &password);
};

#endif // REGISTRATIONFORMDIALOG_H
