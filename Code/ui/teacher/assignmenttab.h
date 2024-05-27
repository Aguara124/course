#ifndef ASSIGNMENTTAB_H
#define ASSIGNMENTTAB_H

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

#include <QMap>

#include "../hint.h"
#include "assignmentrepository.h"
#include "assistant.h"
#include "assistantrepository.h"
#include "student.h"

using QStringMap = QMap<QString, QString>;

class AssignmentTab : public QWidget
{
    Q_OBJECT
public:
    explicit AssignmentTab(const QString &teacherLogin, QWidget *parent = nullptr);
    void setupTabLayout();

protected slots:
    void updateAssignments(const QList<std::shared_ptr<Assignment>> &result);
    void resetAssignmentsSelection(const QList<std::shared_ptr<Assignment>> &result);
signals:

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    QWidget *setupFilterBox();
    QWidget *setupAssignmentBox();
    QWidget *setupAssignmentListBox();

    void showAssignment(QVBoxLayout *pLayout, Assignment *pAssignment);

    Assistant *getAssistant(const QString &assistantLogin);
    bool isAssignmentExpired(Assignment *pAssignment);

    void addHeaderField(QVBoxLayout *headerLayout,
                        const QString &labelText,
                        QWidget *fieldContent,
                        int labelWidth);

    QStringList getAvailableGroups();
    QStringList getAvailableSubjects();
    void addTableItem(
        QTableWidget *pTableWidget, int row, int column, const QString &itemText, bool isEditable);
    QTableWidget *setupGradeTable(Assignment *pAssignment);
    void addImageToLayout(QVBoxLayout *imageLayout, const QImage &image);
    void adjustGradeTableGeometry();
    void buildImageLayout(QVBoxLayout *parentLayout, Assignment *pAssignment);
    QString getAssigner(const QString &assignerLogin);

    QByteArray generateGradeTable(int group, int subGroup);
    QByteArray getGradeTable();
    void loadGradeTableToWidget(QTableWidget *tableWidget, Assignment *pAssignment);

private:
    const QString teacherLogin;

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
    QtMaterialCheckBox *m_pExpired;
    QtMaterialCheckBox *m_pNotExpired;
    QGroupBox *m_pAssignListGroupBox;

    Assignment *m_pCurrentRequest;
    QTableWidget *m_pGradeTable;

    QList<std::shared_ptr<Teacher>> teachers;
    QList<std::shared_ptr<Assignment>> assignments;
    QList<std::shared_ptr<Assistant>> assistants;
    QList<std::shared_ptr<Student>> students;

    QList<QImage> m_images;
    std::unique_ptr<Hint> hintWindow;
};

#endif // ASSIGNMENT
