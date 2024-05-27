#ifndef STUDENTASSIGNMENTTAB_H
#define STUDENTASSIGNMENTTAB_H

#include <QWidget>

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
class AskConsultationDialog;

#include <QMap>

#include "../hint.h"
#include "assignmentrepository.h"
#include "assistant.h"
#include "assistantrepository.h"
#include "grade.h"
#include "student.h"
#include "work.h"

using QStringMap = QMap<QString, QString>;

class StudentAssignmentTab : public QWidget
{
    Q_OBJECT
public:
    explicit StudentAssignmentTab(const QString &teacherLogin, QWidget *parent = nullptr);
    void setupTabLayout();

protected slots:
    void updateAssignments(const QList<std::shared_ptr<Assignment>> &result);
    void resetAssignmentsSelection(const QList<std::shared_ptr<Assignment>> &result);

    void onReceiveAssignments(const QList<std::shared_ptr<Assignment>> &assignments,
                              const QList<std::shared_ptr<Work>> &works,
                              const QList<std::shared_ptr<Grade>> &grades,
                              const QList<std::shared_ptr<Teacher>> &teachers,
                              const QList<std::shared_ptr<Assistant>> &assistants);

signals:

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    QWidget *setupFilterBox();
    QWidget *setupAssignmentBox();
    QWidget *setupAssignmentListBox();

    void showAssignment(QVBoxLayout *pLayout, Assignment *pAssignment);

    void addHeaderField(QVBoxLayout *headerLayout,
                        const QString &labelText,
                        QWidget *fieldContent,
                        int labelWidth);
    void addTableItem(
        QTableWidget *pTableWidget, int row, int column, const QString &itemText, bool isEditable);
    QTableWidget *setupGradeTable(Assignment *pAssignment);
    void addImageToLayout(QVBoxLayout *imageLayout, const QImage &image);
    void adjustGradeTableGeometry();
    void buildImageLayout(QVBoxLayout *parentLayout, Assignment *pAssignment);

    QByteArray generateGradeTable(int group, int subGroup);
    QByteArray getGradeTable();
    void loadGradeTableToWidget(QTableWidget *tableWidget, Assignment *pAssignment);

    static void removeWidgets(QLayout *pLayout);

    bool isAssignmentExpired(Assignment *pAssignment);
    bool isAssignmentSolved(int assignmentId);
    Assistant *getAssistant(const QString &assistantLogin);
    QString getAssigner(const QString &assignerLogin);
    Teacher *getTeacher(const QString &teacherLogin);
    QList<std::shared_ptr<Work>> getWork(int assignmentId);

private:
    const QString m_studentLogin;
    std::shared_ptr<Student> m_student;

    std::shared_ptr<QBrush> oddColor;
    std::shared_ptr<QBrush> evenColor;

    std::unique_ptr<IAssignmentRepository> assignmentRepository;

    QScrollArea *m_pAssignmentsArea;
    QVBoxLayout *m_pAssignmentsLayout;

    QVBoxLayout *m_pAssignmentRequestLayout;
    QGroupBox *m_pAssignmentRequestBox;
    QStringMap m_mapProgramToName;
    QHBoxLayout *m_pTabLayout;

    QtMaterialTextField *m_pSearchText;
    QtMaterialCheckBox *m_pSolved;
    QtMaterialCheckBox *m_pUnsolved;
    QGroupBox *m_pAssignListGroupBox;

    Assignment *m_pCurrentRequest;
    QTableWidget *m_pGradeTable;

    QList<std::shared_ptr<Teacher>> m_teachers;
    QList<std::shared_ptr<Assignment>> m_assignments;
    QList<std::shared_ptr<Assistant>> m_assistants;
    QList<std::shared_ptr<Work>> m_works;
    QList<std::shared_ptr<Grade>> m_grades;

    QList<QImage> m_images;
    std::unique_ptr<Hint> hintWindow;

    AskConsultationDialog *consultationRequest;
};

#endif // ASSIGNMENT
