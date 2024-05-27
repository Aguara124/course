#include "gradetab.h"
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
#include "../ProfileEditDialog.h"
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

GradeTab::GradeTab(const QString &teacherLogin, QWidget *parent)
    : QWidget{parent}
    , m_pTableWidget(new QTableWidget(this))
    , m_pReloadButton(new QtMaterialRaisedButton(this))
    , m_pSearchField(new QtMaterialTextField(this))
    , m_pAssistantArea(new QScrollArea(this))
    , m_pEditProfile(nullptr)
    , m_teacherLogin(teacherLogin)
    , oddColor(new QBrush(QColor(240, 240, 240)))
    , evenColor(new QBrush(Qt::GlobalColor::white))
    , gradeRepository(RepositoryManager::instance().gradeRepository())

{}

template<typename T>
static void truncate(QList<T> &list, int length)
{
    if (0 <= length && length < list.length()) {
        list.erase(list.begin() + length, list.end());
    }
}

template<typename T>
QList<T> reversed(const QList<T> &in)
{
    QList<T> result;
    result.reserve(in.size());
    std::reverse_copy(in.begin(), in.end(), std::back_inserter(result));
    return result;
}

std::shared_ptr<Grade> getGradeById(int id, const QList<std::shared_ptr<Grade>> &gradeList)
{
    for (std::shared_ptr<Grade> grade : gradeList) {
        if (grade->getId() == id) {
            return grade;
        }
    }

    return nullptr;
}

QLayout *GradeTab::setupTableLayout()
{
    QVBoxLayout *tableLayout = new QVBoxLayout(this);
    QHBoxLayout *searchLayout = new QHBoxLayout(this);

    m_pSearchField->setLabel("Enter search text");
    m_pSearchField->setShowInputLine(true);
    m_pSearchField->setClearButtonEnabled(false);
    m_pSearchField->setInkColor(QColor(160, 198, 232));

    searchLayout->addWidget(m_pSearchField);

    connect(m_pSearchField,
            SIGNAL(textChanged(const QString &)),
            this,
            SLOT(searchChanged(const QString &)));

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

    connect(m_pReloadButton, SIGNAL(clicked()), this, SLOT(startReceiveData()));

    setupGradeTableWidget();

    tableLayout->addLayout(searchLayout);
    tableLayout->addWidget(m_pTableWidget, 100);

    tableLayout->setContentsMargins(0, 0, 10, 0);

    return tableLayout;
}

void GradeTab::keyPressEvent(QKeyEvent *e)
{
    qDebug() << "mainWindow::key" << e->key();
    qDebug() << "mainWindow::scanCode" << e->nativeScanCode();
    qDebug() << "mainWindow::vKey" << e->nativeVirtualKey();

    if (e->key() == Qt::Key_Delete) {
        int rowCount = m_pTableWidget->rowCount();
        QList<QTableWidgetItem *> selected = m_pTableWidget->selectedItems();

        if (!selected.isEmpty()) {
            int id = m_pTableWidget->item(selected.first()->row(), 10)->text().toInt();
            m_pTableWidget->removeRow(selected.first()->row());
            m_pTableWidget->setRowCount(rowCount - 1);

            std::shared_ptr<Grade> selectedGrade = getGradeById(id, gradeList);
            gradeList.removeIf([id](const std::shared_ptr<Grade> &e) { return e->getId() == id; });

            if (gradeRepository->removeGrade(*selectedGrade)) {
                MainAppWindow::instance().postMessage("Grade removed");

            } else {
                MainAppWindow::instance().postError("Failed remove grade");
            }
        }
    }

    QWidget::keyPressEvent(e);
}

void GradeTab::setupTabLayout()
{
    m_pGradeLayout = new QHBoxLayout(this);

    QWidget *scrollAreaHolder = new QWidget(this);
    QVBoxLayout *scrollAreaLayout = new QVBoxLayout(scrollAreaHolder);
    scrollAreaLayout->setAlignment(Qt::AlignTop);

    m_pAssistantArea->setWidget(scrollAreaHolder);
    m_pAssistantArea->setWidgetResizable(true);

    QtMaterialScrollBar *verticalScrollbar = new QtMaterialScrollBar(this);
    QtMaterialScrollBar *horizontalScrollbar = new QtMaterialScrollBar(this);

    horizontalScrollbar->setOrientation(Qt::Horizontal);

    horizontalScrollbar->setHideOnMouseOut(false);
    verticalScrollbar->setHideOnMouseOut(false);

    horizontalScrollbar->setSliderColor(QColor(204, 204, 204));
    verticalScrollbar->setSliderColor(QColor(204, 204, 204));

    m_pAssistantLayout = new QVBoxLayout(this);

    m_pAssistantArea->setVerticalScrollBar(verticalScrollbar);
    m_pAssistantArea->setHorizontalScrollBar(horizontalScrollbar);
    m_pAssistantArea->setFrameShape(QFrame::NoFrame);
    m_pAssistantArea->setMinimumWidth(300);
    m_pAssistantArea->setMinimumHeight(500);

    m_pAssistantLayout->addWidget(m_pAssistantArea, 10, Qt::AlignTop);

    m_pGradeLayout->addLayout(m_pAssistantLayout);
    m_pGradeLayout->addLayout(setupTableLayout(), 100);

    setLayout(m_pGradeLayout);
    setContentsMargins(0, 0, 0, 0);

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(policy);

    startReceiveData();
}

void GradeTab::getFcs(const QList<std::shared_ptr<Fcs>> &fcsList)
{
    QStringList programs;
    QStringList programNames;

    for (std::shared_ptr<Fcs> fcs : fcsList) {
        if (!programNames.contains(fcs->getProgramName())) {
            programNames.append(fcs->getProgramName());
            programs.append(fcs->getProgram());
        }
    }

    Teacher teacherModel = *teacherList.first();

    bool moreThanThreeProgram = programNames.size() > 3;
    truncate(programNames, 3);

    QImage teacherImage = QImage();
    teacherImage.loadFromData(teacherModel.getAvatar());

    m_pTeacherCard = new QtMaterialCard(
        72,
        teacherImage,
        QString("%1 %2").arg(teacherModel.getName()).arg(teacherModel.getSurname()),
        QString("Teacher, %1%2")
            .arg(programNames.size() == 2 ? programNames.join(", ") : programs.join(", "))
            .arg(moreThanThreeProgram ? " and others..." : ""),
        true,
        false,
        QColor(),
        QList<QWidget *>(),
        this);

    m_pTeacherCard->setDescriptionColor(QColor(107, 126, 143));
    m_pTeacherCard->setMinimumHeight(75);
    m_pTeacherCard->setMinimumWidth(300);
    m_pTeacherCard->setParent(m_pAssistantArea->widget());

    connect(m_pTeacherCard, &QtMaterialCard::toolClicked, [=] {
        if (m_pEditProfile == nullptr) {
            m_pEditProfile = new ProfileEditDialog(this);
            m_pEditProfile->setupForm(QImage(teacherImage));

            connect(m_pEditProfile, &ProfileEditDialog::doApply, [=]() {
                qDebug() << "doApply";

                m_pTeacherCard->setImage(m_pEditProfile->getImage());

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

    m_pAssistantLayout->insertWidget(0, m_pTeacherCard, 1, Qt::AlignTop);
    m_pAssistantLayout->insertWidget(1, new QtMaterialDivider(this), 0, Qt::AlignTop);
    m_pAssistantLayout->insertStretch(2);
}

void GradeTab::getTeacher(const QList<std::shared_ptr<Teacher>> &teacherList)
{
    this->teacherList = teacherList;
    qDebug() << "getteacher start " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs);

    FcsGetLecturer *getLecturer = new FcsGetLecturer(teacherList, this);
    connect(getLecturer,
            SIGNAL(result(const QList<std::shared_ptr<Fcs>> &)),
            this,
            SLOT(getFcs(const QList<std::shared_ptr<Fcs>> &)));
    getLecturer->start();
}

void GradeTab::getAssistants(const QList<std::shared_ptr<Assistant>> &assistantsList)
{
    qDebug() << "getassist start " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs);

    this->assistantsList = assistantsList;
    if (!assistantsList.isEmpty()) {
        TeacherGetGrades *teacherGetGrades = new TeacherGetGrades(m_teacherLogin, assistantsList);
        connect(teacherGetGrades,
                SIGNAL(result(const QList<std::shared_ptr<Grade>> &)),
                this,
                SLOT(getGrades(const QList<std::shared_ptr<Grade>> &)));
        teacherGetGrades->start();
    }

    m_pButtonGroup = new QButtonGroup(this);
    m_pButtonGroup->setExclusive(false);

    connect(m_pButtonGroup,
            SIGNAL(buttonClicked(QAbstractButton *)),
            this,
            SLOT(onAssistantClicked(QAbstractButton *)));

    QStringList assistSet;
    QMap<QAbstractButton *, std::shared_ptr<Assistant>> prevMap(m_buttonToMap);

    for (std::shared_ptr<Assistant> assistant : assistantsList) {
        if (assistSet.contains(assistant->getLogin())) {
            continue;
        }

        bool assistantFound = false;
        QMapIterator<QAbstractButton *, std::shared_ptr<Assistant>> btnIterator(m_buttonToMap);

        while (btnIterator.hasNext()) {
            btnIterator.next();
            if (btnIterator.value()->getLogin() == assistant->getLogin()) {
                prevMap.remove(btnIterator.key());
                assistantFound = true;
                break;
            }
        }

        if (!assistantFound) {
            assistSet.append(assistant->getLogin());

            QtMaterialRaisedButton *pAssistant = new QtMaterialRaisedButton(this);

            QImage assistantImage;
            assistantImage.loadFromData(assistant->getAvatar());

            QIcon icon(QPixmap::fromImage(assistantImage, Qt::ColorOnly));

            pAssistant->setText(
                QString("%1 %2").arg(assistant->getName()).arg(assistant->getSurname()));
            pAssistant->setIcon(icon);
            pAssistant->setCheckable(true);
            //new
            pAssistant->setBackgroundColor(QColor(204, 204, 204));
            pAssistant->setForegroundColor(QColor(89, 89, 89));
            //new

            m_pButtonGroup->addButton(pAssistant);
            m_buttonToMap[pAssistant] = assistant;

            ((QVBoxLayout *) m_pAssistantArea->widget()->layout())->addWidget(pAssistant);
            ((QVBoxLayout *) m_pAssistantArea->widget()->layout())->addSpacing(20);
        }
    }

    ((QVBoxLayout *) m_pAssistantArea->widget()->layout())->addStretch(10);

    QMapIterator<QAbstractButton *, std::shared_ptr<Assistant>> btnIterator(prevMap);
    while (btnIterator.hasNext()) {
        btnIterator.next();
        m_pButtonGroup->removeButton(btnIterator.key());
    }
}

bool GradeTab::matchBySearchField(const QString &searchText, const Grade &grade)
{
    return grade.getNameOfStudent().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getName().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getSurName().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getElementOfControl().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getSubject().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getExaminerLogin().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getComment().contains(searchText, Qt::CaseSensitivity::CaseInsensitive);
}

void GradeTab::getGrades(const QList<std::shared_ptr<Grade>> &gradeList)
{
    qDebug() << "begin getgrades " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs);

    QList<std::shared_ptr<Grade>> filteredList;

    QStringList allowedExaminers;
    allowedExaminers.append(teacherList.first()->getLogin());

    QMapIterator<QAbstractButton *, std::shared_ptr<Assistant>> btnIterator(m_buttonToMap);
    while (btnIterator.hasNext()) {
        btnIterator.next();
        if (btnIterator.key()->isChecked()) {
            allowedExaminers.append(btnIterator.value()->getLogin());
        }
    }

    for (std::shared_ptr<Grade> grade : gradeList) {
        if (allowedExaminers.contains(grade->getExaminerLogin())
            && (m_pSearchField->text().isEmpty()
                || matchBySearchField(m_pSearchField->text(), *grade))) {
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
            //removed grade
            removeList.append(i);
            continue;
        }

        std::shared_ptr<Grade> oldGrade = getGradeById(id, this->gradeList);
        if (oldGrade != nullptr && !oldGrade->equals(*newGrade)) {
            //changed grade
            setGradeTableRecord(i, *newGrade);
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
            //added grade
            int currentRowCount = m_pTableWidget->rowCount();
            m_pTableWidget->insertRow(currentRowCount);
            setGradeTableRecord(currentRowCount, *grade);
        }
    }

    this->gradeList = gradeList;
    qDebug() << "end getgrades " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs);

    m_pTableWidget->blockSignals(false);
}

void GradeTab::searchChanged(const QString &searchText)
{
    getGrades(this->gradeList);
}

QTableWidgetItem *GradeTab::addGradeItem(int row, int column, const QString &text, bool isEditable)
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

void GradeTab::setGradeTableRecord(int index, const Grade &grade)
{
    addGradeItem(index, 0, grade.getNameOfStudent());
    addGradeItem(index, 1, grade.getName() + " " + grade.getSurName());
    addGradeItem(index, 2, grade.getExaminerLogin(), true);
    addGradeItem(index, 3, grade.getAssignmentName());
    addGradeItem(index, 4, grade.getSubject());
    addGradeItem(index, 5, grade.getElementOfControl());
    addGradeItem(index, 6, QString::number(grade.getMark()), true);
    addGradeItem(index, 7, grade.getGradeTime().toString(), true);
    addGradeItem(index, 8, grade.getDate().toString(), true);
    addGradeItem(index, 9, grade.getComment(), true);
    addGradeItem(index, 10, QString::number(grade.getId()));
}

void GradeTab::setupGradeTableWidget()
{
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

    QStringList headerList({"Login of a student",
                            "Name of a student",
                            "Login of a TA",
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

    m_pTableWidget->hideColumn(10);
    m_pTableWidget->sortByColumn(10, Qt::DescendingOrder);
    m_pTableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter
                                                            | (Qt::Alignment) Qt::TextWordWrap);

    m_pTableWidget->horizontalHeader()->setStretchLastSection(true);
    m_pTableWidget->setSortingEnabled(true);

    m_pTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_pTableWidget->setShowGrid(true);

    connect(m_pTableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(onGradeCellChanged(int, int)));
}

void GradeTab::startReceiveData()
{
    qDebug() << "start receivedata " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs);

    assistantsList.clear();
    if (teacherList.isEmpty()) {
        GetTeacher *teacherGet = new GetTeacher(m_teacherLogin);
        connect(teacherGet,
                SIGNAL(result(const QList<std::shared_ptr<Teacher>> &)),
                this,
                SLOT(getTeacher(const QList<std::shared_ptr<Teacher>> &)));
        teacherGet->start();
    }

    GetAssistants *teacherGetAssistants = new GetAssistants(m_teacherLogin);
    connect(teacherGetAssistants,
            SIGNAL(result(const QList<std::shared_ptr<Assistant>> &)),
            this,
            SLOT(getAssistants(const QList<std::shared_ptr<Assistant>> &)));
    teacherGetAssistants->start();
}

void GradeTab::onAssistantClicked(QAbstractButton *pButton)
{
    std::shared_ptr<Assistant> assistant = m_buttonToMap[pButton];
    if (assistant != nullptr) {
        qDebug() << "clicked on " << assistant->getLogin() << "checked is " << pButton->isChecked();
        getGrades(this->gradeList);
    }
}

const std::shared_ptr<Grade> GradeTab::getRecordFromTable(int index)
{
    int gradeId = (m_pTableWidget->item(index, m_pTableWidget->columnCount() - 1)->text()).toInt();
    for (std::shared_ptr<Grade> g : gradeList) {
        if (g->getId() == gradeId) {
            g->setExaminerLogin(m_pTableWidget->item(index, 2)->text());
            g->setMark((m_pTableWidget->item(index, 6)->text()).toDouble());
            g->setGradeTime(QTime::fromString(m_pTableWidget->item(index, 7)->text()));
            g->setDate(QDate::fromString(m_pTableWidget->item(index, 8)->text()));
            g->setComment(m_pTableWidget->item(index, 9)->text());

            return g;
        }
    }

    return nullptr;
}

void GradeTab::onGradeCellChanged(int row, int column)
{
    const std::shared_ptr<Grade> grade = getRecordFromTable(row);
    if (!gradeRepository->updateGrade(*grade)) {
        MainAppWindow::instance().postMessage("Error updating record. Data will be reloaded.",
                                              QtMaterialSnackbar::MessageQualifier::Error,
                                              true);
        startReceiveData();
    }
}
