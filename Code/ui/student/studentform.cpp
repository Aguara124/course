#include "StudentForm.h"
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
#include "workers/fcsgetlecturer.h"
#include "workers/teacherget.h"
#include "workers/teachergetassistant.h"
#include "workers/teachergetgrades.h"
#include <qalgorithms.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qtmaterialtabs.h>

#include "studentassignmenttab.h"
// #include "consultationtab.h"
#include "studentformulatab.h"
#include "studentgradetab.h"

StudentForm::StudentForm(const QString &studentLogin, QWidget *parent)
    : QWidget{parent}
    , mainLayout(new QVBoxLayout(this))
    , tabsPanel(new QtMaterialTabs(this))
    , tabsLayout(new QHBoxLayout(this))
    , m_pGradeTab(new StudentGradeTab(studentLogin, this))
    , m_pFormulaTab(new StudentFormulaTab(studentLogin, this))
    , m_pAssignmentTab(new StudentAssignmentTab(studentLogin, this))
    , m_pCurrentWidget(nullptr)

{
}


void StudentForm::setupForm()
{
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet("QWidget { background: white; }");

    m_pGradeTab->setupTabLayout();
    m_pFormulaTab->setupTabLayout();
    m_pAssignmentTab->setupTabLayout();

    m_pAssignmentTab->hide();
    m_pFormulaTab->hide();

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(setupTabs());
    connect(tabsPanel, SIGNAL(currentChanged(int)), this, SLOT(tabsChanged(int)));

    mainLayout->addWidget(m_pGradeTab);
    mainLayout->addWidget(m_pAssignmentTab);
    mainLayout->addWidget(m_pFormulaTab);

    tabsChanged(0);

    setLayout(mainLayout);
}

QLayout *StudentForm::setupTabs()
{
    tabsPanel->addTab("Marks");
    tabsPanel->addTab("Assignments");
    tabsPanel->addTab("Results");

    tabsPanel->setInkTabLayout(Material::InkTabLayout::Down);
    tabsPanel->setMaximumHeight(50);

    tabsPanel->setBackgroundColor(QColor(207, 226, 243));
    tabsPanel->setTextColor(QColor(41,49,51));
    tabsPanel->setInkColor(QColor(60, 98, 132));

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;

    effect->setBlurRadius(15);
    effect->setOffset(QPointF(0, 3));
    effect->setColor(QColor(0, 0, 0, 75));
    tabsPanel->setGraphicsEffect(effect);

    tabsLayout->addWidget(tabsPanel);
    tabsLayout->setContentsMargins(20, 20, 20, 20);

    return tabsLayout;
}

void StudentForm::tabsChanged(int currentTab)
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
        m_pCurrentWidget = m_pFormulaTab;
        break;

    case 3:
        m_pCurrentWidget = nullptr;
        break;
    }

    if (m_pCurrentWidget != nullptr) {
        m_pCurrentWidget->show();
    }
}
