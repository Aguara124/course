#include "studentgradetab.h"
#include <QButtonGroup>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
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
#include <qlabel.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qtmaterialautocomplete.h>
#include <qtmaterialcard.h>
#include <qtmaterialcheckbox.h>
#include <qtmaterialdivider.h>
#include <qtmaterialflatbutton.h>
#include <qtmaterialiconbutton.h>
#include <qtmaterialraisedbutton.h>
#include <qtmaterialscrollbar.h>
#include <qtmaterialtabs.h>
#include <qtmaterialtextfield.h>
#include <qtmaterialtoggle.h>
#include <teacher.h>

#include "../ProfileEditDialog.h"
#include "workers/fcsgetprogram.h"
#include "workers/getgradesforstudent.h"
#include "workers/getstudent.h"
#include "workers/getsubjects.h"

StudentGradeTab::StudentGradeTab(const QString &studentLogin, QWidget *parent)
    : QWidget{parent}
    , m_pTableWidget(new QTableWidget(this))
    , m_pReloadButton(new QtMaterialRaisedButton(this))
    , m_pSearchField(new QtMaterialTextField(this))
    , m_pSubjectsArea(new QScrollArea(this))
    , m_studentLogin(studentLogin)
    , oddColor(new QBrush(QColor(240, 240, 240)))
    , evenColor(new QBrush(Qt::GlobalColor::white))
    , gradeRepository(RepositoryManager::instance().gradeRepository())
    , m_pEditProfile(nullptr)
{}

void StudentGradeTab::setupTabLayout()
{
    m_pGradeLayout = new QHBoxLayout(this);

    QWidget *scrollAreaHolder = new QWidget(this);
    QVBoxLayout *scrollAreaLayout = new QVBoxLayout(scrollAreaHolder);
    scrollAreaLayout->setAlignment(Qt::AlignTop);

    m_pSubjectsArea->setWidget(scrollAreaHolder);
    m_pSubjectsArea->setWidgetResizable(true);

    QtMaterialScrollBar *verticalScrollbar = new QtMaterialScrollBar(this);
    QtMaterialScrollBar *horizontalScrollbar = new QtMaterialScrollBar(this);

    horizontalScrollbar->setOrientation(Qt::Horizontal);

    horizontalScrollbar->setHideOnMouseOut(false);
    verticalScrollbar->setHideOnMouseOut(false);

    horizontalScrollbar->setSliderColor(QColor(207, 226, 243));
    verticalScrollbar->setSliderColor(QColor(207, 226, 243));

    m_pSubjectsLayout = new QVBoxLayout(this);

    m_pSubjectsArea->setVerticalScrollBar(verticalScrollbar);
    m_pSubjectsArea->setHorizontalScrollBar(horizontalScrollbar);
    m_pSubjectsArea->setFrameShape(QFrame::NoFrame);
    m_pSubjectsArea->setMinimumWidth(300);
    m_pSubjectsArea->setMinimumHeight(500);

    m_pModuleSelector = new QtMaterialSelectBox(this);
    m_pModuleSelector->setInkColor(QColor(160, 198, 232));
    m_pModuleSelector->setDataSource(QStringList{"All periods",
                                                 "1st Semester",
                                                 "1st Module",
                                                 "2nd Module",
                                                 "2nd Semester",
                                                 "3rd Module",
                                                 "4th Module"});
    addHeaderField(m_pSubjectsLayout, "Periods:", m_pModuleSelector, 100);
    m_pModuleSelector->setText("All periods");
    connect(m_pModuleSelector, &QLineEdit::textChanged, [=](const QString &) {
        redrawGrades(m_gradeList);
    });

    m_pControlSelector = new QtMaterialSelectBox(this);
    m_pControlSelector->setInkColor(QColor(160, 198, 232));
    m_pControlSelector->setDataSource(QStringList{
        "Any", "HW", "Quiz", "Test", "Laboratory", "Classwork", "Bonus Task", "Pesentation"});
    addHeaderField(m_pSubjectsLayout, "Control type:", m_pControlSelector, 100);
    m_pControlSelector->setText("Any");
    connect(m_pControlSelector, &QLineEdit::textChanged, [=](const QString &) {
        redrawGrades(m_gradeList);
    });

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
        m_student = student;

        FcsGetProgram *getProgram = new FcsGetProgram(QString());
        connect(getProgram, &FcsGetProgram::result, this, &StudentGradeTab::updateStudentCard);
        getProgram->start();

        GetSubjects *getSubjects = new GetSubjects(student->getProgram(),
                                                   student->getPilot(),
                                                   student->getCourse());
        connect(getSubjects, &GetSubjects::result, this, &StudentGradeTab::updateSubjects);
        getSubjects->start();
    });
    getStudent->start();

    GetGradesForStudent *getGrades = new GetGradesForStudent(m_studentLogin);
    connect(getGrades, &GetGradesForStudent::result, this, &StudentGradeTab::redrawGrades);
    getGrades->start();
}

QLayout *StudentGradeTab::setupGradeTableLayout()
{
    QVBoxLayout *tableLayout = new QVBoxLayout(this);
    QHBoxLayout *searchLayout = new QHBoxLayout(this);

    m_pSearchField->setLabel("Enter search text");
    m_pSearchField->setShowInputLine(true);
    m_pSearchField->setClearButtonEnabled(false);
    m_pSearchField->setInkColor(QColor(160, 198, 232));

    searchLayout->addWidget(m_pSearchField);

    connect(m_pSearchField, &QLineEdit::textChanged, [=](const QString &) {
        redrawGrades(this->m_gradeList);
    });

    m_pReloadButton->setIcon(QIcon(":/icons/icons/action/svg/production/ic_autorenew_24px.svg"));
    m_pReloadButton->setText("Reload");
    m_pReloadButton->setMinimumWidth(150);
    m_pReloadButton->setMaximumWidth(150);
    m_pReloadButton->setMaximumHeight(20);
    m_pReloadButton->setRole(Material::Role::Primary);
    m_pReloadButton->setBackgroundColor(QColor(204, 204, 204));
    m_pReloadButton->setForegroundColor(QColor(89, 89, 89));

    searchLayout->addWidget(m_pReloadButton);
    searchLayout->setContentsMargins(55, 20, 20, 20);

    connect(m_pReloadButton, &QAbstractButton::clicked, [=] {
        GetGradesForStudent *getGrades = new GetGradesForStudent(m_studentLogin);
        connect(getGrades, &GetGradesForStudent::result, this, &StudentGradeTab::redrawGrades);
        getGrades->start();
    });

    setupStudentGradeTableWidget();

    tableLayout->addLayout(searchLayout);
    tableLayout->addWidget(m_pTableWidget, 100);

    tableLayout->setContentsMargins(0, 0, 10, 0);

    return tableLayout;
}

void StudentGradeTab::updateStudentCard(const QList<std::shared_ptr<Fcs>> &fcsList)
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

void StudentGradeTab::updateSubjects(const QList<std::shared_ptr<Fcs>> &fcsList)
{
    qDebug() << "getassist start " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs);

    this->m_fcsList = fcsList;

    m_pButtonGroup = new QButtonGroup(this);
    m_pButtonGroup->setExclusive(false);

    connect(m_pButtonGroup, &QButtonGroup::buttonClicked, [=](QAbstractButton *pButton) {
        std::shared_ptr<Fcs> fcs = m_buttonToMap[pButton];
        if (fcs != nullptr) {
            qDebug() << "clicked on " << fcs->getSubject() << "checked is " << pButton->isChecked();
            redrawGrades(m_gradeList);
        }
    });

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
            subjectButton->setBackgroundColor(QColor(204, 204, 204));
            subjectButton->setForegroundColor(QColor(89, 89, 89));
            subjectButton->setText(fcs->getSubject());
            subjectButton->setCheckable(true);

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

    QTimer::singleShot(100, [=] {
        m_pButtonGroup->blockSignals(true);
        for (auto [k, v] : m_buttonToMap.asKeyValueRange()) {
            k->setChecked(true);
        }
        m_pButtonGroup->blockSignals(false);
        redrawGrades(m_gradeList);
    });
}

bool StudentGradeTab::matchBySearchField(const QString &searchText, const Grade &grade)
{
    return grade.getNameOfStudent().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getName().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getSurName().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getElementOfControl().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getSubject().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getExaminerLogin().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getComment().contains(searchText, Qt::CaseSensitivity::CaseInsensitive);
}

void StudentGradeTab::redrawGrades(const QList<std::shared_ptr<Grade>> &gradeList)
{
    QList<std::shared_ptr<Grade>> filteredList;

    QStringList allowedModules;
    QStringList allowedSubjects;
    QStringList allowedControl;

    QMapIterator<QAbstractButton *, std::shared_ptr<Fcs>> btnIterator(m_buttonToMap);
    while (btnIterator.hasNext()) {
        btnIterator.next();
        if (btnIterator.key()->isChecked()) {
            allowedSubjects.append(btnIterator.value()->getSubject());
        }
    }

    if (m_pControlSelector->text() != "Any") {
        allowedControl.append(m_pControlSelector->text());
    }

    if (m_pModuleSelector->text() != "All periods") {
        if (m_pModuleSelector->text() == "1st Semester") {
            allowedModules.append("1st Module");
            allowedModules.append("2nd Module");
        } else if (m_pModuleSelector->text() == "2nd Semester") {
            allowedModules.append("3rd Module");
            allowedModules.append("4th Module");
        } else {
            allowedModules.append(m_pModuleSelector->text());
        }
    }

    for (std::shared_ptr<Grade> grade : gradeList) {
        if (allowedSubjects.contains(grade->getSubject())
            && (m_pSearchField->text().isEmpty()
                || matchBySearchField(m_pSearchField->text(), *grade))
            && (allowedModules.isEmpty() || allowedModules.contains(grade->getModule()))
            && (allowedControl.isEmpty() || allowedControl.contains(grade->getElementOfControl()))) {
            filteredList.append(grade);
        }
    }

    QList<int> removeList;
    int rowCount = m_pTableWidget->rowCount();
    QList<int> existList(rowCount);

    m_pTableWidget->blockSignals(true);

    for (int i = 0; i < rowCount; i++) {
        QTableWidgetItem *pItem = m_pTableWidget->item(i, m_pTableWidget->columnCount() - 1);
        if (pItem == nullptr) {
            break;
        }

        int id = pItem->text().toInt();
        existList.append(id);

        std::shared_ptr<Grade> newGrade = getGradeById(id, filteredList);
        if (newGrade == nullptr) {
            removeList.append(i);
            continue;
        }

        std::shared_ptr<Grade> oldGrade = getGradeById(id, this->m_gradeList);
        if (oldGrade != nullptr && !oldGrade->equals(*newGrade)) {
            setStudentGradeTableRecord(i, *newGrade);
        }
    }

    if (!removeList.isEmpty()) {
        std::sort(removeList.begin(), removeList.end());
        for (int i = removeList.size() - 1; i >= 0; i--) {
            m_pTableWidget->removeRow(removeList.at(i));
        }
    }

    for (std::shared_ptr<Grade> grade : filteredList) {
        if (!existList.contains(grade->getId())) {
            int currentRowCount = m_pTableWidget->rowCount();
            m_pTableWidget->insertRow(currentRowCount);
            setStudentGradeTableRecord(currentRowCount, *grade);
        }
    }

    this->m_gradeList = gradeList;
    qDebug() << "end getgrades " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs);

    m_pTableWidget->blockSignals(false);
}

QTableWidgetItem *StudentGradeTab::addGradeItem(int row, int column, const QString &text, bool isEditable)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    if (!isEditable) {
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }

    item->setTextAlignment(Qt::AlignCenter);
    item->setBackground((row & 1) ? *evenColor : *oddColor);

    m_pTableWidget->setItem(row, column, item);
    return item;
}

void StudentGradeTab::setStudentGradeTableRecord(int index, const Grade &grade)
{
    addGradeItem(index, 0, grade.getExaminerLogin());
    addGradeItem(index, 1, grade.getAssignmentName());
    addGradeItem(index, 2, grade.getSubject());
    addGradeItem(index, 3, grade.getElementOfControl());
    addGradeItem(index, 4, QString::number(grade.getMark()));
    addGradeItem(index, 5, grade.getGradeTime().toString());
    addGradeItem(index, 6, grade.getDate().toString());
    addGradeItem(index, 7, grade.getComment());
    addGradeItem(index, 8, QString::number(grade.getId()));
}

void StudentGradeTab::setupStudentGradeTableWidget()
{
    QtMaterialScrollBar *verticalScrollbar = new QtMaterialScrollBar(this);
    QtMaterialScrollBar *horizontalScrollbar = new QtMaterialScrollBar(this);

    horizontalScrollbar->setOrientation(Qt::Horizontal);
    horizontalScrollbar->setHideOnMouseOut(false);

    verticalScrollbar->setHideOnMouseOut(false);

    horizontalScrollbar->setSliderColor(QColor(207, 226, 243));
    verticalScrollbar->setSliderColor(QColor(207, 226, 243));

    QFont font("Roboto", 10, QFont::Normal);

    m_pTableWidget->setStyleSheet(
        "QTableView::item:selected { background-color: rgb(207, 226, 243); }"
        "QTableCornerButton::section{"
        "border-top:0px solid #D8D8D8;"
        "border-left:0px solid #D8D8D8;"
        "border-right:1px solid #D8D8D8;"
        "border-bottom: 1px solid #D8D8D8;"
        "background-color:white;"
        "}");
    m_pTableWidget->setFont(font);

    m_pTableWidget->setVerticalScrollBar(verticalScrollbar);
    m_pTableWidget->setHorizontalScrollBar(horizontalScrollbar);

    m_pTableWidget->setFrameShape(QFrame::NoFrame);

    QStringList headerList({"Grade master",
                            "Assignment's name",
                            "Subject",
                            "Element of control",
                            "Mark",
                            "Time of adding mark",
                            "Date",
                            "Comment",
                            "Id"});
    m_pTableWidget->setColumnCount(headerList.size());
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

    m_pTableWidget->hideColumn(8);
    m_pTableWidget->sortByColumn(8, Qt::DescendingOrder);
    m_pTableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter
                                                            | (Qt::Alignment) Qt::TextWordWrap);

    m_pTableWidget->horizontalHeader()->setStretchLastSection(true);
    m_pTableWidget->setSortingEnabled(true);

    m_pTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_pTableWidget->setShowGrid(true);
}

std::shared_ptr<Grade> StudentGradeTab::getGradeById(int id,
                                                     const QList<std::shared_ptr<Grade>> &gradeList)
{
    for (std::shared_ptr<Grade> grade : gradeList) {
        if (grade->getId() == id) {
            return grade;
        }
    }

    return nullptr;
}

void StudentGradeTab::addHeaderField(QVBoxLayout *headerLayout,
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
