#include "studentassignmenttab.h"
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

#include "qtmaterialbadge.h"
#include "workers/getassignments.h"
#include "workers/getassignmentsforstudent.h"

#include "../common/submittedworkdialog.h"
#include "../hint.h"
#include "askconsultation.h"
#include "submitworkdialog.h"
#include <qtmaterialfab.h>

StudentAssignmentTab::StudentAssignmentTab(const QString &studentLogin, QWidget *parent)
    : QWidget{parent}
    , m_studentLogin(studentLogin)
    , oddColor(new QBrush(QColor(240, 240, 240)))
    , evenColor(new QBrush(Qt::GlobalColor::white))
    , assignmentRepository(RepositoryManager::instance().assignmentRepository())
    , m_pCurrentRequest(nullptr)
    , m_pGradeTable(nullptr)
{
}

void StudentAssignmentTab::setupTabLayout()
{
    setStyleSheet("QWidget { background: white; }");

    m_pTabLayout = new QHBoxLayout(this);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);

    vBoxLayout->addWidget(setupFilterBox());

    vBoxLayout->addWidget(setupAssignmentListBox());

    m_pTabLayout->addStretch();
    m_pTabLayout->addLayout(vBoxLayout);
    m_pTabLayout->addSpacing(30);
    m_pTabLayout->addWidget(setupAssignmentBox());
    m_pTabLayout->addStretch();

    setLayout(m_pTabLayout);
    setContentsMargins(0, 0, 0, 0);

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(policy);

    GetStudent *getStudent = new GetStudent(m_studentLogin);
    connect(getStudent, &GetStudent::result, [=](const std::shared_ptr<Student> &student) {
        m_student = student;
        GetAssignmentsForStudent *getAssignments
            = new GetAssignmentsForStudent(m_studentLogin,
                                           student->getGroup(),
                                           student->getSubgroup());

        connect(getAssignments,
                &GetAssignmentsForStudent::result,
                this,
                &StudentAssignmentTab::onReceiveAssignments);
        getAssignments->start();
    });

    getStudent->start();
}

void StudentAssignmentTab::onReceiveAssignments(const QList<std::shared_ptr<Assignment>> &assignments,
                                                const QList<std::shared_ptr<Work>> &works,
                                                const QList<std::shared_ptr<Grade>> &grades,
                                                const QList<std::shared_ptr<Teacher>> &teachers,
                                                const QList<std::shared_ptr<Assistant>> &assistants)
{
    m_assignments = assignments;
    m_works = works;
    m_grades = grades;
    m_teachers = teachers;
    m_assistants = assistants;

    updateAssignments(m_assignments);
}

QList<std::shared_ptr<Work>> StudentAssignmentTab::getWork(int assignmentId)
{
    QList<std::shared_ptr<Work>> works;

    for (int i = 0; i < m_works.size(); i++) {
        if (m_works.at(i)->getAssignmentId() == assignmentId) {
            works.append(m_works.at(i));
        }
    }

    return works;
}

bool StudentAssignmentTab::isAssignmentSolved(int assignmentId)
{
    for (int i = 0; i < m_works.size(); i++) {
        if (m_works.at(i)->getAssignmentId() == assignmentId) {
            return true;
        }
    }

    return false;
}

void StudentAssignmentTab::updateAssignments(const QList<std::shared_ptr<Assignment>> &result)
{
    QList<std::shared_ptr<Assignment>> orderedList(result);

    std::sort(orderedList.begin(),
              orderedList.end(),
              [](const std::shared_ptr<Assignment> &p1, const std::shared_ptr<Assignment> &p2) {
                  if (p1->getDeadLine() == p2->getDeadLine()) {
                      return p1->getId() < p2->getId();
                  }

                  return p1->getDeadLine() < p2->getDeadLine();
              });
    std::reverse(orderedList.begin(), orderedList.end());
    m_assignments = orderedList;

    QVBoxLayout *widgetLayout = ((QVBoxLayout *) m_pAssignmentsArea->widget()->layout());
    removeWidgets(widgetLayout);

    int assignmentCount = orderedList.count();
    for (int i = 0; i < assignmentCount; i++) {
        Assignment *pAssignment = orderedList.at(i).get();
        Assistant *pAssistant = getAssistant(pAssignment->getExaminerName());
        Teacher *pTeacher = getTeacher(pAssignment->getExaminerName());

        bool isSolved = isAssignmentSolved(pAssignment->getId());
        if (!m_pSolved->isChecked() && isSolved) {
            continue;
        }

        if (!m_pUnsolved->isChecked() && !isSolved) {
            continue;
        }

        QString assignmentTitle
            = QString("%1 (%2)").arg(pAssignment->getName()).arg(pAssignment->getSubject());
        QString assignmentDescription = QString("Group: %1-%2 Deadline: %3")
                                            .arg(pAssignment->getGroup())
                                            .arg(pAssignment->getSubGroup())
                                            .arg(pAssignment->getDeadLine().isNull()
                                                     ? "not defined"
                                                     : pAssignment->getDeadLine().toString());

        if (!m_pSearchText->text().isEmpty()) {
            QString loweredText = m_pSearchText->text().toLower();
            bool haveAny = (!pAssignment->getComment().isNull()
                            && pAssignment->getComment().toLower().contains(loweredText))
                           || pAssignment->getControl().toLower().contains(loweredText)
                           || assignmentTitle.contains(loweredText)
                           || assignmentDescription.toLower().contains(loweredText);
            if (!haveAny) {
                continue;
            }
        }

        QImage examinerAvatar;
        examinerAvatar.loadFromData(pAssistant == nullptr ? pTeacher->getAvatar()
                                                          : pAssistant->getAvatar());

        QtMaterialCard *pCard = new QtMaterialCard(72,
                                                   examinerAvatar,
                                                   assignmentTitle,
                                                   assignmentDescription,
                                                   false,
                                                   false,
                                                   m_pCurrentRequest == pAssignment
                                                       ? QColor(238, 240, 245)
                                                       : QColor(),
                                                   QList<QWidget *>(),
                                                   this);
        pCard->setDescriptionColor(QColor(107, 126, 143));
        pCard->setMinimumWidth(500);
        widgetLayout->addWidget(pCard, 1, Qt::AlignHCenter);
        widgetLayout->addSpacing(5);

        connect(pCard, &QtMaterialCard::clicked, [this, pCard, pAssignment, widgetLayout]() {
            int widgetCount = widgetLayout->count();
            for (int j = 0; j < widgetCount; j++) {
                QtMaterialCard *itemCard = static_cast<QtMaterialCard *>(
                    widgetLayout->itemAt(j)->widget());
                if (itemCard == nullptr) {
                    continue;
                }

                if (pCard == itemCard) {
                    itemCard->setPanelColor(QColor(238, 240, 245));
                    showAssignment(m_pAssignmentRequestLayout, pAssignment);
                } else {
                    itemCard->setPanelColor(QColor());
                }

                itemCard->update();
            }
        });

        if (m_pCurrentRequest == nullptr) {
            showAssignment(m_pAssignmentRequestLayout, pAssignment);
        }
    }
}

QWidget *StudentAssignmentTab::setupFilterBox()
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
    m_pSearchText->setMinimumWidth(200);
    m_pSearchText->setInkColor(QColor(160, 198, 232));

    hLayout->addWidget(m_pSearchText, 0, Qt::AlignVCenter | Qt::AlignLeft);
    hLayout->addSpacing(30);

    connect(m_pSearchText, &QLineEdit::textChanged, [=](const QString &) {
        updateAssignments(m_assignments);
    });

    m_pSolved = new QtMaterialCheckBox(this);
    m_pSolved->setText("Solved");
    m_pSolved->setChecked(true);
    m_pSolved->setMaximumHeight(100);
    m_pSolved->setFont(fontSmall);
    m_pSolved->setCheckedColor(QColor(207, 226, 243));

    hLayout->addWidget(m_pSolved, 0, Qt::AlignBottom);
    hLayout->addSpacing(30);

    connect(m_pSolved, &QAbstractButton::toggled, [=] { updateAssignments(m_assignments); });

    m_pUnsolved = new QtMaterialCheckBox(this);
    m_pUnsolved->setText("Unsolved");
    m_pUnsolved->setChecked(true);
    m_pUnsolved->setMaximumHeight(100);
    m_pUnsolved->setFont(fontSmall);
    m_pUnsolved->setCheckedColor(QColor(207, 226, 243));

    hLayout->addWidget(m_pUnsolved, 0, Qt::AlignBottom);
    hLayout->addStretch(1);

    connect(m_pUnsolved, &QAbstractButton::toggled, [=] { updateAssignments(m_assignments); });

    groupStageBox->setLayout(hLayout);
    groupStageBox->setContentsMargins(30, 10, 30, 10);

    groupStageBox->setMaximumHeight(90);
    groupStageBox->setMaximumWidth(550);

    return groupStageBox;
}

QWidget *StudentAssignmentTab::setupAssignmentBox()
{
    QFont font("Roboto", 12, QFont::Normal);

    m_pAssignmentRequestBox = new QGroupBox(tr("Assignment"), this);
    m_pAssignmentRequestBox->setFont(font);
    m_pAssignmentRequestBox->setAutoFillBackground(true);

    m_pAssignmentRequestBox->setStyleSheet("QWidget { background: white; }");

    m_pAssignmentRequestLayout = new QVBoxLayout(this);
    m_pAssignmentRequestLayout->addSpacing(30);

    m_pAssignmentRequestBox->installEventFilter(this);

    m_pAssignmentRequestBox->setLayout(m_pAssignmentRequestLayout);
    m_pAssignmentRequestBox->setContentsMargins(30, 10, 30, 10);

    m_pAssignmentRequestBox->installEventFilter(this);

    return m_pAssignmentRequestBox;
}

QWidget *StudentAssignmentTab::setupAssignmentListBox()
{
    QFont font("Roboto", 12, QFont::Normal);

    m_pAssignListGroupBox = new QGroupBox(tr("Assignments list"), this);
    m_pAssignListGroupBox->setFont(font);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addSpacing(30);

    QWidget *scrollAreaHolder = new QWidget(this);
    QVBoxLayout *scrollAreaLayout = new QVBoxLayout(scrollAreaHolder);
    scrollAreaLayout->setAlignment(Qt::AlignTop);

    m_pAssignmentsArea = new QScrollArea(this);
    m_pAssignmentsLayout = new QVBoxLayout(this);

    m_pAssignmentsArea->setWidget(scrollAreaHolder);
    m_pAssignmentsArea->setWidgetResizable(true);

    QtMaterialScrollBar *verticalScrollbar = new QtMaterialScrollBar(this);
    QtMaterialScrollBar *horizontalScrollbar = new QtMaterialScrollBar(this);

    verticalScrollbar->setSliderColor(QColor(207, 226, 243));
    horizontalScrollbar->setSliderColor(QColor(207, 226, 243));

    horizontalScrollbar->setOrientation(Qt::Horizontal);

    horizontalScrollbar->setHideOnMouseOut(false);
    verticalScrollbar->setHideOnMouseOut(false);

    m_pAssignmentsLayout = new QVBoxLayout(this);

    m_pAssignmentsArea->setVerticalScrollBar(verticalScrollbar);
    m_pAssignmentsArea->setHorizontalScrollBar(horizontalScrollbar);
    m_pAssignmentsArea->setFrameShape(QFrame::NoFrame);

    m_pAssignmentsLayout->addWidget(m_pAssignmentsArea, 10, Qt::AlignTop | Qt::AlignHCenter);

    m_pAssignmentsArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_pAssignmentsArea->setMinimumWidth(530);

    m_pAssignListGroupBox->setLayout(m_pAssignmentsLayout);
    m_pAssignListGroupBox->setContentsMargins(30, 10, 30, 10);
    m_pAssignListGroupBox->installEventFilter(this);

    return m_pAssignListGroupBox;
}

bool StudentAssignmentTab::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_pAssignListGroupBox) {
        switch (event->type()) {
        case QEvent::Move:
        case QEvent::Resize: {
            m_pAssignmentsArea->setGeometry(
                m_pAssignListGroupBox->contentsRect().adjusted(1, 10, -1, -10));
        } break;
        default:
            break;
        }
    } else if (watched == m_pAssignmentRequestBox && m_pGradeTable != nullptr) {
        switch (event->type()) {
        case QEvent::Move:
        case QEvent::Resize:
            adjustGradeTableGeometry();
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void StudentAssignmentTab::addHeaderField(QVBoxLayout *headerLayout,
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
    hLayout->addWidget(pLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    hLayout->addSpacing(10);
    hLayout->addWidget(fieldContent, 0, Qt::AlignLeft | Qt::AlignVCenter);
    hLayout->addSpacing(10);

    headerLayout->addSpacing(5);
    headerLayout->addLayout(hLayout);
    headerLayout->addSpacing(5);
}


QString StudentAssignmentTab::getAssigner(const QString &assignerLogin)
{
    for (int i = 0; i < m_teachers.size(); i++) {
        if (m_teachers.at(i)->getLogin() == assignerLogin) {
            return QString("%1 %2")
                .arg(m_teachers.at(i)->getName())
                .arg(m_teachers.at(i)->getSurname());
        }
    }

    for (int i = 0; i < m_assistants.size(); i++) {
        if (m_assistants.at(i)->getLogin() == assignerLogin) {
            return QString("%1 %2")
                .arg(m_assistants.at(i)->getName())
                .arg(m_assistants.at(i)->getSurname());
        }
    }

    return QString(assignerLogin);
}

void StudentAssignmentTab::buildImageLayout(QVBoxLayout *parentLayout, Assignment *pAssignment)
{
    QHBoxLayout *hFileLayout = new QHBoxLayout(this);

    QtMaterialRaisedButton *pSaveFile = new QtMaterialRaisedButton(this);
    pSaveFile->setBackgroundColor(QColor(204, 204, 204));
    pSaveFile->setText("Save file to disk");
    pSaveFile->setDisabled(pAssignment->getAssign().isNull());
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

            file.write(pAssignment->getAssign());
            file.close();

            MainAppWindow::instance().postMessage("File saved");
        }
    });

    QtMaterialRaisedButton *pOpenFile = new QtMaterialRaisedButton(this);
    pOpenFile->setBackgroundColor(QColor(204, 204, 204));
    pOpenFile->setText("Open in hint");
    pOpenFile->setDisabled(pAssignment->getAssign().isNull());
    connect(pOpenFile, &QAbstractButton::clicked, [=] {
        hintWindow = std::unique_ptr<Hint>(new Hint(this));
        hintWindow->setHint(pAssignment->getAssign(), pAssignment->getIsPdf());
        hintWindow->show();
    });

    hFileLayout->addSpacing(50);
    hFileLayout->addWidget(pSaveFile);
    hFileLayout->addSpacing(20);
    hFileLayout->addWidget(pOpenFile);
    hFileLayout->addSpacing(50);

    parentLayout->addLayout(hFileLayout);

    QVBoxLayout *imageLayout = new QVBoxLayout(this);
    m_images.clear();

    if (!pAssignment->getAssign().isNull()) {
        if (!pAssignment->getIsPdf()) {
            QImage image;
            image.loadFromData(pAssignment->getAssign());
            m_images.push_back(image);

            addImageToLayout(imageLayout, image);
        } else {
            std::unique_ptr<Poppler::Document> doc = Poppler::Document::loadFromData(
                pAssignment->getAssign());
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

    parentLayout->addLayout(imageLayout, 0);

    QHBoxLayout *hPageLayout = new QHBoxLayout(this);

    QtMaterialRaisedButton *prevPage = new QtMaterialRaisedButton(this);
    prevPage->setText("Previous page");
    prevPage->setIcon(QIcon(":/icons/icons/navigation/svg/production/ic_arrow_back_24px.svg"));
    prevPage->setDisabled(true);
    prevPage->setBackgroundColor(QColor(160, 198, 232));

    QLabel *pPageLabel = new QLabel(this);
    pPageLabel->setAlignment(Qt::AlignCenter);
    pPageLabel->setText("1");
    pPageLabel->setDisabled(true);

    QtMaterialRaisedButton *nextPage = new QtMaterialRaisedButton(this);
    nextPage->setText("Next page");
    nextPage->setIcon(QIcon(":/icons/icons/navigation/svg/production/ic_arrow_forward_24px.svg"));
    nextPage->setIconPlacement(Material::ButtonIconPlacement::RightIcon);
    nextPage->setDisabled(m_images.size() > 1 ? false : true);
    nextPage->setBackgroundColor(QColor(160, 198, 232));

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

void StudentAssignmentTab::showAssignment(QVBoxLayout *pLayout, Assignment *pAssignment)
{
    if (pLayout == nullptr) {
        return;
    }

    QHBoxLayout *hBoxLayout = new QHBoxLayout(this);
    QVBoxLayout *vBoxLayoutControls = new QVBoxLayout(this);
    QVBoxLayout *vBoxLayoutImage = new QVBoxLayout(this);

    QWidget *pWidget = new QWidget(this);
    pWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    hBoxLayout->addLayout(vBoxLayoutControls);
    hBoxLayout->addSpacing(20);
    hBoxLayout->addLayout(vBoxLayoutImage);

    pWidget->setContentsMargins(20, 20, 20, 20);
    pWidget->setLayout(hBoxLayout);


    int fieldWide = 200;
    QtMaterialTextField *nameOfAssigner = new QtMaterialTextField(this);
    nameOfAssigner->setInkColor(QColor(160, 198, 232));
    nameOfAssigner->setText(getAssigner(pAssignment->getExaminerName()));
    nameOfAssigner->setReadOnly(true);
    nameOfAssigner->setMinimumWidth(200);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Name of assigner",
                   nameOfAssigner,
                   fieldWide);

    QtMaterialTextField *nameOfAssignment = new QtMaterialTextField(this);
    nameOfAssignment->setInkColor(QColor(160, 198, 232));
    nameOfAssignment->setPlaceholderText("Enter name of assignment");
    nameOfAssignment->setText(pAssignment->getName());
    nameOfAssignment->setMinimumWidth(200);
    nameOfAssignment->setReadOnly(true);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Name of assignment",
                   nameOfAssignment,
                   fieldWide);
    QtMaterialTextField *groupSelector = new QtMaterialTextField(this);
    groupSelector->setInkColor(QColor(160, 198, 232));
    groupSelector->setText(
        QString("%1-%2").arg(pAssignment->getGroup()).arg(pAssignment->getSubGroup()));
    groupSelector->setReadOnly(true);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Group",
                   groupSelector,
                   fieldWide);

    QtMaterialTextField *subjectSelector = new QtMaterialTextField(this);
    subjectSelector->setInkColor(QColor(160, 198, 232));
    subjectSelector->setText(pAssignment->getSubject());
    subjectSelector->setReadOnly(true);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Subject",
                   subjectSelector,
                   fieldWide);

    QtMaterialTextField *typeSelector = new QtMaterialTextField(this);
    typeSelector->setInkColor(QColor(160, 198, 232));
    typeSelector->setText(pAssignment->getControl());
    typeSelector->setReadOnly(true);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Control form type",
                   typeSelector,
                   fieldWide);

    QtMaterialTextField *deadLine = new QtMaterialTextField(this);
    deadLine->setPlaceholderText("Enter YYYY/MM/DD HH:MM");
    deadLine->setInputMask("9999/99/99 99:99:99");
    deadLine->setText(pAssignment->getDeadLine().isNull()
                          ? QString()
                          : pAssignment->getDeadLine().toString("yyyy/MM/dd HH:mm:ss"));
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls), "Dead line", deadLine, fieldWide);
    deadLine->setReadOnly(true);
    deadLine->setInkColor(QColor(160, 198, 232));
    deadLine->setMinimumWidth(150);

    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setText(pAssignment->getComment());
    textEdit->setMaximumHeight(100);
    QFont fontSmall("Roboto", 10, QFont::Normal);
    textEdit->setFont(fontSmall);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls), "Comment", textEdit, fieldWide);
    textEdit->setReadOnly(true);

    QtMaterialRaisedButton *submitWorkButton = new QtMaterialRaisedButton(this);
    submitWorkButton->setBackgroundColor(QColor(204, 204, 204));
    submitWorkButton->setForegroundColor(QColor(89, 89, 89));
    submitWorkButton->setText("Submit work");
    submitWorkButton->setRole(Material::Role::Primary);

    connect(submitWorkButton, &QAbstractButton::clicked, [=] {
        SubmitWorkDialog *workDialog = new SubmitWorkDialog(m_student, pAssignment, this);
        workDialog->setupForm();

        connect(workDialog, &SubmitWorkDialog::doSubmit, [=](const std::shared_ptr<Work> &work) {
            qDebug() << "dosubmit";
            m_works.append(work);
            QTimer::singleShot(100, [=] {
                updateAssignments(m_assignments);
                showAssignment(m_pAssignmentRequestLayout, m_pCurrentRequest);
            });

            MainAppWindow::instance().hideDialog(workDialog);
        });

        connect(workDialog, &SubmitWorkDialog::doCancel, [=]() {
            qDebug() << "docancel";
            MainAppWindow::instance().hideDialog(workDialog);
        });

        connect(workDialog, &SubmitWorkDialog::doHelp, [=]() {
            qDebug() << "dohelp";
            MainAppWindow::instance().help("dummyhelp.pdf");
        });

        MainAppWindow::instance().showDialog(workDialog);        
    });

    QtMaterialRaisedButton *viewWorks = new QtMaterialRaisedButton(this);
    viewWorks->setBackgroundColor(QColor(204, 204, 204));
    submitWorkButton->setForegroundColor(QColor(89, 89, 89));
    viewWorks->setText("View submitted works");
    viewWorks->setRole(Material::Role::Primary);
    viewWorks->setDisabled(!isAssignmentSolved(pAssignment->getId()));

    connect(viewWorks, &QAbstractButton::clicked, [=] {
        SubmittedWorkDialog *workDialog = new SubmittedWorkDialog(pAssignment->getId(), this);
        workDialog->setupLayout();
        workDialog->show();
    });

    vBoxLayoutControls->addSpacing(20);
    vBoxLayoutControls->addWidget(submitWorkButton);
    vBoxLayoutControls->addSpacing(10);
    vBoxLayoutControls->addWidget(viewWorks);
    vBoxLayoutControls->addSpacing(20);

    buildImageLayout(vBoxLayoutImage, pAssignment);

    removeWidgets(pLayout);
    pLayout->addWidget(pWidget);

    m_pGradeTable = setupGradeTable(pAssignment);
    m_pGradeTable->setMinimumHeight(100);

    pLayout->addWidget(m_pGradeTable);

    adjustGradeTableGeometry();

    QtMaterialFloatingActionButton *pFabButton = new QtMaterialFloatingActionButton(
        QtMaterialTheme::icon("action", "help"));
    pFabButton->setParent(this);
    pFabButton->setMini(true);
    pFabButton->setXOffset(72);
    connect(pFabButton, &QAbstractButton::clicked, [=] {
        consultationRequest = new AskConsultationDialog(m_student, m_assistants, m_teachers, this);
        consultationRequest->setupForm();

        connect(consultationRequest, &AskConsultationDialog::doRequest, [=]() {
            qDebug() << "dorequest";

            MainAppWindow::instance().hideDialog(consultationRequest);
        });

        connect(consultationRequest, &AskConsultationDialog::doCancel, [=]() {
            qDebug() << "docancel";
            MainAppWindow::instance().hideDialog(consultationRequest);
        });

        connect(consultationRequest, &AskConsultationDialog::doHelp, [=]() {
            qDebug() << "dohelp";
            MainAppWindow::instance().help("dummyhelp.pdf");
        });

        MainAppWindow::instance().showDialog(consultationRequest);
    });

    m_pCurrentRequest = pAssignment;
}

void StudentAssignmentTab::resetAssignmentsSelection(const QList<std::shared_ptr<Assignment>> &result)
{
    if (result.size()) {
        int maxId = -1;
        for (int i = 0; i < result.size(); i++) {
            if (result.at(i)->getId() > maxId) {
                m_pCurrentRequest = result.at(i).get();
                maxId = result.at(i)->getId();
            }
        }
        updateAssignments(result);
        showAssignment(m_pAssignmentRequestLayout, m_pCurrentRequest);
    }
}

Assistant *StudentAssignmentTab::getAssistant(const QString &assistantLogin)
{
    for (int i = 0; i < m_assistants.size(); i++) {
        if (m_assistants.at(i)->getLogin() == assistantLogin) {
            return m_assistants.at(i).get();
        }
    }

    return nullptr;
}

Teacher *StudentAssignmentTab::getTeacher(const QString &teacherLogin)
{
    for (int i = 0; i < m_teachers.size(); i++) {
        if (m_teachers.at(i)->getLogin() == teacherLogin) {
            return m_teachers.at(i).get();
        }
    }

    return nullptr;
}

bool StudentAssignmentTab::isAssignmentExpired(Assignment *pAssignment)
{
    return pAssignment->getDeadLine().isNull() || !pAssignment->getDeadLine().isValid()
           || pAssignment->getDeadLine() < QDateTime::currentDateTime();
}

QByteArray StudentAssignmentTab::getGradeTable()
{
    int columnCount = m_pGradeTable->columnCount();
    int rowCount = m_pGradeTable->rowCount();

    QStringList headers;
    for (int i = 0; i < columnCount; i++) {
        headers.append(m_pGradeTable->horizontalHeaderItem(i)->text());
    }

    QStringList rows;
    rows.append(headers.join('\t'));

    for (int row = 0; row < rowCount; row++) {
        QStringList currentRow;
        for (int column = 0; column < columnCount; column++) {
            currentRow.append(m_pGradeTable->item(row, column)->text().replace("\t", " "));
        }
        rows.append(currentRow.join('\t'));
    }

    QString result = rows.join('\n');
    return result.toUtf8();
}

QByteArray StudentAssignmentTab::generateGradeTable(int group, int subGroup)
{
    Q_UNUSED(group);
    Q_UNUSED(subGroup);

    QStringList headers{"Student", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    QStringList rows;
    rows.append(headers.join('\t'));

    QStringList rowValue;
    rowValue.append(QString("%1 %2 (%3)")
                        .arg(m_student->getName())
                        .arg(m_student->getSurname())
                        .arg(m_student->getLogin()));
    for (int i = 1; i < 11; i++) {
        rowValue.append(QString::number(i));
    }
            rows.append(rowValue.join('\t'));

            QString result = rows.join('\n');
            return result.toUtf8();
}

QTableWidget *StudentAssignmentTab::setupGradeTable(Assignment *pAssignment)
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

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionsClickable(false);
    tableWidget->horizontalHeader()->setFont(font);
    tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter
                                                         | (Qt::Alignment) Qt::TextWordWrap);

    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{"
                                                   "border-top:1px solid #D8D8D8;"
                                                   "border-left:0px solid #D8D8D8;"
                                                   "border-right:1px solid #D8D8D8;"
                                                   "border-bottom: 2px solid #D8D8D8;"
                                                   "background-color:white;"
                                                   "height: 22px;"
                                                   "padding-top:10px;"
                                                   "padding-bottom:10px;"
                                                   "}");

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

    tableWidget->setFrameShape(QFrame::NoFrame);

    if (pAssignment->getGradingTable().isNull() || pAssignment->getGradingTable().isEmpty()) {
        pAssignment->setGradingTable(
            generateGradeTable(pAssignment->getGroup(), pAssignment->getSubGroup()));
    }

    tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tableWidget->setWordWrap(false);
    tableWidget->setShowGrid(true);

    loadGradeTableToWidget(tableWidget, pAssignment);

    return tableWidget;
}

void StudentAssignmentTab::loadGradeTableToWidget(QTableWidget *tableWidget, Assignment *pAssignment)
{
    QList<QByteArray> gradeTable = pAssignment->getGradingTable().split('\n');
    if (!gradeTable.isEmpty()) {
        QStringList checkedTable;
        QString studentName = QString("%1 %2 (%3)")
                                  .arg(m_student->getName())
                                  .arg(m_student->getSurname())
                                  .arg(m_student->getLogin());

        for (int i = 0; i < gradeTable.size(); i++) {
            QString row = gradeTable.at(i);
            if (!row.isNull() && !row.trimmed().isEmpty()
                && (i == 0 || row.startsWith(studentName))) {
                checkedTable.append(row);
            }
        }

        QStringList headers = checkedTable.first().split('\t');
        // first item is student, so skip
        tableWidget->setColumnCount(headers.size() - 1);

        QStringList headerList;
        for (int i = 1; i < headers.size(); i++) {
            headerList.append(headers.at(i).trimmed());
        }

        tableWidget->setHorizontalHeaderLabels(headerList);
        tableWidget->setRowCount(checkedTable.size() - 1);

        QStringList verticalHeaders;
        for (int row = 1; row < checkedTable.size(); row++) {
            verticalHeaders.append(checkedTable.at(row).split('\t').first().trimmed());
        }

        if (!verticalHeaders.isEmpty()) {
            tableWidget->setVerticalHeaderLabels(verticalHeaders);
        }

        for (int row = 1; row < checkedTable.size(); row++) {
            QStringList columnData = checkedTable.at(row).split('\t');
            for (int column = 1; column < headers.size(); column++) {
                addTableItem(tableWidget, row - 1, column - 1, columnData.at(column).trimmed(), true);
            }
        }
    }

    tableWidget->update();
}

void StudentAssignmentTab::addTableItem(
    QTableWidget *pTableWidget, int row, int column, const QString &itemText, bool isEditable)
{
    QTableWidgetItem *item = new QTableWidgetItem(itemText);
    if (!isEditable) {
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }

    item->setTextAlignment(Qt::AlignCenter);
    item->setBackground((row & 1) ? *evenColor : *oddColor);

    pTableWidget->setItem(row, column, item);
}

void StudentAssignmentTab::addImageToLayout(QVBoxLayout *imageLayout, const QImage &image)
{
    removeWidgets(imageLayout);

    QWidget *widget = new QWidget;
    widget->setLayout(new QVBoxLayout());

    QLabel *label = new QLabel;
    label->setScaledContents(true);
    label->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    if (!image.isNull()) {
        double aspectRatio = double(image.width()) / double(image.height());
        label->setMinimumWidth(300);
        label->setMaximumWidth(400);
        label->setMinimumHeight(int(300.0 / aspectRatio));
        label->setMaximumHeight(int(400.0 / aspectRatio));

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
    widget->layout()->addWidget(label);

    imageLayout->addWidget(widget, 0, Qt::AlignCenter);
}

void StudentAssignmentTab::adjustGradeTableGeometry()
{
    QRect gradeTableRect = m_pGradeTable->geometry();
    QRect newArea = m_pAssignmentRequestBox->contentsRect().adjusted(gradeTableRect.left(),
                                                                     gradeTableRect.top(),
                                                                     -gradeTableRect.left(),
                                                                     -10);

    int columnCount = m_pGradeTable->horizontalHeader()->count();
    if (columnCount > 0) {
        int perColumnWidth = (newArea.width() - (m_pGradeTable->verticalHeader()->width() * 2))
                             / columnCount;
        m_pGradeTable->horizontalHeader()->setDefaultSectionSize(perColumnWidth);
    }
}

void StudentAssignmentTab::removeWidgets(QLayout *pLayout)
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
