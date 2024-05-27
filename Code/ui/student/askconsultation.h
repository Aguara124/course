#ifndef CONSULTATIONREQDIALOG_H
#define CONSULTATIONREQDIALOG_H

#include <QAction>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <qtmaterialdialog.h>
#include <qtmaterialiconbutton.h>
#include <qtmaterialraisedbutton.h>
#include <qtmaterialtextfield.h>

class QtMaterialSelectBox;
class QTextEdit;

#include <assistant.h>
#include <student.h>
#include <teacher.h>

class AskConsultationDialog : public QtMaterialDialog
{
    Q_OBJECT
public:
    explicit AskConsultationDialog(std::shared_ptr<Student> student,
                                   QList<std::shared_ptr<Assistant>> assistants,
                                   QList<std::shared_ptr<Teacher>> teachers,
                                   QWidget *parent = nullptr);
    void setupForm();

protected slots:
    void clickRequest();
    void clickCancel();
    void clickHelp();
signals:
    void doRequest(const QString &assistant, const QString &question);
    void doCancel();
    void doHelp();

private:
    QVBoxLayout *vertLayout;
    QHBoxLayout *horzLayout;

    QtMaterialRaisedButton *requestButton;
    QtMaterialRaisedButton *cancelButton;
    QtMaterialIconButton *helpButton;

    QtMaterialSelectBox *m_pAssistantSelector;
    QtMaterialSelectBox *m_pSubjectSelector;
    QTextEdit *m_pTextEdit;

    void addHeaderField(QVBoxLayout *headerLayout,
                        const QString &labelText,
                        QWidget *fieldContent,
                        int labelWidth);
    QStringList getAvailableSubjects();

    std::shared_ptr<Student> m_student;
    QList<std::shared_ptr<Assistant>> m_assistants;
    QList<std::shared_ptr<Teacher>> m_teachers;
};

#endif 
