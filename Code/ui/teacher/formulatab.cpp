#include "formulatab.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJSEngine>
#include <QJSValue>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include "../mainappwindow.h"
#include "repositorymanager.h"
#include "workers/fcsgetlecturer.h"
#include "workers/teacherget.h"
#include <qbuttongroup.h>
#include <qtmaterialiconbutton.h>
#include <qtmaterialradiobutton.h>
#include <qtmaterialraisedbutton.h>
#include <qtmaterialscrollbar.h>
#include <qtmaterialselectbox.h>
#include <qtmaterialtextfield.h>

FormulaTab::FormulaTab(const QString &teacherLogin, QWidget *parent)
    : QWidget{parent}
    , teacherLogin(teacherLogin)
    , oddColor(new QBrush(QColor(240, 240, 240)))
    , evenColor(new QBrush(Qt::GlobalColor::white))
    , fcsRepository(RepositoryManager::instance().fcsRepository())
{}

QWidget *FormulaTab::setupStageBox()
{
    QFont font("Roboto", 12, QFont::Normal);

    QGroupBox *groupStageBox = new QGroupBox(tr("Select stage for calculating"), this);
    groupStageBox->setFont(font);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addSpacing(30);

    m_pSubjects = new QtMaterialSelectBox(this);
    m_pSubjects->setInkColor(QColor(160, 198, 232));
    m_pSubjects->setPlaceholderText("Select discipline");
    m_pSubjects->setLabel("Subject of education");
    m_pSubjects->setMinimumWidth(400);
    m_pSubjects->setMaximumWidth(400);
    m_pSubjects->setMenuMaxHeight(200);

    vLayout->addWidget(m_pSubjects, 0, Qt::AlignVCenter | Qt::AlignLeft);
    vLayout->addSpacing(50);

    QButtonGroup *pButtonGroup = new QButtonGroup(this);

    m_pFormula1Button = new QtMaterialRadioButton(this);
    m_pFormula1Button->setText("Formula 1");
    m_pFormula1Button->setLabelPosition(QtMaterialCheckable::LabelPositionRight);
    m_pFormula1Button->setMaximumHeight(50);
    m_pFormula1Button->setCheckable(true);
    m_pFormula1Button->setChecked(true);

    connect(m_pFormula1Button, &QAbstractButton::toggled, [=](bool checked) {
        if (checked) {
            onSubjectChanged(m_pSubjects->text(), 1);
        }
    });

    vLayout->addWidget(m_pFormula1Button, 0, Qt::AlignVCenter | Qt::AlignLeft);

    m_pFormula2Button = new QtMaterialRadioButton(this);
    m_pFormula2Button->setText("Formula 2");
    m_pFormula2Button->setLabelPosition(QtMaterialCheckable::LabelPositionRight);
    m_pFormula2Button->setMaximumHeight(50);

    connect(m_pFormula2Button, &QAbstractButton::toggled, [=](bool checked) {
        if (checked) {
            onSubjectChanged(m_pSubjects->text(), 2);
        }
    });

    vLayout->addWidget(m_pFormula2Button, 0, Qt::AlignVCenter | Qt::AlignLeft);

    m_pFormula3Button = new QtMaterialRadioButton(this);
    m_pFormula3Button->setText("Final Formula");
    m_pFormula3Button->setLabelPosition(QtMaterialCheckable::LabelPositionRight);
    m_pFormula3Button->setMaximumHeight(50);
    m_pFormula1Button->setCheckedColor(QColor(207, 226, 243));
    m_pFormula2Button->setCheckedColor(QColor(207, 226, 243));
    m_pFormula3Button->setCheckedColor(QColor(207, 226, 243));

    connect(m_pFormula3Button, &QAbstractButton::toggled, [=](bool checked) {
        if (checked) {
            onSubjectChanged(m_pSubjects->text(), 3);
        }
    });

    vLayout->addWidget(m_pFormula3Button, 0, Qt::AlignVCenter | Qt::AlignLeft);

    pButtonGroup->addButton(m_pFormula1Button);
    pButtonGroup->addButton(m_pFormula2Button);
    pButtonGroup->addButton(m_pFormula3Button);

    vLayout->addStretch(1);
    groupStageBox->setLayout(vLayout);
    groupStageBox->setContentsMargins(30, 10, 30, 10);

    groupStageBox->setMaximumHeight(400);
    groupStageBox->setMaximumWidth(450);

    return groupStageBox;
}

QWidget *FormulaTab::setupVariableBox()
{
    QFont font("Roboto", 12, QFont::Normal);

    groupVarBox = new QGroupBox(tr("Enter variables"), this);
    groupVarBox->setFont(font);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addSpacing(30);

    QHBoxLayout *hLayout = new QHBoxLayout(this);

    hLayout->addSpacing(30);
    QFont fontSmall("Roboto", 12, QFont::Normal);
    QLabel *varNameLabel = new QLabel(this);
    varNameLabel->setText("Variable name");
    varNameLabel->setFont(fontSmall);
    hLayout->addWidget(varNameLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);

    m_pVarName = new QtMaterialTextField(this);
    m_pVarName->setPlaceholderText("Enter variable name");
    m_pVarName->setInkColor(QColor(160, 198, 232));
    hLayout->addWidget(m_pVarName, 0, Qt::AlignLeft | Qt::AlignVCenter);

    hLayout->addSpacing(30);
    vLayout->addLayout(hLayout);
    vLayout->addSpacing(20);

    hLayout = new QHBoxLayout(this);
    hLayout->addSpacing(30);
    QLabel *periodLabel = new QLabel(this);
    periodLabel->setText("Period for analyze");
    periodLabel->setFont(fontSmall);
    hLayout->addWidget(periodLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    hLayout->addSpacing(30);

    m_pPeriodBox = new QtMaterialSelectBox(this);
    m_pPeriodBox->setInkColor(QColor(160, 198, 232));
    m_pPeriodBox->setDataSource(QStringList{"1st Module",
                                            "2nd Module",
                                            "1st Semester",
                                            "3rd Module",
                                            "4th Module",
                                            "2nd Semester"});
    m_pPeriodBox->setMinimumWidth(200);
    m_pPeriodBox->setPlaceholderText("Click to select");
    //periodBox->setMenuMaxHeight(200);
    hLayout->addWidget(m_pPeriodBox, 0, Qt::AlignLeft | Qt::AlignVCenter);
    hLayout->addSpacing(30);

    hLayout->addSpacing(30);
    vLayout->addLayout(hLayout);
    vLayout->addSpacing(20);

    hLayout = new QHBoxLayout(this);
    hLayout->addSpacing(30);
    QLabel *controlLabel = new QLabel(this);
    controlLabel->setText("Element of control");
    controlLabel->setFont(fontSmall);
    hLayout->addWidget(controlLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    hLayout->addSpacing(30);

    m_pControlBox = new QtMaterialSelectBox(this);
    m_pControlBox->setInkColor(QColor(160, 198, 232));
    m_pControlBox->setDataSource(
        QStringList{"HW", "Quiz", "Test", "Laboratory", "Classwork", "Bonus Task", "Pesentation"});
    m_pControlBox->setMinimumWidth(200);
    m_pControlBox->setPlaceholderText("Click to select");
    //controlBox->setMenuMaxHeight(200);
    hLayout->addWidget(m_pControlBox, 0, Qt::AlignLeft | Qt::AlignVCenter);
    hLayout->addSpacing(30);

    hLayout->addSpacing(30);
    vLayout->addLayout(hLayout);
    vLayout->addSpacing(20);

    hLayout = new QHBoxLayout(this);

    m_pAddButton = new QtMaterialRaisedButton(this);
    m_pAddButton->setText("Add variable");
    m_pAddButton->setRole(Material::Primary);
    m_pAddButton->setMinimumWidth(150);
    m_pAddButton->setDisabled(true);
    m_pAddButton->setBackgroundColor((QColor(207, 226, 243)));
    m_pAddButton->setForegroundColor(QColor(89, 89, 89));


    connect(m_pVarName, &QLineEdit::textChanged, [=](const QString &text) {
        m_pAddButton->setDisabled(m_pVarName->text().isEmpty() || m_pControlBox->text().isEmpty()
                                  || m_pPeriodBox->text().isEmpty());
    });

    connect(m_pPeriodBox, &QtMaterialSelectBox::itemSelected, [=](QString) {
        m_pAddButton->setDisabled(m_pVarName->text().isEmpty() || m_pControlBox->text().isEmpty()
                                  || m_pPeriodBox->text().isEmpty());
    });

    connect(m_pControlBox, &QtMaterialSelectBox::itemSelected, [=](QString) {
        m_pAddButton->setDisabled(m_pVarName->text().isEmpty() || m_pControlBox->text().isEmpty()
                                  || m_pPeriodBox->text().isEmpty());
    });

    connect(m_pAddButton, &QAbstractButton::clicked, [=](bool) {
        if (m_pVarName->text().isEmpty()) {
            m_pAddButton->setDisabled(true);
            return;
        }

        bool isNew = true;
        for (int i = 0; i < m_pTableWidget->rowCount(); i++) {
            if (m_pVarName->text() == m_pTableWidget->item(i, 1)->text()) {
                setRow(i, m_pVarName->text(), m_pPeriodBox->text(), m_pControlBox->text());
                isNew = false;
                break;
            }
        }

        if (isNew) {
            int nIndex = m_pTableWidget->rowCount();
            m_pTableWidget->setRowCount(nIndex + 1);
            setRow(nIndex, m_pVarName->text(), m_pPeriodBox->text(), m_pControlBox->text());
        }
    });

    hLayout->addSpacing(30);
    hLayout->addWidget(m_pAddButton, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    hLayout->addSpacing(30);

    vLayout->addLayout(hLayout);
    vLayout->addSpacing(50);

    setupTableWidget();
    vLayout->addWidget(m_pTableWidget);
    m_pTableWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    groupVarBox->installEventFilter(this);

    groupVarBox->setLayout(vLayout);
    groupVarBox->setContentsMargins(30, 10, 30, 10);

    groupVarBox->setMaximumHeight(800);
    groupVarBox->setMaximumWidth(600);

    return groupVarBox;
}

QWidget *FormulaTab::setupFormulaBox()
{
    QFont font("Roboto", 12, QFont::Normal);

    QGroupBox *groupVarBox = new QGroupBox(tr("Enter formula"), this);
    groupVarBox->setFont(font);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addSpacing(30);

    QHBoxLayout *hLayout = new QHBoxLayout(this);

    hLayout->addSpacing(30);
    m_pFormulaText = new QtMaterialTextField(this);
    m_pFormulaText->setPlaceholderText("Enter formula");
    m_pFormulaText->setMinimumWidth(400);
    m_pFormulaText->setLabel("Calculation formula");
    m_pFormulaText->setInkColor(QColor(160, 198, 232));
    hLayout->addWidget(m_pFormulaText, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    hLayout->addSpacing(30);
    vLayout->addLayout(hLayout);
    vLayout->addSpacing(20);

    hLayout = new QHBoxLayout(this);
    hLayout->addSpacing(30);

    m_pTestButton = new QtMaterialRaisedButton(this);
    m_pTestButton->setBackgroundColor(QColor(160, 198, 232));
    m_pTestButton->setText("Test calculate");
    m_pTestButton->setRole(Material::Default);
    m_pTestButton->setMinimumWidth(150);
    hLayout->addWidget(m_pTestButton, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    m_pTestButton->setDisabled(true);

    connect(m_pTestButton, &QAbstractButton::clicked, [=](bool) {
        QJSEngine engine;

        for (int row = 0; row < m_pTableWidget->rowCount(); row++) {
            const QString varName = m_pTableWidget->item(row, 1)->text();
            const double varTestValue = m_pTableWidget->item(row, 0)->text().toDouble();
            engine.globalObject().setProperty(varName, varTestValue);
        }

        QJSValue function = engine.evaluate(m_pFormulaText->text());
        if (function.isError()) {
            m_pResultLabel->setText(function.toString());
        } else {
            m_pResultLabel->setText(QString("%1").arg(function.toNumber()));
        }
    });

    hLayout->addSpacing(30);

    m_pSaveButton = new QtMaterialRaisedButton(this);
    m_pSaveButton->setText("Save settings");
    m_pSaveButton->setRole(Material::Default);
    m_pSaveButton->setMinimumWidth(150);
    hLayout->addWidget(m_pSaveButton, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    m_pSaveButton->setDisabled(true);

    m_pTestButton->setBackgroundColor(QColor(207, 226, 243));
    m_pTestButton->setForegroundColor(QColor(89, 89, 89));

    m_pSaveButton->setBackgroundColor(QColor(207, 226, 243));
    m_pSaveButton->setForegroundColor(QColor(89, 89, 89));


    hLayout->addSpacing(30);

    connect(m_pFormulaText, &QLineEdit::textChanged, [=](const QString &) {
        m_pTestButton->setDisabled(m_pFormulaText->text().isEmpty());
        m_pSaveButton->setDisabled(m_pFormulaText->text().isEmpty());
    });

    connect(m_pSaveButton, &QAbstractButton::clicked, [=](bool) {
        FormulaDto *dto = new FormulaDto();

        dto->setFormula(m_pFormulaText->text());
        QList<VariableDto> variables;

        for (int row = 0; row < m_pTableWidget->rowCount(); row++) {
            VariableDto varDto;
            varDto.setTestValue(m_pTableWidget->item(row, 0)->text().toDouble());
            varDto.setVarName(m_pTableWidget->item(row, 1)->text());
            varDto.setPeriod(m_pTableWidget->item(row, 2)->text());
            varDto.setElementOfControl(m_pTableWidget->item(row, 3)->text());
            variables.append(varDto);
        }
        dto->setVariables(variables);

        QString json(dto->toJson());

        for (std::shared_ptr<Fcs> fcs : fcsList) {
            if (fcs->getSubject() == m_pSubjects->text() && fcs->getLecturer() == teacherLogin) {
                Fcs *fcsPtr = fcs.get();

                if (m_pFormula1Button->isChecked()) {
                    formula1 = std::unique_ptr<FormulaDto>(dto);
                    fcsPtr->setFormula1(json);
                } else if (m_pFormula2Button->isChecked()) {
                    formula2 = std::unique_ptr<FormulaDto>(dto);
                    fcsPtr->setFormula2(json);
                } else {
                    formula3 = std::unique_ptr<FormulaDto>(dto);
                    fcsPtr->setFormulaFinal(json);
                }

                if (fcsRepository->update(*fcs)) {
                    MainAppWindow::instance().postMessage("Successfully saved");
                } else {
                    MainAppWindow::instance().postError("Errors occured at save. Try again later.");
                }
                break;
            }
        }
    });

    vLayout->addLayout(hLayout);
    vLayout->addSpacing(20);

    hLayout = new QHBoxLayout(this);

    hLayout->addSpacing(30);
    QFont fontSmall("Roboto", 12, QFont::Normal);
    QLabel *varNameLabel = new QLabel(this);
    varNameLabel->setText("Calculation result: ");
    varNameLabel->setFont(fontSmall);
    hLayout->addWidget(varNameLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);

    m_pResultLabel = new QLabel(this);
    m_pResultLabel->setWordWrap(true);
    m_pResultLabel->setFont(fontSmall);
    hLayout->addWidget(m_pResultLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);

    hLayout->addSpacing(30);

    vLayout->addLayout(hLayout);
    vLayout->addSpacing(20);

    vLayout->addStretch(1);

    groupVarBox->setLayout(vLayout);
    groupVarBox->setContentsMargins(30, 10, 30, 10);

    groupVarBox->setMaximumHeight(345);
    groupVarBox->setMaximumWidth(450);

    return groupVarBox;
}

void FormulaTab::setupTableWidget()
{
    m_pTableWidget = new QTableWidget(this);

    QtMaterialScrollBar *verticalScrollbar = new QtMaterialScrollBar(this);
    QtMaterialScrollBar *horizontalScrollbar = new QtMaterialScrollBar(this);

    horizontalScrollbar->setOrientation(Qt::Horizontal);
    horizontalScrollbar->setHideOnMouseOut(false);

    verticalScrollbar->setHideOnMouseOut(false);

    //new
    horizontalScrollbar->setSliderColor(QColor(204, 204, 204));
    verticalScrollbar->setSliderColor(QColor(204, 204, 204));
    //new

    QFont font("Roboto", 10, QFont::Normal);

    //new
    m_pTableWidget->setStyleSheet(
        "QTableView::item:selected { background-color: rgb(207, 226, 243); }"
        "QTableCornerButton::section{"
        "border-top:0px solid #D8D8D8;"
        "border-left:0px solid #D8D8D8;"
        "border-right:1px solid #D8D8D8;"
        "border-bottom: 1px solid #D8D8D8;"
        "background-color:white;"
        "}");
    //new

    m_pTableWidget->setFont(font);

    m_pTableWidget->setVerticalScrollBar(verticalScrollbar);
    m_pTableWidget->setHorizontalScrollBar(horizontalScrollbar);

    m_pTableWidget->setFrameShape(QFrame::NoFrame);

    QStringList headerList({"Test Value", "Variable", "Time period", "Element of control", ""});
    m_pTableWidget->setColumnCount(headerList.size());

    m_pTableWidget->setColumnWidth(0, 50);
    m_pTableWidget->setColumnWidth(1, 50);
    m_pTableWidget->setColumnWidth(2, 100);
    m_pTableWidget->setColumnWidth(3, 100);
    m_pTableWidget->setColumnWidth(4, 30);

    m_pTableWidget->setHorizontalHeaderLabels(headerList);
    m_pTableWidget->horizontalHeader()->setFont(font);

    m_pTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{"
                                                      "border-top:1px solid #D8D8D8;"
                                                      "border-left:0px solid #D8D8D8;"
                                                      "border-right:1px solid #D8D8D8;"
                                                      "border-bottom: 2px solid #D8D8D8;"
                                                      "background-color:white;"
                                                      "height: 32px;"
                                                      "padding-top:10px;"
                                                      "padding-bottom:10px;"
                                                      "}");

    m_pTableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_pTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_pTableWidget->horizontalHeader()->setSectionsClickable(false);

    m_pTableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    m_pTableWidget->verticalHeader()->setFont(font);
    m_pTableWidget->verticalHeader()->setSectionsClickable(false);

    m_pTableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{"
                                                    "border-top:0px solid #D8D8D8;"
                                                    "border-left:1px solid #D8D8D8;"
                                                    "border-right:2px solid #D8D8D8;"
                                                    "border-bottom: 1px solid #D8D8D8;"
                                                    "background-color:white;"
                                                    "padding-left:10px;"
                                                    "padding-right:10px;"
                                                    "}");

    m_pTableWidget->verticalHeader()->setContentsMargins(20, 0, 20, 0);

    m_pTableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_pTableWidget->setWordWrap(false);

    m_pTableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter
                                                            | (Qt::Alignment) Qt::TextWordWrap);

    m_pTableWidget->horizontalHeader()->setStretchLastSection(true);
    m_pTableWidget->setSortingEnabled(true);

    m_pTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_pTableWidget->setShowGrid(true);

    connect(m_pTableWidget, &QTableWidget::currentCellChanged, [=](int, int) {

    });

    connect(m_pTableWidget,
            SIGNAL(cellChanged(int, int)),
            this,
            SLOT(onVariableCellChanged(int, int)));

    connect(m_pTableWidget,
            &QTableWidget::currentCellChanged,
            [=](int currentRow, int, int previousRow, int) {
                if (currentRow != previousRow) {
                    m_pVarName->setText(m_pTableWidget->item(currentRow, 1)->text());
                    m_pPeriodBox->setText(m_pTableWidget->item(currentRow, 2)->text());
                    m_pControlBox->setText(m_pTableWidget->item(currentRow, 3)->text());
                }
            });
}

void FormulaTab::addTableItem(int row, int column, const QString &itemText, bool isEditable)
{
    QTableWidgetItem *item = new QTableWidgetItem(itemText);
    if (!isEditable) {
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }

    item->setTextAlignment(Qt::AlignCenter);
    item->setBackground((row & 1) ? *evenColor : *oddColor);

    m_pTableWidget->setItem(row, column, item);
}

void FormulaTab::setRow(int rowIndex,
                        const QString &variableName,
                        const QString &period,
                        const QString &elementOfControl)
{
    QTableWidgetItem *pItem = m_pTableWidget->item(rowIndex, 0);
    bool parseOk;
    double testValue = 1.0;
    if (pItem != nullptr && !pItem->text().isEmpty()) {
        pItem->text().toDouble(&parseOk);
        if (parseOk) {
            testValue = pItem->text().toDouble();
        }
    }

    addTableItem(rowIndex, 0, QString(QString::number(testValue, 'g', 2)), true);
    addTableItem(rowIndex, 1, variableName, false);
    addTableItem(rowIndex, 2, period, false);
    addTableItem(rowIndex, 3, elementOfControl, false);

    QtMaterialIconButton *iconButton
        = new QtMaterialIconButton(QIcon(":/icons/icons/action/svg/production/ic_delete_24px.svg"),
                                   this);
    m_pTableWidget->setCellWidget(rowIndex, 4, iconButton);
    m_pTableWidget->setRowHeight(rowIndex, 30);

    connect(iconButton, &QtMaterialIconButton::clicked, [=]() {
        m_pTableWidget->blockSignals(true);
        for (int row = 0; row < m_pTableWidget->rowCount(); row++) {
            if (m_pTableWidget->item(row, 1)->text() == variableName) {
                m_pTableWidget->removeRow(row);
                break;
            }
        }
        m_pTableWidget->blockSignals(false);
    });
}

bool FormulaTab::eventFilter(QObject *watched, QEvent *event)
{
    if (groupVarBox == watched) {
        switch (event->type()) {
        case QEvent::Move:
        case QEvent::Resize: {
            QRect groupBoxGeometry = groupVarBox->geometry();
            QRect ftRect = m_pTableWidget->geometry();

            m_pTableWidget->resize(m_pTableWidget->width(),
                                   groupBoxGeometry.height() - (ftRect.top() + 60));
            //formulaTableWidget->setGeometry(groupBoxGeometry);
            break;
        }
        default:
            break;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void FormulaTab::setupTabLayout()
{
    m_pFormulaLayout = new QHBoxLayout(this);

    QFont font("Roboto", 16, QFont::Normal);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);

    vBoxLayout->addWidget(setupStageBox());
    vBoxLayout->addWidget(setupFormulaBox());

    m_pFormulaLayout->addStretch();
    m_pFormulaLayout->addLayout(vBoxLayout);
    m_pFormulaLayout->addSpacing(50);
    m_pFormulaLayout->addWidget(setupVariableBox());
    m_pFormulaLayout->addStretch();

    setLayout(m_pFormulaLayout);
    setContentsMargins(0, 0, 0, 0);

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(policy);

    GetTeacher *teacherGet = new GetTeacher(teacherLogin, this);
    connect(teacherGet, &GetTeacher::result, [=](const QList<std::shared_ptr<Teacher>> &teachers) {
        FcsGetLecturer *getLecturer = new FcsGetLecturer(teachers, this);
        connect(getLecturer,
                &FcsGetLecturer::result,
                [=](const QList<std::shared_ptr<Fcs>> &result) {
                    fcsList = result;
                    QStringList subjects;
                    for (std::shared_ptr<Fcs> fcs : result) {
                        subjects.append(fcs->getSubject());
                    }

                    if (!subjects.isEmpty()) {
                        m_pSubjects->setDataSource(subjects);
                        m_pSubjects->setText(result.first()->getSubject());

                        onSubjectChanged(result.first()->getSubject(), 1);
                    }
                });
        getLecturer->start();
    });
    teacherGet->start();

    connect(m_pSubjects, &QtMaterialSelectBox::itemSelected, [=](QString selectedText) {
        onSubjectChanged(selectedText, 1);
    });
}

void FormulaTab::onSubjectChanged(QString subject, int formulaIndex)
{
    for (std::shared_ptr<Fcs> fcs : fcsList) {
        if (fcs->getSubject() == subject && fcs->getLecturer() == teacherLogin) {
            formula1 = std::unique_ptr<FormulaDto>(new FormulaDto(fcs->getFormula1().toUtf8()));
            formula2 = std::unique_ptr<FormulaDto>(new FormulaDto(fcs->getFormula2().toUtf8()));
            formula3 = std::unique_ptr<FormulaDto>(new FormulaDto(fcs->getFormulaFinal().toUtf8()));

            m_pVarName->setText("");
            m_pPeriodBox->setText("");
            m_pControlBox->setText("");

            FormulaDto *pFormula;
            switch (formulaIndex) {
            case 1:
                m_pFormula1Button->blockSignals(true);
                m_pFormula1Button->setChecked(true);
                m_pFormula1Button->blockSignals(false);

                m_pFormula2Button->setChecked(false);
                m_pFormula3Button->setChecked(false);

                m_pControlBox->setDataSource(QStringList{
                    "HW", "Quiz", "Test", "Laboratory", "Classwork", "Bonus Task", "Pesentation"});
                pFormula = formula1.get();
                break;
            case 2:
                m_pFormula2Button->blockSignals(true);
                m_pFormula2Button->setChecked(true);
                m_pFormula2Button->blockSignals(false);

                m_pFormula1Button->setChecked(false);
                m_pFormula3Button->setChecked(false);

                m_pControlBox->setDataSource(QStringList{
                    "HW", "Quiz", "Test", "Laboratory", "Classwork", "Bonus Task", "Pesentation"});
                pFormula = formula2.get();
                break;
            case 3:
                m_pFormula3Button->blockSignals(true);
                m_pFormula3Button->setChecked(true);
                m_pFormula3Button->blockSignals(false);

                m_pFormula1Button->setChecked(false);
                m_pFormula2Button->setChecked(false);

                m_pControlBox->setDataSource(QStringList{"HW",
                                                         "Quiz",
                                                         "Test",
                                                         "Laboratory",
                                                         "Classwork",
                                                         "Bonus Task",
                                                         "Pesentation",
                                                         "Formula"});
                m_pControlBox->setText(QString("Formula"));
                pFormula = formula3.get();
                break;
            }

            m_pFormulaText->setText(pFormula->getFormula());
            m_pResultLabel->setText("");

            // init tablewidget from saved formula
            m_pTableWidget->clearContents();
            m_pTableWidget->setRowCount(pFormula->getVariables().size());
            for (int i = 0; i < pFormula->getVariables().size(); i++) {
                VariableDto dto = pFormula->getVariables().at(i);
                setRow(i, dto.getVarName(), dto.getPeriod(), dto.getElementOfControl());
            }
        }
    }
}
