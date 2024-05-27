#include "submittedworkdialog.h"
#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QImage>
#include <QJSEngine>
#include <QJSValue>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include "../mainappwindow.h"
#include "repositorymanager.h"
#include "workers/fcsgetlecturer.h"
#include "workers/teacherget.h"
#include <poppler/qt6/poppler-qt6.h>
#include <qbuttongroup.h>
#include <qtmaterialcard.h>
#include <qtmaterialcheckbox.h>
#include <qtmaterialdivider.h>
#include <qtmaterialiconbutton.h>
#include <qtmaterialradiobutton.h>
#include <qtmaterialraisedbutton.h>
#include <qtmaterialscrollbar.h>
#include <qtmaterialselectbox.h>
#include <qtmaterialtextfield.h>

#include "qalgorithms.h"
#include "workers/fcsgetprogram.h"
#include "workers/getconsultation.h"
#include "workers/getstudent.h"

#include "authdata.h"
#include "qtmaterialbadge.h"
#include "workers/getworks.h"

#include "../hint.h"

static void removeWidgets(QLayout *pLayout)
{
    QLayoutItem *item;
    while ((item = pLayout->takeAt(0)) != 0) {
        if (item->widget()) {
            pLayout->removeWidget(item->widget());
            delete item->widget();
        }
        delete item;
    }
}

SubmittedWorkDialog::SubmittedWorkDialog(const int assignmentId, QWidget *parent)
    : QDialog(parent)
    , m_nAssignmentId(assignmentId)
    , oddColor(new QBrush(QColor(240, 240, 240)))
    , evenColor(new QBrush(Qt::GlobalColor::white))
    , assignmentRepository(RepositoryManager::instance().assignmentRepository())
    , workRepository(RepositoryManager::instance().workRepository())
    , gradeRepository(RepositoryManager::instance().gradeRepository())
    , m_pCurrentRequest(nullptr)
    , m_pGradeTable(nullptr)
{
}

void SubmittedWorkDialog::setupLayout()
{
    setStyleSheet("QWidget { background: white; }");

    m_pTabLayout = new QHBoxLayout(this);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);

    vBoxLayout->addWidget(setupFilterBox());
    vBoxLayout->addWidget(setupWorkListBox());

    m_pTabLayout->addStretch();
    m_pTabLayout->addLayout(vBoxLayout);
    m_pTabLayout->addSpacing(30);
    m_pTabLayout->addWidget(setupWorkBox());
    m_pTabLayout->addStretch();

    setLayout(m_pTabLayout);
    setContentsMargins(0, 0, 0, 0);

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(policy);

    GetWorks *getWorks = new GetWorks(m_nAssignmentId);
    connect(getWorks, &GetWorks::result, this, &SubmittedWorkDialog::buildWorkList);
    getWorks->start();
}

QWidget *SubmittedWorkDialog::setupFilterBox()
{
    QFont font("Roboto", 12, QFont::Normal);
    QFont fontSmall("Roboto", 10, QFont::Normal);

    QGroupBox *groupStageBox = new QGroupBox(tr("Filter"), this);
    groupStageBox->setFont(font);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addSpacing(30);

    m_pSearchText = new QtMaterialTextField(this);
    m_pSearchText->setLabel("Search...");
    m_pSearchText->setPlaceholderText("Enter text for filter");
    m_pSearchText->setMinimumHeight(50);
    m_pSearchText->setMinimumWidth(150);
    m_pSearchText->setMaximumWidth(150);

    hLayout->addWidget(m_pSearchText, 0, Qt::AlignVCenter | Qt::AlignLeft);
    hLayout->addSpacing(30);

    connect(m_pSearchText, &QLineEdit::textChanged, [=](const QString &) {
        buildWorkList(m_assignment, m_works, m_students, m_grades);
    });

    m_pGraded = new QtMaterialCheckBox(this);
    m_pGraded->setText("Graded");
    m_pGraded->setChecked(true);
    m_pGraded->setMaximumHeight(100);
    m_pGraded->setFont(fontSmall);

    hLayout->addWidget(m_pGraded, 0, Qt::AlignBottom);
    hLayout->addSpacing(30);

    connect(m_pGraded, &QAbstractButton::toggled, [=] {
        buildWorkList(m_assignment, m_works, m_students, m_grades);
    });

    m_pNotGraded = new QtMaterialCheckBox(this);
    m_pNotGraded->setText("Not graded");
    m_pNotGraded->setChecked(true);
    m_pNotGraded->setMaximumHeight(100);
    m_pNotGraded->setFont(fontSmall);

    hLayout->addWidget(m_pNotGraded, 0, Qt::AlignBottom);
    hLayout->addStretch(1);

    connect(m_pNotGraded, &QAbstractButton::toggled, [=] {
        buildWorkList(m_assignment, m_works, m_students, m_grades);
    });

    groupStageBox->setLayout(hLayout);
    groupStageBox->setContentsMargins(30, 10, 30, 10);

    groupStageBox->setMaximumHeight(90);
    groupStageBox->setMaximumWidth(500);

    return groupStageBox;
}

QWidget *SubmittedWorkDialog::setupWorkListBox()
{
    QFont font("Roboto", 12, QFont::Normal);

    m_pWorkListGroupBox = new QGroupBox(tr("Assignments list"), this);
    m_pWorkListGroupBox->setFont(font);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addSpacing(30);

    QWidget *scrollAreaHolder = new QWidget(this);
    QVBoxLayout *scrollAreaLayout = new QVBoxLayout(scrollAreaHolder);
    scrollAreaLayout->setAlignment(Qt::AlignTop);

    m_pWorkListArea = new QScrollArea(this);
    m_pWorkListLayout = new QVBoxLayout(this);

    m_pWorkListArea->setWidget(scrollAreaHolder);
    m_pWorkListArea->setWidgetResizable(true);

    QtMaterialScrollBar *verticalScrollbar = new QtMaterialScrollBar(this);
    QtMaterialScrollBar *horizontalScrollbar = new QtMaterialScrollBar(this);

    horizontalScrollbar->setOrientation(Qt::Horizontal);

    horizontalScrollbar->setHideOnMouseOut(false);
    verticalScrollbar->setHideOnMouseOut(false);

    m_pWorkListLayout = new QVBoxLayout(this);

    m_pWorkListArea->setVerticalScrollBar(verticalScrollbar);
    m_pWorkListArea->setHorizontalScrollBar(horizontalScrollbar);
    m_pWorkListArea->setFrameShape(QFrame::NoFrame);

    m_pWorkListLayout->addWidget(m_pWorkListArea, 10, Qt::AlignTop | Qt::AlignHCenter);

    m_pWorkListArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    m_pWorkListGroupBox->setLayout(m_pWorkListLayout);
    m_pWorkListGroupBox->setContentsMargins(30, 10, 30, 10);
    m_pWorkListGroupBox->installEventFilter(this);

    m_pWorkListGroupBox->setMaximumWidth(500);

    return m_pWorkListGroupBox;
}

QWidget *SubmittedWorkDialog::setupWorkBox()
{
    QFont font("Roboto", 12, QFont::Normal);

    m_pWorkGroupBox = new QGroupBox(tr("SubmittedWork"), this);
    m_pWorkGroupBox->setFont(font);
    m_pWorkGroupBox->setAutoFillBackground(true);

    m_pWorkGroupBox->setStyleSheet("QWidget { background: white; }");

    m_pWorkLayout = new QVBoxLayout(this);
    m_pWorkLayout->addSpacing(30);

    m_pWorkGroupBox->installEventFilter(this);

    m_pWorkGroupBox->setLayout(m_pWorkLayout);
    m_pWorkGroupBox->setContentsMargins(30, 10, 30, 10);

    m_pWorkGroupBox->installEventFilter(this);

    return m_pWorkGroupBox;
}

bool SubmittedWorkDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_pWorkListGroupBox) {
        switch (event->type()) {
        case QEvent::Move:
        case QEvent::Resize: {
            m_pWorkListArea->setGeometry(
                m_pWorkListGroupBox->contentsRect().adjusted(1, 10, -1, -10));
        } break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void SubmittedWorkDialog::buildWorkList(const std::shared_ptr<Assignment> &assignment,
                                        const QList<std::shared_ptr<Work>> &works,
                                        const QList<std::shared_ptr<Student>> &students,
                                        const QList<std::shared_ptr<Grade>> &grades)
{
    m_assignment = assignment;
    m_works = works;
    m_students = students;
    m_grades = grades;

    QList<std::shared_ptr<Work>> orderedList(works);

    std::sort(orderedList.begin(),
              orderedList.end(),
              [](const std::shared_ptr<Work> &p1, const std::shared_ptr<Work> &p2) {
                  return p1->getId() < p2->getId();
              });
    std::reverse(orderedList.begin(), orderedList.end());
    m_works = orderedList;

    QVBoxLayout *widgetLayout = ((QVBoxLayout *) m_pWorkListArea->widget()->layout());
    removeWidgets(widgetLayout);

    int assignmentCount = orderedList.count();
    for (int i = 0; i < assignmentCount; i++) {
        Work *pWork = orderedList.at(i).get();
        Student *pStudent = getStudent(pWork->getLogin()).get();
        if (pStudent == nullptr) {
            qDebug() << "work builder -> not found student " << pWork->getLogin();
            continue;
        }

        bool isExpired = assignment->getDeadLine().isNull()
                         || assignment->getDeadLine() < pWork->getDateTime();
        bool isGraded = pWork->getIsChecked();

        if (!m_pGraded->isChecked() && isGraded) {
            continue;
        }

        if (!m_pNotGraded->isChecked() && !isGraded) {
            continue;
        }

        QString workTitle = QString("%1 %2 (%3-%4)")
                                .arg(pStudent->getName())
                                .arg(pStudent->getSurname())
                                .arg(pStudent->getGroup())
                                .arg(pStudent->getSubgroup());
        QString assignmentDescription = QString("Submission time: %1")
                                            .arg(pWork->getDateTime().toString(
                                                "yyyy/MM/dd HH:mm:ss"));

        if (!m_pSearchText->text().isEmpty()) {
            QString loweredText = m_pSearchText->text().toLower();
            bool haveAny = (!pWork->getComment().isNull()
                            && pWork->getComment().toLower().contains(loweredText))
                           || pWork->getLogin().toLower().contains(loweredText)
                           || workTitle.toLower().contains(loweredText)
                           || assignmentDescription.toLower().contains(loweredText);
            if (!haveAny) {
                continue;
            }
        }

        QImage studentAvatar;
        studentAvatar.loadFromData(pStudent->getAvatar());
        QtMaterialCard *pCard = new QtMaterialCard(72,
                                                   studentAvatar,
                                                   workTitle,
                                                   assignmentDescription,
                                                   false,
                                                   false,
                                                   m_pCurrentRequest == pWork
                                                       ? (isExpired ? QColor(238, 140, 145)
                                                                    : QColor(238, 240, 245))
                                                       : QColor(),
                                                   QList<QWidget *>(),
                                                   this);
        pCard->setDescriptionColor(QColor(107, 126, 143));
        if (isExpired) {
            QtMaterialBadge *badge = new QtMaterialBadge("Late", pCard);
            badge->setBackgroundColor(QColor(160, 198, 232));
            badge->setRelativePosition(200, 0);
        }

        widgetLayout->addWidget(pCard, 1, Qt::AlignHCenter);
        widgetLayout->addSpacing(5);

        connect(pCard, &QtMaterialCard::clicked, [this, pCard, pWork, widgetLayout, isExpired]() {
            int widgetCount = widgetLayout->count();
            for (int j = 0; j < widgetCount; j++) {
                QtMaterialCard *itemCard = static_cast<QtMaterialCard *>(
                    widgetLayout->itemAt(j)->widget());
                if (itemCard == nullptr) {
                    continue;
                }

                if (pCard == itemCard) {
                    itemCard->setPanelColor(isExpired ? QColor(238, 140, 145)
                                                      : QColor(238, 240, 245));
                    showWork(m_pWorkLayout, pWork);
                } else {
                    itemCard->setPanelColor(QColor());
                }

                itemCard->update();
            }
        });

        if (m_pCurrentRequest == nullptr) {
            showWork(m_pWorkLayout, pWork);
        }
    }
}

void SubmittedWorkDialog::addHeaderField(QVBoxLayout *headerLayout,
                                   const QString &labelText,
                                   QWidget *fieldContent,
                                   int labelWidth)
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addSpacing(10);
    QLabel *pLabel = new QLabel();
    QFont font("Roboto", 10, QFont::Normal);

    pLabel->setText(labelText);
    pLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    pLabel->setFont(font);

    pLabel->setMinimumWidth(labelWidth);
    hLayout->addSpacing(10);
    hLayout->addWidget(pLabel, 1, Qt::AlignLeft | Qt::AlignVCenter);
    hLayout->addSpacing(10);
    hLayout->addWidget(fieldContent, 1, Qt::AlignLeft | Qt::AlignVCenter);
    hLayout->addSpacing(10);

    headerLayout->addSpacing(5);
    headerLayout->addLayout(hLayout);
    headerLayout->addSpacing(5);
}

std::shared_ptr<Student> SubmittedWorkDialog::getStudent(const QString &studentLogin)
{
    for (int i = 0; i < m_students.size(); i++) {
        if (m_students.at(i)->getLogin() == studentLogin) {
            return m_students.at(i);
        }
    }

    return nullptr;
}

std::shared_ptr<Grade> SubmittedWorkDialog::getGrade(const QString &studentLogin)
{
    for (int i = 0; i < m_grades.size(); i++) {
        if (m_grades.at(i)->getNameOfStudent() == studentLogin) {
            return m_grades.at(i);
        }
    }

    return nullptr;
}

void SubmittedWorkDialog::buildImageLayout(QVBoxLayout *parentLayout, Work *pWork)
{
    QHBoxLayout *hFileLayout = new QHBoxLayout(this);

    QtMaterialRaisedButton *pSaveFile = new QtMaterialRaisedButton(this);
    pSaveFile->setText("Save file to disk");
    pSaveFile->setDisabled(pWork->getAssign().isNull());
    connect(pSaveFile, &QAbstractButton::clicked, [=] {
        QString userFile = QFileDialog::getSaveFileName(this,
                                                        "Save data file",
                                                        AppSettings::instance().getLastDirectory(),
                                                        "Images (*.png *.xpm *.jpg *.pdf)");
        if (!userFile.isNull() && !userFile.isEmpty()) {
            AppSettings::instance().setLastDirectory(userFile);

            QFile file(userFile);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                MainAppWindow::instance().postError("Failed create file");
                return;
            }

            file.write(pWork->getAssign());
            file.close();

            MainAppWindow::instance().postMessage("File saved");
        }
    });

    QtMaterialRaisedButton *pOpenFile = new QtMaterialRaisedButton(this);
    pOpenFile->setText("Open in hint");
    pOpenFile->setDisabled(pWork->getAssign().isNull());
    connect(pOpenFile, &QAbstractButton::clicked, [=] {
        hintWindow = std::unique_ptr<Hint>(new Hint(this));
        hintWindow->setHint(pWork->getAssign(), pWork->getIsPdf());
        hintWindow->show();
    });

    hFileLayout->addSpacing(10);
    hFileLayout->addWidget(pSaveFile);
    hFileLayout->addSpacing(20);
    hFileLayout->addWidget(pOpenFile);
    hFileLayout->addSpacing(10);

    parentLayout->addLayout(hFileLayout);

    QVBoxLayout *imageLayout = new QVBoxLayout(this);
    m_images.clear();

    if (!pWork->getAssign().isNull()) {
        if (!pWork->getIsPdf()) {
            QImage image;
            image.loadFromData(pWork->getAssign());
            m_images.push_back(image);

            addImageToLayout(imageLayout, image);
        } else {
            std::unique_ptr<Poppler::Document> doc = Poppler::Document::loadFromData(
                pWork->getAssign());
            if (doc != nullptr) {
                doc->setRenderHint(Poppler::Document::TextAntialiasing);
                for (int i = 0; i < doc->numPages(); i++) {
                    std::unique_ptr<Poppler::Page> pdfPage = doc->page(i);
                    if (pdfPage == nullptr) {
                        MainAppWindow::instance().postError("Failed get page from PDF");
                        break;
                    }

                    QImage image = pdfPage->renderToImage(72.0,
                                                          72.0,
                                                          0,
                                                          0,
                                                          pdfPage->pageSize().width(),
                                                          pdfPage->pageSize().height());
                    m_images.push_back(image);
                    if (i == 0) {
                        addImageToLayout(imageLayout, image);
                    }
                }
            }
        }
    } else {
        addImageToLayout(imageLayout, QImage());
        pOpenFile->setDisabled(true);
        pSaveFile->setDisabled(true);
    }

    parentLayout->addLayout(imageLayout, 1);

    QHBoxLayout *hPageLayout = new QHBoxLayout(this);

    QtMaterialRaisedButton *prevPage = new QtMaterialRaisedButton(this);
    prevPage->setText("Previous page");
    prevPage->setIcon(QIcon(":/icons/icons/navigation/svg/production/ic_arrow_back_24px.svg"));
    prevPage->setDisabled(true);

    QLabel *pPageLabel = new QLabel(this);
    pPageLabel->setAlignment(Qt::AlignCenter);
    pPageLabel->setText("1");
    pPageLabel->setDisabled(true);

    QtMaterialRaisedButton *nextPage = new QtMaterialRaisedButton(this);
    nextPage->setText("Next page");
    nextPage->setIcon(QIcon(":/icons/icons/navigation/svg/production/ic_arrow_forward_24px.svg"));
    nextPage->setIconPlacement(Material::ButtonIconPlacement::RightIcon);
    nextPage->setDisabled(m_images.size() > 1 ? false : true);

    hPageLayout->addSpacing(100);
    hPageLayout->addWidget(prevPage);
    hPageLayout->addSpacing(50);
    hPageLayout->addWidget(pPageLabel);
    hPageLayout->addSpacing(50);
    hPageLayout->addWidget(nextPage);
    hPageLayout->addSpacing(100);

    connect(prevPage, &QAbstractButton::clicked, [=] {
        int switchToPage = pPageLabel->text().toInt() - 1;

        addImageToLayout(imageLayout, m_images.at(switchToPage - 1));

        pPageLabel->setText(QString::number(switchToPage));
        if (switchToPage == 1) {
            prevPage->setDisabled(true);
        } else if (int(m_images.size()) > 1) {
            prevPage->setDisabled(false);
        }
        nextPage->setDisabled(false);
    });

    connect(nextPage, &QAbstractButton::clicked, [=] {
        int switchToPage = pPageLabel->text().toInt() + 1;

        addImageToLayout(imageLayout, m_images.at(switchToPage - 1));

        pPageLabel->setText(QString::number(switchToPage));
        prevPage->setDisabled(false);
        if (switchToPage >= m_images.size()) {
            nextPage->setDisabled(true);
        } else {
            nextPage->setDisabled(false);
        }
    });
    parentLayout->addLayout(hPageLayout);
}

void SubmittedWorkDialog::showWork(QVBoxLayout *pLayout, Work *pWork)
{
    if (pLayout == nullptr) {
        return;
    }

    QHBoxLayout *hBoxLayout = new QHBoxLayout(this);
    QVBoxLayout *vBoxLayoutControls = new QVBoxLayout(this);
    QVBoxLayout *vBoxLayoutImage = new QVBoxLayout(this);
    QWidget *pWidget = new QWidget(this);
    pWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    hBoxLayout->addLayout(vBoxLayoutImage);
    hBoxLayout->addSpacing(20);
    hBoxLayout->addLayout(vBoxLayoutControls);

    pWidget->setContentsMargins(20, 20, 20, 20);
    pWidget->setLayout(hBoxLayout);

    int fieldWide = 200;
    QtMaterialTextField *studentName = new QtMaterialTextField(this);
    std::shared_ptr<Student> student = getStudent(pWork->getLogin());

    studentName->setText(QString("%1 %2").arg(student->getName()).arg(student->getSurname()));
    studentName->setReadOnly(true);
    studentName->setMinimumWidth(200);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Student name",
                   studentName,
                   fieldWide);
    studentName->setReadOnly(true);

    QtMaterialTextField *submissionTime = new QtMaterialTextField(this);
    submissionTime->setText(pWork->getDateTime().toString("yyyy/MM/dd HH:mm:ss"));
    submissionTime->setReadOnly(true);
    submissionTime->setMinimumWidth(200);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Submission time",
                   submissionTime,
                   fieldWide);
    submissionTime->setReadOnly(true);

    QtMaterialCheckBox *checkGraded = new QtMaterialCheckBox(this);
    checkGraded->setChecked(pWork->getIsChecked());
    checkGraded->setText("Is graded");
    checkGraded->setLabelPosition(QtMaterialCheckBox::LabelPosition::LabelPositionRight);
    checkGraded->setDisabled(true);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls), "", checkGraded, fieldWide);

    QtMaterialCheckBox *checkLate = new QtMaterialCheckBox(this);
    checkLate->setChecked(pWork->getIsLate());
    checkLate->setText("Is expired");
    checkLate->setLabelPosition(QtMaterialCheckBox::LabelPosition::LabelPositionRight);
    checkLate->setCheckedColor(pWork->getDateTime() > m_assignment->getDeadLine()
                                   ? QColor(255, 0, 0)
                                   : checkLate->checkedColor());
    checkLate->setDisabled(true);

    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls), "", checkLate, fieldWide);

    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setText(pWork->getComment());
    textEdit->setMaximumHeight(100);
    QFont fontSmall("Roboto", 10, QFont::Normal);
    textEdit->setFont(fontSmall);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls), "Comment", textEdit, fieldWide);
    textEdit->setReadOnly(true);

    vBoxLayoutControls->addSpacing(20);

    QtMaterialDivider *divider = new QtMaterialDivider(this);
    vBoxLayoutControls->addWidget(divider);
    vBoxLayoutControls->addSpacing(50);

    std::shared_ptr<Grade> grade = getGrade(pWork->getLogin());

    if (grade != nullptr) {
        QtMaterialTextField *gradedByField = new QtMaterialTextField(this);
        gradedByField->setText(grade->getExaminerLogin());
        addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                       "Graded by",
                       gradedByField,
                       fieldWide);
        gradedByField->setReadOnly(true);

        QtMaterialTextField *gradedAtField = new QtMaterialTextField(this);
        gradedAtField->setText(
            QDateTime(grade->getDate(), grade->getGradeTime()).toString("yyyy/MM/dd HH:mm:ss"));
        addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                       "Graded at",
                       gradedAtField,
                       fieldWide);
        gradedAtField->setReadOnly(true);
    }

    bool isStudent = AuthData::instance().getRole() == IAuthRepository::STUDENT;

    QtMaterialTextField *gradeValue = new QtMaterialTextField(this);
    if (!isStudent) {
        gradeValue->setLabel("Grade");
        gradeValue->setPlaceholderText("Enter grade value");
    }
    if (grade != nullptr) {
        gradeValue->setText(QString::number(grade->getMark()));
    }
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Grade value",
                   gradeValue,
                   fieldWide);
    gradeValue->setReadOnly(isStudent);

    QTextEdit *gradeComment = new QTextEdit(this);
    if (grade != nullptr) {
        gradeComment->setText(grade->getComment());
    }

    gradeComment->setMaximumHeight(100);
    gradeComment->setFont(QFont("Roboto", 10, QFont::Normal));
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Grade comment",
                   gradeComment,
                   fieldWide);
    if (grade != nullptr) {
        gradeComment->setText(grade->getComment());
    }
    gradeComment->setReadOnly(isStudent);

    if (!isStudent) {
        QtMaterialRaisedButton *saveChanges = new QtMaterialRaisedButton(this);
        saveChanges->setText(grade == nullptr ? "Set grade" : "Regrade");
        saveChanges->setMinimumWidth(300);
        saveChanges->setRole(grade == nullptr ? Material::Primary : Material::Secondary);
        saveChanges->setDisabled(gradeValue->text().isEmpty() ? true : false);

        connect(saveChanges, &QAbstractButton::clicked, [=] {
            if (grade == nullptr) {
                Grade *newGrade = new Grade();
                newGrade->setAssigned(true);
                newGrade->setAssignmentId(m_nAssignmentId);
                newGrade->setAssignmentName(m_assignment->getName());
                newGrade->setComment(gradeComment->toPlainText());
                newGrade->setDate(QDate::currentDate());
                newGrade->setGradeTime(QTime::currentTime());
                newGrade->setElementOfControl(m_assignment->getControl());
                newGrade->setMark(gradeValue->text().toDouble());
                newGrade->setModule(currentModule(m_assignment->getDeadLine()));
                newGrade->setName(student->getName());
                newGrade->setSurName(student->getSurname());
                newGrade->setNameOfStudent(student->getLogin());
                newGrade->setSubject(m_assignment->getSubject());

                if (gradeRepository->addGrade(*newGrade)) {
                    m_grades.append(std::shared_ptr<Grade>(newGrade));
                    pWork->setIsChecked(true);
                    workRepository->updateWork(*pWork);
                    MainAppWindow::instance().postMessage("Grade added");
                    QTimer::singleShot(100, [=] { showWork(m_pWorkLayout, pWork); });
                } else {
                    MainAppWindow::instance().postError("Error at submitting grade");
                }
            } else {
                Grade *newGrade = grade.get();
                newGrade->setAssigned(true);
                newGrade->setDate(QDate::currentDate());
                newGrade->setGradeTime(QTime::currentTime());
                newGrade->setComment(gradeComment->toPlainText());

                newGrade->setMark(gradeValue->text().toDouble());
                if (gradeRepository->updateGrade(*newGrade)) {
                    pWork->setIsChecked(true);
                    MainAppWindow::instance().postMessage("Grade updated");
                    QTimer::singleShot(100, [=] { showWork(m_pWorkLayout, pWork); });
                } else {
                    MainAppWindow::instance().postError("Error at submitting grade");
                }
            }
        });

        vBoxLayoutControls->addSpacing(30);
        vBoxLayoutControls->addWidget(saveChanges, 0, Qt::AlignCenter);

        connect(gradeValue, &QLineEdit::textChanged, [=](const QString &text) {
            bool isok;
            double value = text.toDouble(&isok);
            if (isok && value > 0.0) {
                saveChanges->setDisabled(false);
            } else {
                saveChanges->setDisabled(true);
            }
        });
    }

    vBoxLayoutControls->addStretch(1);

    buildImageLayout(vBoxLayoutImage, pWork);
    vBoxLayoutImage->addStretch();

    removeWidgets(pLayout);
    pLayout->addWidget(pWidget);

    m_pCurrentRequest = pWork;

}

const QString SubmittedWorkDialog::currentModule(const QDateTime &forTime)
{
    int month = (forTime.isNull() ? QDate::currentDate() : forTime.date()).month();
    if (month >= 1 && month < 4)
        return "3rd Module";
    else if (month < 9)
        return "4th Module";
    else if (month < 11)
        return "1st Module";
    else
        return "2nd Module";
}

void SubmittedWorkDialog::addImageToLayout(QVBoxLayout *imageLayout, const QImage &image)
{
    removeWidgets(imageLayout);

    QWidget *widget = new QWidget;
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    widget->setLayout(vLayout);

    QLabel *label = new QLabel;
    label->setScaledContents(true);
    label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    if (!image.isNull()) {
        double aspectRatio = double(image.width()) / double(image.height());
        label->setMinimumWidth(300);
        label->setMinimumHeight(int(300.0 / aspectRatio));

        label->setPixmap(QPixmap::fromImage(image));
    } else {
        label->setMinimumHeight(614);
        label->setMinimumWidth(400);
        label->setStyleSheet("QLabel {"
                             "border-top:1px solid #D8D8D8;"
                             "border-left:1px solid #D8D8D8;"
                             "border-right:1px solid #D8D8D8;"
                             "border-bottom: 1px solid #D8D8D8;"
                             "background-color:#D8D8D8;"
                             "padding-bottom:10px;"
                             "}");
    }
    vLayout->addWidget(label, 1, Qt::AlignCenter);
    imageLayout->addWidget(widget, 1, Qt::AlignCenter);
}

