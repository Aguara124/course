#include "assistantform.h"

#include <QButtonGroup>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QMap>
#include "../mainappwindow.h"
#include "qcombobox.h"

#include "qtmaterialdivider.h"
#include "qtmaterialradiobutton.h"
#include "qtmaterialscrollbar.h"
#include "qtmaterialselectbox.h"
#include "qtmaterialtabs.h"

#include "assignmenttabassistant.h"
#include "consultationassistant.h"
#include "gradetabassistant.h"


AssistantForm::AssistantForm(const QString &assistantLogin, QWidget *parent)
    : QWidget{parent}
    , mainLayout(new QVBoxLayout(this))
    , tabsPanel(new QtMaterialTabs(this))
    , tabsLayout(new QHBoxLayout(this))
    , m_pGradeTab(new GradeTabAssistant(assistantLogin, this))
    , m_pConsultationTab(new ConsultationTabAssistant(assistantLogin, this))
    , m_pAssignmentTab(new AssignmentTabAssistant(assistantLogin, this))
    , m_pCurrentWidget(nullptr)
{
}

void AssistantForm::setupForm()
{
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet("QWidget { background: white; }");

    m_pGradeTab->setupTabLayout();
    m_pConsultationTab->setupTabLayout();
    m_pAssignmentTab->setupTabLayout();

    m_pAssignmentTab->hide();
    m_pConsultationTab->hide();

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(setupTabs());
    connect(tabsPanel, SIGNAL(currentChanged(int)), this, SLOT(tabsChanged(int)));

    mainLayout->addWidget(m_pGradeTab);
    mainLayout->addWidget(m_pAssignmentTab);
    mainLayout->addWidget(m_pConsultationTab);

    tabsChanged(0);

    setLayout(mainLayout);
}

QLayout *AssistantForm::setupTabs()
{
    tabsPanel->addTab("Marks");
    //new
    tabsPanel->addTab("Assignments");
    //new
    tabsPanel->addTab("Consultations");

    tabsPanel->setInkTabLayout(Material::InkTabLayout::Down);
    tabsPanel->setMaximumHeight(50);

    //new
    tabsPanel->setBackgroundColor(QColor(207, 226, 243));
    tabsPanel->setTextColor(QColor(41,49,51));
    tabsPanel->setInkColor(QColor(60, 98, 132));
    //new

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;

    effect->setBlurRadius(15);
    effect->setOffset(QPointF(0, 3));
    effect->setColor(QColor(0, 0, 0, 75));
    tabsPanel->setGraphicsEffect(effect);

    tabsLayout->addWidget(tabsPanel);
    tabsLayout->setContentsMargins(20, 20, 20, 20);

    return tabsLayout;
}

void AssistantForm::tabsChanged(int currentTab)
{
    if (m_pCurrentWidget != nullptr) {
        m_pCurrentWidget->hide();
    }

    switch (currentTab) {
    case 0:
        m_pCurrentWidget = m_pGradeTab;
        break;

    case 1:
        m_pCurrentWidget = m_pAssignmentTab;
        break;

    case 2:
        m_pCurrentWidget = m_pConsultationTab;
        break;
    }

    if (m_pCurrentWidget != nullptr) {
        m_pCurrentWidget->show();
    }
}
