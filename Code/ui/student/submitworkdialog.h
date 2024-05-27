#ifndef SUBMITWORKQDIALOG_H
#define SUBMITWORKDIALOG_H

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

#include <assignment.h>
#include <student.h>
#include <work.h>

class SubmitWorkDialog : public QtMaterialDialog
{
    Q_OBJECT
public:
    explicit SubmitWorkDialog(std::shared_ptr<Student> student,
                              const Assignment *assignment,
                              QWidget *parent = nullptr);
    void setupForm();

protected slots:
    void clickSubmit();
    void clickCancel();
    void clickHelp();
signals:
    void doSubmit(const std::shared_ptr<Work> &work);
    void doCancel();
    void doHelp();

private:
    QVBoxLayout *vertLayout;
    QHBoxLayout *horzLayout;

    QtMaterialRaisedButton *submitButton;
    QtMaterialRaisedButton *cancelButton;
    QtMaterialIconButton *helpButton;

    QtMaterialTextField *m_pAssignmentNameField;
    QTextEdit *m_pTextEdit;

    void addHeaderField(QVBoxLayout *headerLayout,
                        const QString &labelText,
                        QWidget *fieldContent,
                        int labelWidth);
    void addImageToLayout(QVBoxLayout *imageLayout, const QImage &image);
    static void removeWidgets(QLayout *pLayout);
    QByteArray readFile(const QString &filePath);

    std::shared_ptr<Student> m_student;
    const Assignment *m_pAssignment;
    std::shared_ptr<Work> m_work;

    QByteArray m_currentFile;
    bool m_bIsPdf;
    QList<QImage> m_images;
};

#endif 
