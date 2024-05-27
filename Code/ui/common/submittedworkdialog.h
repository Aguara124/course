#ifndef SUBMITTEDWORKDIALOG_H
#define SUBMITTEDWORKDIALOG_H

#include <QWidget>
#include <QDialog>

class QHBoxLayout;
class QtMaterialTextField;
class QLabel;
class QTableWidget;
class QGroupBox;
class Teacher;
class Fcs;
class QtMaterialSelectBox;
class QtMaterialRadioButton;
class QtMaterialRaisedButton;
class QScrollArea;
class QVBoxLayout;
class QtMaterialCheckBox;

#include <QMap>

#include "../hint.h"
#include "assignmentrepository.h"
#include "assistantrepository.h"
#include "grade.h"
#include "graderepository.h"
#include "student.h"
#include "workrepository.h"

using QStringMap = QMap<QString, QString>;

class SubmittedWorkDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SubmittedWorkDialog(int assignmentId, QWidget *parent = nullptr);
    void setupLayout();

protected slots:
    void buildWorkList(const std::shared_ptr<Assignment> &assignment,
                       const QList<std::shared_ptr<Work>> &works,
                       const QList<std::shared_ptr<Student>> &students,
                       const QList<std::shared_ptr<Grade>> &grades);
signals:

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    QWidget *setupFilterBox();
    QWidget *setupWorkBox();
    QWidget *setupWorkListBox();

    void showWork(QVBoxLayout *pLayout, Work *pWork);
    std::shared_ptr<Student> getStudent(const QString &studentLogin);
    std::shared_ptr<Grade> getGrade(const QString &studentLogin);

    void addHeaderField(QVBoxLayout *headerLayout,
                        const QString &labelText,
                        QWidget *fieldContent,
                        int labelWidth);
    void addImageToLayout(QVBoxLayout *imageLayout, const QImage &image);
    void buildImageLayout(QVBoxLayout *parentLayout, Work *pWork);

    static const QString currentModule(const QDateTime &forTime = QDateTime::currentDateTime());

private:
    int m_nAssignmentId;

    std::shared_ptr<QBrush> oddColor;
    std::shared_ptr<QBrush> evenColor;

    std::unique_ptr<IAssignmentRepository> assignmentRepository;
    std::unique_ptr<IWorkRepository> workRepository;
    std::unique_ptr<IGradeRepository> gradeRepository;

    QScrollArea *m_pWorkListArea;
    QVBoxLayout *m_pWorkListLayout;

    QVBoxLayout *m_pWorkLayout;
    QGroupBox *m_pWorkGroupBox;
    QStringMap m_mapProgramToName;
    QHBoxLayout *m_pTabLayout;

    QtMaterialTextField *m_pSearchText;
    QtMaterialCheckBox *m_pGraded;
    QtMaterialCheckBox *m_pNotGraded;
    QGroupBox *m_pWorkListGroupBox;

    Work *m_pCurrentRequest;
    QTableWidget *m_pGradeTable;

    std::shared_ptr<Assignment> m_assignment;
    QList<std::shared_ptr<Work>> m_works;
    QList<std::shared_ptr<Student>> m_students;
    QList<std::shared_ptr<Grade>> m_grades;

    QList<QImage> m_images;
    std::unique_ptr<Hint> hintWindow;
};

#endif // ASSIGNMENT
