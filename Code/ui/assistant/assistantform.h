#ifndef ASSISTANTFORM_H
#define ASSISTANTFORM_H

#include <QWidget>

#include <QHBoxLayout>
#include <QVBoxLayout>

class QVBoxLayout;
class QHBoxLayout;
class QLayout;

class QtMaterialTabs;

class GradeTabAssistant;
class ConsultationTabAssistant;
class AssignmentTabAssistant;

class QBrush;


class AssistantForm: public QWidget
{
    Q_OBJECT
public:
    explicit AssistantForm(const QString &assistantLogin, QWidget *parent = nullptr);
    void setupForm();
protected slots:
    void tabsChanged(int currentTab);
private:
    QVBoxLayout *mainLayout;
    QtMaterialTabs *tabsPanel;
    QHBoxLayout *tabsLayout;

    GradeTabAssistant *m_pGradeTab;
    ConsultationTabAssistant *m_pConsultationTab;
    AssignmentTabAssistant *m_pAssignmentTab;
    QWidget *m_pCurrentWidget;

private:
    QLayout *setupTabs();

};

#endif // ASSISTANTFORM_H
