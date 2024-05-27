#ifndef TEACHERFORM_H
#define TEACHERFORM_H

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLayout;

class QtMaterialTabs;

class GradeTab;
class FormulaTab;
class ConsultationTab;
class AssignmentTab;

class QBrush;

class TeacherForm : public QWidget
{
    Q_OBJECT
public:
    explicit TeacherForm(const QString &teacherLogin, QWidget *parent = nullptr);
    void setupForm();

signals:

protected slots:
    void tabsChanged(int currentTab);

private:
    QVBoxLayout *mainLayout;
    QtMaterialTabs *tabsPanel;
    QHBoxLayout *tabsLayout;

    GradeTab *m_pGradeTab;
    AssignmentTab *m_pAssignmentTab;
    ConsultationTab *m_pConsultationTab;
    FormulaTab *m_pFormulaTab;
    QWidget *m_pCurrentWidget;

private:
    QLayout *setupTabs();

};

#endif // TEACHERFORM_H
