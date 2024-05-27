#include "StudentFormulaTab.h"
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJSEngine>
#include <QList>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include "qtmaterialdivider.h"
#include <qlabel.h>
#include <qtmaterialcard.h>
#include <qtmaterialraisedbutton.h>
#include <qtmaterialscrollbar.h>
#include <qtmaterialtextfield.h>

#include "../ProfileEditDialog.h"
#include "../mainappwindow.h"
#include "formuladto.h"
#include "workers/fcsgetprogram.h"
#include "workers/getgradesforstudent.h"
#include "workers/getstudent.h"
#include "workers/getsubjects.h"

StudentFormulaTab::StudentFormulaTab(const QString &studentLogin, QWidget *parent)
    : QWidget{parent}
    , m_pReloadButton(new QtMaterialRaisedButton(this))
    , m_pSubjectsArea(new QScrollArea(this))
    , m_studentLogin(studentLogin)
    , oddColor(new QBrush(QColor(240, 240, 240)))
    , evenColor(new QBrush(Qt::GlobalColor::white))
    , gradeRepository(RepositoryManager::instance().gradeRepository())
    , m_pEditProfile(nullptr)
{}

void StudentFormulaTab::setupTabLayout()
{
    m_pGradeLayout = new QHBoxLayout(this);

    QWidget *scrollAreaHolder = new QWidget(this);
    QVBoxLayout *scrollAreaLayout = new QVBoxLayout(scrollAreaHolder);
    scrollAreaLayout->setAlignment(Qt::AlignTop);

    m_pSubjectsArea->setWidget(scrollAreaHolder);
    m_pSubjectsArea->setWidgetResizable(true);

    QtMaterialScrollBar *verticalScrollbar = new QtMaterialScrollBar(this);
    QtMaterialScrollBar *horizontalScrollbar = new QtMaterialScrollBar(this);

    verticalScrollbar->setSliderColor(QColor(207, 226, 243));
    horizontalScrollbar->setSliderColor(QColor(207, 226, 243));

    horizontalScrollbar->setOrientation(Qt::Horizontal);

    horizontalScrollbar->setHideOnMouseOut(false);
    verticalScrollbar->setHideOnMouseOut(false);

    m_pSubjectsLayout = new QVBoxLayout(this);

    m_pSubjectsArea->setVerticalScrollBar(verticalScrollbar);
    m_pSubjectsArea->setHorizontalScrollBar(horizontalScrollbar);
    m_pSubjectsArea->setFrameShape(QFrame::NoFrame);
    m_pSubjectsArea->setMinimumWidth(300);
    m_pSubjectsArea->setMinimumHeight(500);

    m_pSubjectsLayout->addSpacing(20);

    m_pSubjectsLayout->addWidget(m_pSubjectsArea, 1, Qt::AlignTop);
    m_pSubjectsLayout->addStretch(1);

    m_pGradeLayout->addLayout(m_pSubjectsLayout);
    m_pGradeLayout->addLayout(setupGradeTableLayout(), 100);

    setLayout(m_pGradeLayout);
    setContentsMargins(0, 0, 0, 0);

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(policy);

    GetStudent *getStudent = new GetStudent(m_studentLogin);
    connect(getStudent, &GetStudent::result, this, [=](const std::shared_ptr<Student> student) {
        if (student == nullptr) {
            MainAppWindow::instance().postError("Failed receive student");
            return;
        }

        m_student = student;

        FcsGetProgram *getProgram = new FcsGetProgram(QString());
        connect(getProgram, &FcsGetProgram::result, this, &StudentFormulaTab::updateStudentCard);
        getProgram->start();

        GetSubjects *getSubjects = new GetSubjects(student->getProgram(),
                                                   student->getPilot(),
                                                   student->getCourse());
        connect(getSubjects, &GetSubjects::result, this, &StudentFormulaTab::updateSubjects);
        getSubjects->start();
    });
    getStudent->start();

    GetGradesForStudent *getGrades = new GetGradesForStudent(m_studentLogin);
    connect(getGrades, &GetGradesForStudent::result, this, &StudentFormulaTab::redrawGrades);
    getGrades->start();
}

QVBoxLayout *StudentFormulaTab::addStage(int stage)
{
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);

    vBoxLayout->addSpacing(10);

    QtMaterialTextField *pFormula1Label = new QtMaterialTextField(this);
    pFormula1Label->setInkColor(QColor(160, 198, 232));
    pFormula1Label->setText(QString("Stage %1").arg(stage));
    pFormula1Label->setLabel("Metrics");
    pFormula1Label->setLabelFontSize(6);
    QFont font("Roboto", 14, QFont::Normal);
    pFormula1Label->setFont(font);
    pFormula1Label->setReadOnly(true);

    vBoxLayout->addWidget(pFormula1Label);
    vBoxLayout->addSpacing(20);

    QTableWidget *tableWidget = createFormulaTableWidget();

    vBoxLayout->addWidget(tableWidget, 1);
    vBoxLayout->addSpacing(20);
    QtMaterialDivider *divider = new QtMaterialDivider(this);
    vBoxLayout->addWidget(divider);

    QtMaterialTextField *pFormulaResult = new QtMaterialTextField(this);
    pFormulaResult->setInkColor(QColor(160, 198, 232));
    pFormulaResult->setText("No data");
    pFormulaResult->setMaximumWidth(100);
    pFormulaResult->setReadOnly(true);
    pFormulaResult->setAlignment(Qt::AlignHCenter);
    addHeaderField(vBoxLayout, "Result: ", pFormulaResult, 100);
    vBoxLayout->addStretch();

    m_results[stage - 1] = pFormulaResult;
    m_tables[stage - 1] = tableWidget;

    return vBoxLayout;
}

QLayout *StudentFormulaTab::setupGradeTableLayout()
{
    QVBoxLayout *tableLayout = new QVBoxLayout(this);
    QHBoxLayout *searchLayout = new QHBoxLayout(this);
    m_pReloadButton->setIcon(QIcon(":/icons/icons/action/svg/production/ic_autorenew_24px.svg"));
    m_pReloadButton->setText("Reload");
    m_pReloadButton->setMinimumWidth(150);
    m_pReloadButton->setMaximumWidth(150);
    m_pReloadButton->setMaximumHeight(20);
    m_pReloadButton->setRole(Material::Role::Primary);
    m_pReloadButton->setBackgroundColor(QColor(204, 204, 204));
    m_pReloadButton->setForegroundColor(QColor(89, 89, 89));

    searchLayout->addWidget(m_pReloadButton, 0, Qt::AlignVCenter | Qt::AlignRight);
    searchLayout->setContentsMargins(55, 20, 20, 20);

    connect(m_pReloadButton, &QAbstractButton::clicked, [=] {
        GetGradesForStudent *getGrades = new GetGradesForStudent(m_studentLogin);
        connect(getGrades, &GetGradesForStudent::result, this, &StudentFormulaTab::redrawGrades);
        getGrades->start();
    });

    tableLayout->addLayout(searchLayout);

    QHBoxLayout *formulaLayout = new QHBoxLayout(this);
    formulaLayout->addSpacing(20);
    formulaLayout->addLayout(addStage(1));
    formulaLayout->addSpacing(20);
    formulaLayout->addLayout(addStage(2));
    formulaLayout->addSpacing(20);
    formulaLayout->addLayout(addStage(3));
    formulaLayout->addSpacing(20);

    tableLayout->addLayout(formulaLayout);
    tableLayout->setContentsMargins(0, 0, 10, 0);

    return tableLayout;
}

void StudentFormulaTab::updateStudentCard(const QList<std::shared_ptr<Fcs>> &fcsList)
{
    QMap<QString, QString> programToName;

    for (std::shared_ptr<Fcs> fcs : fcsList) {
        programToName[fcs->getProgram()] = fcs->getProgramName();
    }

    Student *student = m_student.get();

    QImage studentImage = QImage();
    studentImage.loadFromData(student->getAvatar());

    m_pStudentCard
        = new QtMaterialCard(72,
                             studentImage,
                             QString("%1 %2").arg(student->getName()).arg(student->getSurname()),
                             QString("Student, %1, %2-%3")
                                 .arg(programToName[student->getProgram()] == nullptr
                                          ? student->getProgram()
                                          : programToName[student->getProgram()])
                                 .arg(student->getGroup())
                                 .arg(student->getSubgroup()),
                             true,
                             false,
                             QColor(),
                             QList<QWidget *>(),
                             this);

    m_pStudentCard->setDescriptionColor(QColor(107, 126, 143));
    m_pStudentCard->setMinimumHeight(75);
    m_pStudentCard->setMinimumWidth(300);
    m_pStudentCard->setParent(m_pSubjectsArea->widget());

    connect(m_pStudentCard, &QtMaterialCard::toolClicked, [=] {
        if (m_pEditProfile == nullptr) {
            m_pEditProfile = new ProfileEditDialog(this);
            m_pEditProfile->setupForm(QImage(studentImage));

            connect(m_pEditProfile, &ProfileEditDialog::doApply, [=]() {
                qDebug() << "doApply";
                m_pStudentCard->setImage(m_pEditProfile->getImage());
                MainAppWindow::instance().hideDialog(m_pEditProfile);
                m_pEditProfile->deleteLater();
                m_pEditProfile = nullptr;
            });

            connect(m_pEditProfile, &ProfileEditDialog::doCancel, [=]() {
                qDebug() << "docancel";
                MainAppWindow::instance().hideDialog(m_pEditProfile);
                m_pEditProfile->deleteLater();
                m_pEditProfile = nullptr;
            });

            connect(m_pEditProfile, &ProfileEditDialog::doHelp, [=]() {
                qDebug() << "dohelp";
                MainAppWindow::instance().help("dummyhelp.pdf");
            });

            MainAppWindow::instance().showDialog(m_pEditProfile);
        }
    });

    m_pSubjectsLayout->insertSpacing(0, 50);
    m_pSubjectsLayout->insertWidget(0, new QtMaterialDivider(this), 0, Qt::AlignTop);
    m_pSubjectsLayout->insertWidget(0, m_pStudentCard, 1, Qt::AlignTop);
}

void StudentFormulaTab::updateSubjects(const QList<std::shared_ptr<Fcs>> &fcsList)
{
    qDebug() << "getassist start " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs);

    this->m_fcsList = fcsList;

    m_pButtonGroup = new QButtonGroup(this);
    m_pButtonGroup->setExclusive(true);

    QStringList subjects;
    QMap<QAbstractButton *, std::shared_ptr<Fcs>> prevMap(m_buttonToMap);

    for (std::shared_ptr<Fcs> fcs : m_fcsList) {
        if (subjects.contains(fcs->getSubject())
            || (m_student != nullptr && fcs->getProgram() != m_student->getProgram())) {
            continue;
        }

        bool subjectFound = false;
        QMapIterator<QAbstractButton *, std::shared_ptr<Fcs>> btnIterator(m_buttonToMap);

        while (btnIterator.hasNext()) {
            btnIterator.next();
            if (btnIterator.value()->getSubject() == fcs->getSubject()) {
                prevMap.remove(btnIterator.key());
                subjectFound = true;
                break;
            }
        }

        if (!subjectFound) {
            subjects.append(fcs->getSubject());

            QtMaterialRaisedButton *subjectButton = new QtMaterialRaisedButton(this);
            subjectButton->setText(fcs->getSubject());
            subjectButton->setCheckable(true);
            subjectButton->setBackgroundColor(QColor(204, 204, 204));
            subjectButton->setForegroundColor(QColor(89, 89, 89));

            m_pButtonGroup->addButton(subjectButton);
            m_buttonToMap[subjectButton] = fcs;

            ((QVBoxLayout *) m_pSubjectsArea->widget()->layout())->addWidget(subjectButton);
            ((QVBoxLayout *) m_pSubjectsArea->widget()->layout())->addSpacing(20);
        }
    }

    ((QVBoxLayout *) m_pSubjectsArea->widget()->layout())->addStretch(10);

    QMapIterator<QAbstractButton *, std::shared_ptr<Fcs>> btnIterator(prevMap);
    while (btnIterator.hasNext()) {
        btnIterator.next();
        m_pButtonGroup->removeButton(btnIterator.key());
    }

    connect(m_pButtonGroup, &QButtonGroup::buttonClicked, [=](QAbstractButton *pButton) {
        std::shared_ptr<Fcs> fcs = m_buttonToMap[pButton];
        if (fcs != nullptr) {
            qDebug() << "clicked on " << fcs->getSubject() << "checked is " << pButton->isChecked();
            redrawGrades(m_gradeList);
        }
    });

    if (!m_buttonToMap.isEmpty()) {
        QTimer::singleShot(100, [=] {
            m_buttonToMap.firstKey()->setChecked(true);
            redrawGrades(m_gradeList);
        });
    }
}

double StudentFormulaTab::calculatePeriod(const QString &subject,
                                          const QString &period,
                                          const QString &controlType,
                                          const QList<double> &rawResult)
{
    QStringList modules;
    if (period == "1st Semester") {
        if (controlType != "Formula") {
            modules.append("1st Module");
            modules.append("2nd Module");
            modules.append(period);
        } else {
            return rawResult.size() == 0 ? 0.0 : rawResult.at(0);
        }

    } else if (period == "2nd Semester") {
        if (controlType != "Formula") {
            modules.append("3rd Module");
            modules.append("4th Module");
            modules.append(period);
        } else {
            return rawResult.size() < 2 ? 0.0 : rawResult.at(1);
        }

    } else {
        modules.append(period);
    }

    if (controlType == "Formula") {
        return 0.0;
    }

    double agg = 0.0;
    int count = 0;
    for (int i = 0; i < m_gradeList.size(); i++) {
        std::shared_ptr<Grade> grade = m_gradeList.at(i);
        if (grade->getSubject() == subject && modules.contains(grade->getModule())
            && controlType == grade->getElementOfControl()) {
            agg += grade->getMark();
            count++;
        }
    }

    return agg / qMax(1.0, double(count));
}

void StudentFormulaTab::fillTableData(const QString &subject,
                                      const FormulaDto &formula,
                                      QTableWidget *table,
                                      QtMaterialTextField *result,
                                      QList<double> &rawResult)
{
    table->setSpan(0, 0, 1, 1);
    QJSEngine engine;
    table->blockSignals(true);
    result->setText("Calculating...");

    table->setRowCount(formula.getVariables().size());
    for (int i = 0; i < formula.getVariables().size(); i++) {
        VariableDto variable = formula.getVariables().at(i);
        addTableItem(table, i, 0, variable.getPeriod(), false);
        addTableItem(table, i, 1, variable.getElementOfControl(), false);

        double periodValue = calculatePeriod(subject,
                                             variable.getPeriod(),
                                             variable.getElementOfControl(),
                                             rawResult);
        engine.globalObject().setProperty(variable.getVarName(), periodValue);

        addTableItem(table, i, 2, QString::number(periodValue), false);
        addTableItem(table, i, 3, "", true);
    }

    QJSValue function = engine.evaluate(formula.getFormula());
    if (function.isError()) {
        result->setText(function.toString());
        rawResult.append(0.0);
    } else {
        result->setText(QString("%1").arg(function.toNumber()));
        rawResult.append(function.toNumber());
    }

    table->blockSignals(false);
}

void StudentFormulaTab::calcTableData(const QString &subject,
                                      const FormulaDto &formula,
                                      QTableWidget *table,
                                      QtMaterialTextField *result,
                                      QList<double> &rawResult)
{
    table->setSpan(0, 0, 1, 1);
    QJSEngine engine;
    table->blockSignals(true);
    result->setText("Calculating...");

    table->setRowCount(formula.getVariables().size());
    for (int i = 0; i < formula.getVariables().size(); i++) {
        VariableDto variable = formula.getVariables().at(i);
        table->item(i, 0)->setText(variable.getPeriod());
        table->item(i, 1)->setText(variable.getElementOfControl());

        double periodValue = calculatePeriod(subject,
                                             variable.getPeriod(),
                                             variable.getElementOfControl(),
                                             rawResult);
        table->item(i, 2)->setText(QString::number(periodValue));
        if (!table->item(i, 3)->text().isNull() && !table->item(i, 3)->text().isEmpty()) {
            bool ok;
            double testValue = table->item(i, 3)->text().toDouble(&ok);
            if (ok) {
                periodValue = testValue;
            }
        }

        engine.globalObject().setProperty(variable.getVarName(), periodValue);
    }

    QJSValue function = engine.evaluate(formula.getFormula());
    if (function.isError()) {
        result->setText(function.toString());
        rawResult.append(0.0);
    } else {
        result->setText(QString("%1").arg(function.toNumber()));
        rawResult.append(function.toNumber());
    }

    table->blockSignals(false);
}

void StudentFormulaTab::recalcGrades(const QList<std::shared_ptr<Grade>> &gradeList)
{
    m_gradeList = gradeList;
    std::shared_ptr<Fcs> selectedFcs;
    for (auto [k, v] : m_buttonToMap.asKeyValueRange()) {
        if (k->isChecked()) {
            selectedFcs = v;
            break;
        }
    }

    if (selectedFcs == nullptr) {
        for (int i = 0; i < 3; i++) {
            m_tables[i]->blockSignals(true);
            m_tables[i]->clearContents();
            m_tables[i]->setRowCount(1);

            QTableWidgetItem *pItem = new QTableWidgetItem("No data available");
            m_tables[i]->setItem(0, 0, pItem);
            m_tables[i]->setSpan(0, 0, 1, 4);
            pItem->setTextAlignment(Qt::AlignCenter);

            m_results[i]->setText("No data");
            m_tables[i]->blockSignals(false);
        }

        return;
    }

    Fcs *fcs = selectedFcs.get();
    QList<double> rawResult;
    if (selectedFcs->getFormula1().isNull() || selectedFcs->getFormula1().isEmpty()) {
        m_tables[0]->blockSignals(true);
        m_tables[0]->clearContents();
        m_tables[0]->setRowCount(1);

        QTableWidgetItem *pItem = new QTableWidgetItem("No formula available");
        m_tables[0]->setItem(0, 0, pItem);
        m_tables[0]->setSpan(0, 0, 1, 4);
        pItem->setTextAlignment(Qt::AlignCenter);

        m_results[0]->setText("No formula");
        m_tables[0]->blockSignals(false);
        rawResult.append(0.0);
    } else {
        calcTableData(fcs->getSubject(),
                      FormulaDto(fcs->getFormula1().toUtf8()),
                      m_tables[0],
                      m_results[0],
                      rawResult);
    }

    if (selectedFcs->getFormula2().isNull() || selectedFcs->getFormula2().isEmpty()) {
        m_tables[1]->blockSignals(true);
        m_tables[1]->clearContents();
        m_tables[1]->setRowCount(1);

        QTableWidgetItem *pItem = new QTableWidgetItem("No formula available");
        m_tables[1]->setItem(0, 0, pItem);
        m_tables[1]->setSpan(0, 0, 1, 4);
        pItem->setTextAlignment(Qt::AlignCenter);

        m_results[1]->setText("No formula");
        m_tables[1]->blockSignals(false);
        rawResult.append(0.0);
    } else {
        calcTableData(fcs->getSubject(),
                      FormulaDto(fcs->getFormula2().toUtf8()),
                      m_tables[1],
                      m_results[1],
                      rawResult);
    }

    if (selectedFcs->getFormulaFinal().isNull() || selectedFcs->getFormulaFinal().isEmpty()) {
        m_tables[2]->blockSignals(true);
        m_tables[2]->clearContents();
        m_tables[2]->setRowCount(1);

        QTableWidgetItem *pItem = new QTableWidgetItem("No formula available");
        m_tables[2]->setItem(0, 0, pItem);
        m_tables[2]->setSpan(0, 0, 1, 4);
        pItem->setTextAlignment(Qt::AlignCenter);

        m_results[2]->setText("No formula");
        m_tables[2]->blockSignals(false);
        rawResult.append(0.0);
    } else {
        calcTableData(fcs->getSubject(),
                      FormulaDto(fcs->getFormulaFinal().toUtf8()),
                      m_tables[2],
                      m_results[2],
                      rawResult);
    }
}

void StudentFormulaTab::redrawGrades(const QList<std::shared_ptr<Grade>> &gradeList)
{
    m_gradeList = gradeList;
    std::shared_ptr<Fcs> selectedFcs;
    for (auto [k, v] : m_buttonToMap.asKeyValueRange()) {
        if (k->isChecked()) {
            selectedFcs = v;
            break;
        }
    }

    if (selectedFcs == nullptr) {
        for (int i = 0; i < 3; i++) {
            m_tables[i]->blockSignals(true);
            m_tables[i]->clearContents();
            m_tables[i]->setRowCount(1);

            QTableWidgetItem *pItem = new QTableWidgetItem("No data available");
            m_tables[i]->setItem(0, 0, pItem);
            m_tables[i]->setSpan(0, 0, 1, 4);
            pItem->setTextAlignment(Qt::AlignCenter);

            m_results[i]->setText("No data");
            m_tables[i]->blockSignals(false);
        }

        return;
    }

    qDebug() << "selected fcs is " << selectedFcs->getSubject();

    Fcs *fcs = selectedFcs.get();
    QList<double> rawResult;
    if (selectedFcs->getFormula1().isNull() || selectedFcs->getFormula1().isEmpty()) {
        m_tables[0]->blockSignals(true);
        m_tables[0]->clearContents();
        m_tables[0]->setRowCount(1);

        QTableWidgetItem *pItem = new QTableWidgetItem("No formula available");
        m_tables[0]->setItem(0, 0, pItem);
        m_tables[0]->setSpan(0, 0, 1, 4);
        pItem->setTextAlignment(Qt::AlignCenter);

        m_results[0]->setText("No formula");
        m_tables[0]->blockSignals(false);
        rawResult.append(0.0);
    } else {
        fillTableData(fcs->getSubject(),
                      FormulaDto(fcs->getFormula1().toUtf8()),
                      m_tables[0],
                      m_results[0],
                      rawResult);
    }

    if (selectedFcs->getFormula2().isNull() || selectedFcs->getFormula2().isEmpty()) {
        m_tables[1]->blockSignals(true);
        m_tables[1]->clearContents();
        m_tables[1]->setRowCount(1);

        QTableWidgetItem *pItem = new QTableWidgetItem("No formula available");
        m_tables[1]->setItem(0, 0, pItem);
        m_tables[1]->setSpan(0, 0, 1, 4);
        pItem->setTextAlignment(Qt::AlignCenter);

        m_results[1]->setText("No formula");
        m_tables[1]->blockSignals(false);
        rawResult.append(0.0);
    } else {
        fillTableData(fcs->getSubject(),
                      FormulaDto(fcs->getFormula2().toUtf8()),
                      m_tables[1],
                      m_results[1],
                      rawResult);
    }

    if (selectedFcs->getFormulaFinal().isNull() || selectedFcs->getFormulaFinal().isEmpty()) {
        m_tables[2]->blockSignals(true);
        m_tables[2]->clearContents();
        m_tables[2]->setRowCount(1);

        QTableWidgetItem *pItem = new QTableWidgetItem("No formula available");
        m_tables[2]->setItem(0, 0, pItem);
        m_tables[2]->setSpan(0, 0, 1, 4);
        pItem->setTextAlignment(Qt::AlignCenter);

        m_results[2]->setText("No formula");
        m_tables[2]->blockSignals(false);
        rawResult.append(0.0);
    } else {
        fillTableData(fcs->getSubject(),
                      FormulaDto(fcs->getFormulaFinal().toUtf8()),
                      m_tables[2],
                      m_results[2],
                      rawResult);
    }
}

QTableWidgetItem *StudentFormulaTab::addTableItem(
    QTableWidget *tableWidget, int row, int column, const QString &text, bool isEditable)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    if (!isEditable) {
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }

    item->setTextAlignment(Qt::AlignCenter);
    item->setBackground((row & 1) ? *evenColor : *oddColor);

    tableWidget->setItem(row, column, item);
    return item;
}

QTableWidget *StudentFormulaTab::createFormulaTableWidget()
{
    QTableWidget *tableWidget = new QTableWidget(this);

    QtMaterialScrollBar *verticalScrollbar = new QtMaterialScrollBar(this);
    QtMaterialScrollBar *horizontalScrollbar = new QtMaterialScrollBar(this);

    verticalScrollbar->setSliderColor(QColor(207, 226, 243));
    horizontalScrollbar->setSliderColor(QColor(207, 226, 243));

    horizontalScrollbar->setOrientation(Qt::Horizontal);
    horizontalScrollbar->setHideOnMouseOut(false);
    verticalScrollbar->setHideOnMouseOut(false);

    QFont font("Roboto", 10, QFont::Normal);

    tableWidget->setStyleSheet("QTableView::item:selected { background-color: rgb(207, 226, 243); }"
                               "QTableCornerButton::section{"
                               "border-top:0px solid #D8D8D8;"
                               "border-left:0px solid #D8D8D8;"
                               "border-right:1px solid #D8D8D8;"
                               "border-bottom: 1px solid #D8D8D8;"
                               "background-color:white;"
                               "}");
    tableWidget->setFont(font);

    tableWidget->setVerticalScrollBar(verticalScrollbar);
    tableWidget->setHorizontalScrollBar(horizontalScrollbar);

    tableWidget->setFrameShape(QFrame::NoFrame);

    QStringList headerList({"Time period", "Element of control", "Value", "TestValue"});
    tableWidget->setColumnCount(headerList.size());
    tableWidget->setHorizontalHeaderLabels(headerList);
    tableWidget->horizontalHeader()->setFont(font);

    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{"
                                                   "border-top:1px solid #D8D8D8;"
                                                   "border-left:0px solid #D8D8D8;"
                                                   "border-right:1px solid #D8D8D8;"
                                                   "border-bottom: 2px solid #D8D8D8;"
                                                   "background-color:white;"
                                                   "height: 32px;"
                                                   "padding-top:10px;"
                                                   "padding-bottom:10px;"
                                                   "}");

    tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    tableWidget->verticalHeader()->setFont(font);
    tableWidget->verticalHeader()->setSectionsClickable(false);
    tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{"
                                                 "border-top:0px solid #D8D8D8;"
                                                 "border-left:1px solid #D8D8D8;"
                                                 "border-right:2px solid #D8D8D8;"
                                                 "border-bottom: 1px solid #D8D8D8;"
                                                 "background-color:white;"
                                                 "padding-left:10px;"
                                                 "padding-right:10px;"
                                                 "}");

    tableWidget->verticalHeader()->setContentsMargins(20, 0, 20, 0);
    tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
    tableWidget->setWordWrap(false);

    tableWidget->hideColumn(8);
    tableWidget->sortByColumn(8, Qt::DescendingOrder);
    tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter
                                                         | (Qt::Alignment) Qt::TextWordWrap);

    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setSortingEnabled(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setShowGrid(true);

    connect(tableWidget, &QTableWidget::cellChanged, [=](int, int) { recalcGrades(m_gradeList); });

    return tableWidget;
}


void StudentFormulaTab::addHeaderField(QVBoxLayout *headerLayout,
                                     const QString &labelText,
                                     QWidget *fieldContent,
                                     int labelWidth)
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addSpacing(10);
    QLabel *pLabel = new QLabel();
    QFont font("Roboto", 11, QFont::Normal);

    pLabel->setText(labelText);
    pLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    pLabel->setFont(font);

    pLabel->setMinimumWidth(labelWidth);
    hLayout->addSpacing(10);
    hLayout->addWidget(pLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    hLayout->addSpacing(10);
    hLayout->addWidget(fieldContent, 0, Qt::AlignLeft | Qt::AlignVCenter);
    hLayout->addSpacing(10);

    headerLayout->addSpacing(5);
    headerLayout->addLayout(hLayout);
    headerLayout->addSpacing(5);
}
