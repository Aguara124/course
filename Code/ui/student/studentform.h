#ifndef StudentForm_H
#define StudentForm_H

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLayout;

class QtMaterialTabs;

class FormulaTab;
class ConsultationTab;

class QBrush;
class StudentGradeTab;
class StudentAssignmentTab;
class StudentFormulaTab;

class StudentForm : public QWidget
{
    Q_OBJECT
public:
    explicit StudentForm(const QString &studentLogin, QWidget *parent = nullptr);
    void setupForm();

signals:

protected slots:
    void tabsChanged(int currentTab);

private:
    QVBoxLayout *mainLayout;
    QtMaterialTabs *tabsPanel;
    QHBoxLayout *tabsLayout;

    StudentGradeTab *m_pGradeTab;
    StudentFormulaTab *m_pFormulaTab;
    ConsultationTab *m_pConsultationTab;
    StudentAssignmentTab *m_pAssignmentTab;
    QWidget *m_pCurrentWidget;

private:
    QLayout *setupTabs();

};

#endif // StudentForm_H
