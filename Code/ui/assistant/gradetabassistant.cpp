#include "gradetabassistant.h"
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
#include "workers/assistantget.h"
// #include "workers/teachergetassistant.h"
// #include "workers/teachergetgrades.h"
#include "../profileeditdialog.h"
#include "student.h"
#include "workers/GetStudents.h"
#include "workers/assistantgetgrades.h"
#include "workers/getstudent.h"
#include <assistant.h>
#include <grade.h>
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

GradeTabAssistant::GradeTabAssistant(const QString &assistantLogin, QWidget *parent)
    : QWidget{parent}
    , m_pTableWidget(new QTableWidget(this))
    , m_pReloadButton(new QtMaterialRaisedButton(this))
    , m_pSearchField(new QtMaterialTextField(this))
    , m_pStudentArea(new QScrollArea(this))
    , m_assistantLogin(assistantLogin)
    , oddColor(new QBrush(QColor(240, 240, 240)))
    , evenColor(new QBrush(Qt::GlobalColor::white))
    , gradeRepository(RepositoryManager::instance().gradeRepository())
    , m_pEditProfile(nullptr)

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
    result.reserve(in.size()); // reserve is new in Qt 4.7
    std::reverse_copy(in.begin(), in.end(), std::back_inserter(result));
    return result;
}


QLayout *GradeTabAssistant::setupTableLayout()
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

void GradeTabAssistant::keyPressEvent(QKeyEvent *e)
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

void GradeTabAssistant::setupTabLayout()
{
    GetAssistant *getAssist = new GetAssistant(m_assistantLogin);
    connect(getAssist,
            &GetAssistant::result,
            this,
            [=](const QList<std::shared_ptr<Assistant>> &result){
                assistant = result.first();
                startReceiveData();
            });
    getAssist->start();

    // assistant =
    m_pGradeLayout = new QHBoxLayout(this);

    QWidget *scrollAreaHolder = new QWidget(this);
    QVBoxLayout *scrollAreaLayout = new QVBoxLayout(scrollAreaHolder);
    scrollAreaLayout->setAlignment(Qt::AlignTop);

    m_pStudentArea->setWidget(scrollAreaHolder);
    m_pStudentArea->setWidgetResizable(true);

    QtMaterialScrollBar *verticalScrollbar = new QtMaterialScrollBar(this);
    QtMaterialScrollBar *horizontalScrollbar = new QtMaterialScrollBar(this);

    horizontalScrollbar->setOrientation(Qt::Horizontal);

    horizontalScrollbar->setHideOnMouseOut(false);
    verticalScrollbar->setHideOnMouseOut(false);

    //new
    horizontalScrollbar->setSliderColor(QColor(204, 204, 204));
    verticalScrollbar->setSliderColor(QColor(204, 204, 204));
    //new

    m_pStudentLayout = new QVBoxLayout(this);

    m_pStudentArea->setVerticalScrollBar(verticalScrollbar);
    m_pStudentArea->setHorizontalScrollBar(horizontalScrollbar);
    m_pStudentArea->setFrameShape(QFrame::NoFrame);
    m_pStudentArea->setMinimumWidth(300);
    m_pStudentArea->setMinimumHeight(500);

    m_pStudentLayout->addWidget(m_pStudentArea, 10, Qt::AlignTop);

    m_pGradeLayout->addLayout(m_pStudentLayout);
    m_pGradeLayout->addLayout(setupTableLayout(), 100);

    setLayout(m_pGradeLayout);
    setContentsMargins(0, 0, 0, 0);

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(policy);
}

void GradeTabAssistant::getStudents(const QList<std::shared_ptr<Student>> &studentList)
{
    qDebug() << "------------------------------";
    qDebug() << "getstudent start " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs);
    qDebug() << "------------------------------";

    this->studentsList = studentList;
    qDebug() << "--------------------";
    qDebug() << studentList.size();
    for (std::shared_ptr<Student> s: studentList)
    {
        Student st = *s;
        qDebug() << st.getLogin();
    }
    qDebug() << "--------------------";
    if (!studentsList.isEmpty()) {
        // STOPPED HERE

        AssistantGetGrades *assistantGetGrades = new AssistantGetGrades(m_assistantLogin);
        connect(assistantGetGrades,
                SIGNAL(result(const QList<std::shared_ptr<Grade>> &)),
                this,
                SLOT(getGrades(const QList<std::shared_ptr<Grade>> &)));
        assistantGetGrades->start();
    }

    m_pButtonGroup = new QButtonGroup(this);
    m_pButtonGroup->setExclusive(false);

    connect(m_pButtonGroup,
            SIGNAL(buttonClicked(QAbstractButton *)),
            this,
            SLOT(onStudentClicked(QAbstractButton *)));

    QStringList studentSet;
    QMap<QAbstractButton *, std::shared_ptr<Student>> prevMap(m_buttonToMap);

    for (std::shared_ptr<Student> student: studentsList) {
        if (studentSet.contains(student->getLogin())) {
            continue;
        }

        bool studentFound = false;
        QMapIterator<QAbstractButton *, std::shared_ptr<Student>> btnIterator(m_buttonToMap);

        while (btnIterator.hasNext()) {
            btnIterator.next();
            if (btnIterator.value()->getLogin() == student->getLogin()) {
                prevMap.remove(btnIterator.key());
                studentFound = true;
                break;
            }
        }

        //STOPPED 2
        if (!studentFound) {
            studentSet.append(student->getLogin());

            QtMaterialRaisedButton *pStudent = new QtMaterialRaisedButton(this);

            QImage studentImage;
            studentImage.loadFromData(student->getAvatar());

            QIcon icon(QPixmap::fromImage(studentImage, Qt::ColorOnly));

            pStudent->setText(
                QString("%1 %2").arg(student->getName()).arg(student->getSurname()));
            pStudent->setIcon(icon);
            pStudent->setCheckable(true);
            //new
            pStudent->setBackgroundColor(QColor(204, 204, 204));
            pStudent->setForegroundColor(QColor(89, 89, 89));
            //new

            m_pButtonGroup->addButton(pStudent);
            m_buttonToMap[pStudent] = student;

            ((QVBoxLayout *) m_pStudentArea->widget()->layout())->addWidget(pStudent);
            ((QVBoxLayout *) m_pStudentArea->widget()->layout())->addSpacing(20);
        }
    }

    ((QVBoxLayout *) m_pStudentArea->widget()->layout())->addStretch(10);

    QMapIterator<QAbstractButton *, std::shared_ptr<Student>> btnIterator(prevMap);
    while (btnIterator.hasNext()) {
        btnIterator.next();
        m_pButtonGroup->removeButton(btnIterator.key());
    }
}

bool GradeTabAssistant::matchBySearchField(const QString &searchText, const Grade &grade)
{
    return grade.getNameOfStudent().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getName().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getSurName().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getElementOfControl().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getSubject().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
           || grade.getExaminerLogin().contains(searchText, Qt::CaseSensitivity::CaseInsensitive)
            || grade.getComment().contains(searchText, Qt::CaseSensitivity::CaseInsensitive);
}

void GradeTabAssistant::getGrades(const QList<std::shared_ptr<Grade>> &gradeList)
{
    //bool isInitial = this->gradeList.isEmpty();
    qDebug() << "------------------------------";
    qDebug() << "begin getgrades " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs);
    qDebug() << "------------------------------";

    QList<std::shared_ptr<Grade>> filteredList;

    QStringList allowedStudents;

    QMapIterator<QAbstractButton *, std::shared_ptr<Student>> btnIterator(m_buttonToMap);
    while (btnIterator.hasNext()) {
        btnIterator.next();
        if (btnIterator.key()->isChecked()) {
            allowedStudents.append(btnIterator.value()->getLogin());
        }
    }

    for (std::shared_ptr<Grade> grade : gradeList) {
        if ( allowedStudents.contains(grade->getNameOfStudent())&&(m_pSearchField->text().isEmpty()
                || matchBySearchField(m_pSearchField->text(), *grade))) {

            filteredList.append(grade);
        }
    }

    QList<int> removeList;
    int rowCount = m_pTableWidget->rowCount();
    QList<int> existList(rowCount);

    m_pTableWidget->blockSignals(true);

    for (int i = 0; i < rowCount; i++) {
        QTableWidgetItem *pItem = m_pTableWidget->item(i, 10);
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

void GradeTabAssistant::searchChanged(const QString &searchText)
{
    getGrades(this->gradeList);
}

QTableWidgetItem *GradeTabAssistant::addGradeItem(int row, int column, const QString &text, bool isEditable)
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

void GradeTabAssistant::setGradeTableRecord(int index, const Grade &grade)
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

void GradeTabAssistant::setupGradeTableWidget()
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

//PROBLEM
void GradeTabAssistant::startReceiveData()
{
    qDebug() << "------------------------------";
    qDebug() << "start receivedata " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs);
    qDebug() << "------------------------------";
    Assistant assist = *assistant;

    if (!avatarSet)
        setCard();

    GetStudents *assistGetStudents = new GetStudents(assist.getGroup(), assist.getSubGroup(), m_assistantLogin);
    connect(assistGetStudents,
            SIGNAL(result(const QList<std::shared_ptr<Student>> &)),
            this,
            SLOT(getStudents(const QList<std::shared_ptr<Student>> &)));
    assistGetStudents->start();
}

void GradeTabAssistant::onStudentClicked(QAbstractButton *pButton)
{
    std::shared_ptr<Student> student = m_buttonToMap[pButton];
    if (student != nullptr) {
        qDebug() << "clicked on " << student->getLogin() << "checked is " << pButton->isChecked();
        getGrades(this->gradeList);
    }
}

const std::shared_ptr<Grade> GradeTabAssistant::getRecordFromTable(int index)
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

void GradeTabAssistant::onGradeCellChanged(int row, int column)
{
    const std::shared_ptr<Grade> grade = getRecordFromTable(row);
    if (!gradeRepository->updateGrade(*grade)) {
        MainAppWindow::instance().postMessage("Error updating record. Data will be reloaded.",
                                              QtMaterialSnackbar::MessageQualifier::Error,
                                              true);
        startReceiveData();
    }
}

void GradeTabAssistant::setCard()
{
    avatarSet = 1;
    Assistant assistantModel = *assistant;

    QImage assistImage = QImage();
    assistImage.loadFromData(assistantModel.getAvatar());

    m_pAssistantCard = new QtMaterialCard(
        72,
        assistImage,
        QString("%1 %2").arg(assistantModel.getName()).arg(assistantModel.getSurname()),
        QString("Assistant, %1-%2")
            .arg(assistantModel.getGroup())
            .arg(assistantModel.getSubGroup()),
        true,
        false,
        QColor(),
        QList<QWidget *>(),
        this);

    m_pAssistantCard->setDescriptionColor(QColor(107, 126, 143));
    m_pAssistantCard->setMinimumHeight(75);
    m_pAssistantCard->setMinimumWidth(300);
    m_pAssistantCard->setParent(m_pStudentArea->widget());

    connect(m_pAssistantCard, &QtMaterialCard::toolClicked, [=] {
        if (m_pEditProfile == nullptr) {
            m_pEditProfile = new ProfileEditDialog(this);
            m_pEditProfile->setupForm(QImage(assistImage));

            connect(m_pEditProfile, &ProfileEditDialog::doApply, [=]() {
                qDebug() << "doApply";

                m_pAssistantCard->setImage(m_pEditProfile->getImage());

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

    m_pStudentLayout->insertWidget(0, m_pAssistantCard, 1, Qt::AlignTop);
    m_pStudentLayout->insertWidget(1, new QtMaterialDivider(this), 0, Qt::AlignTop);
    m_pStudentLayout->insertStretch(2);
}

std::shared_ptr<Grade> GradeTabAssistant::getGradeById(
    int id, const QList<std::shared_ptr<Grade>> &gradeList)
{
    for (std::shared_ptr<Grade> grade : gradeList) {
        if (grade->getId() == id) {
            return grade;
        }
    }

    return nullptr;
}
