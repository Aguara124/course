#include "assignmenttab.h"
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
#include "workers/teachergetassistant.h"

#include "../common/submittedworkdialog.h"
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

AssignmentTab::AssignmentTab(const QString &teacherLogin, QWidget *parent)
    : QWidget{parent}
    , teacherLogin(teacherLogin)
    , oddColor(new QBrush(QColor(240, 240, 240)))
    , evenColor(new QBrush(Qt::GlobalColor::white))
    , assignmentRepository(RepositoryManager::instance().assignmentRepository())
    , m_pCurrentRequest(nullptr)
    , m_pGradeTable(nullptr)
{
}

void AssignmentTab::setupTabLayout()
{
    setStyleSheet("QWidget { background: white; }");

    m_pTabLayout = new QHBoxLayout(this);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);

    vBoxLayout->addWidget(setupFilterBox());

    QtMaterialRaisedButton *addButton = new QtMaterialRaisedButton(this);
    addButton->setText("Add new assignment");
    addButton->setRole(Material::Primary);
    addButton->setBackgroundColor(QColor(160, 198, 232));

    connect(addButton, &QAbstractButton::clicked, [=] {
        Assignment *assignment = new Assignment;
        assignment->setId(0);
        assignment->setExaminerName(teacherLogin);
        assignment->setGroup(getAvailableGroups().first().split('-').first().toInt());
        assignment->setSubGroup(getAvailableGroups().first().split('-').last().toInt());
        assignment->setSubject(getAvailableSubjects().first());
        assignment->setControl("HW");
        assignment->setDeadLine(QDateTime::currentDateTime().addMonths(1));
        assignment->setName("New assignment");

        assignments.append(std::shared_ptr<Assignment>(assignment));

        m_pCurrentRequest = assignment;
        updateAssignments(assignments);
        showAssignment(m_pAssignmentRequestLayout, assignment);
    });

    vBoxLayout->addWidget(addButton);

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

    GetTeacher *getTeacher = new GetTeacher(teacherLogin);
    connect(getTeacher, &GetTeacher::result, [=](const QList<std::shared_ptr<Teacher>> &result) {
        teachers = result;
    });
    getTeacher->start();

    GetAssistants *getAssistants = new GetAssistants(teacherLogin);
    connect(getAssistants,
            &GetAssistants::result,
            [=](const QList<std::shared_ptr<Assistant>> &result) { assistants = result; });
    getAssistants->start();

    GetAssignments *getAssignments = new GetAssignments(teacherLogin, true);
    connect(getAssignments, &GetAssignments::result, this, &AssignmentTab::updateAssignments);
    getAssignments->start();
}


void AssignmentTab::updateAssignments(const QList<std::shared_ptr<Assignment>> &result)
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
    assignments = orderedList;

    QVBoxLayout *widgetLayout = ((QVBoxLayout *) m_pAssignmentsArea->widget()->layout());
    removeWidgets(widgetLayout);

    int assignmentCount = orderedList.count();
    for (int i = 0; i < assignmentCount; i++) {
        Assignment *pAssignment = orderedList.at(i).get();
        Assistant *pAssistant = getAssistant(pAssignment->getExaminerName());

        bool isExpired = pAssignment->getDeadLine().isNull()
                         || pAssignment->getDeadLine() < QDateTime::currentDateTime();
        if (!m_pExpired->isChecked() && isExpired) {
            continue;
        }

        bool isActual = !pAssignment->getDeadLine().isNull()
                        && pAssignment->getDeadLine() > QDateTime::currentDateTime();
        if (!m_pNotExpired->isChecked() && isActual) {
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
        examinerAvatar.loadFromData(pAssistant == nullptr ? teachers.first()->getAvatar()
                                                          : pAssistant->getAvatar());

        QtMaterialCard *pCard = new QtMaterialCard(72,
                                                   examinerAvatar,
                                                   assignmentTitle,
                                                   assignmentDescription,
                                                   false,
                                                   false,
                                                   m_pCurrentRequest == pAssignment
                                                       ? (isAssignmentExpired(pAssignment)
                                                              ? QColor(207, 226, 243)
                                                              : QColor(238, 240, 245))
                                                       : QColor(),
                                                   QList<QWidget *>(),
                                                   this);
        pCard->setMinimumWidth(500);
        pCard->setDescriptionColor(QColor(107, 126, 143));
        if (isAssignmentExpired(pAssignment)) {
            QtMaterialBadge *badge = new QtMaterialBadge("expired", pCard);
            badge->setBackgroundColor(QColor(160, 198, 232));
            badge->setRelativePosition(200, 0);
        }

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
                    itemCard->setPanelColor(isAssignmentExpired(pAssignment)
                                                ? QColor(207, 226, 243)
                                                : QColor(238, 240, 245));
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

    //widgetLayout->addStretch(10);
}

QWidget *AssignmentTab::setupFilterBox()
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
        updateAssignments(assignments);
    });

    m_pExpired = new QtMaterialCheckBox(this);
    m_pExpired->setText("Expired");
    m_pExpired->setChecked(true);
    m_pExpired->setMaximumHeight(100);
    m_pExpired->setFont(fontSmall);
    m_pExpired->setCheckedColor(QColor(207, 226, 243));

    hLayout->addWidget(m_pExpired, 0, Qt::AlignBottom);
    hLayout->addSpacing(30);

    connect(m_pExpired, &QAbstractButton::toggled, [=] { updateAssignments(assignments); });

    m_pNotExpired = new QtMaterialCheckBox(this);
    m_pNotExpired->setText("Not expired");
    m_pNotExpired->setChecked(true);
    m_pNotExpired->setMaximumHeight(100);
    m_pNotExpired->setFont(fontSmall);
    m_pNotExpired->setCheckedColor(QColor(207, 226, 243));

    hLayout->addWidget(m_pNotExpired, 0, Qt::AlignBottom);
    hLayout->addStretch(1);

    connect(m_pNotExpired, &QAbstractButton::toggled, [=] { updateAssignments(assignments); });

    groupStageBox->setLayout(hLayout);
    groupStageBox->setContentsMargins(30, 10, 30, 10);

    groupStageBox->setMaximumHeight(90);
    groupStageBox->setMaximumWidth(550);

    return groupStageBox;
}

QWidget *AssignmentTab::setupAssignmentBox()
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

QWidget *AssignmentTab::setupAssignmentListBox()
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

    horizontalScrollbar->setOrientation(Qt::Horizontal);

    horizontalScrollbar->setHideOnMouseOut(false);
    verticalScrollbar->setHideOnMouseOut(false);

    horizontalScrollbar->setSliderColor(QColor(207, 226, 243));
    verticalScrollbar->setSliderColor(QColor(207, 226, 243));

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

bool AssignmentTab::eventFilter(QObject *watched, QEvent *event)
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

void AssignmentTab::addHeaderField(QVBoxLayout *headerLayout,
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

QStringList AssignmentTab::getAvailableGroups()
{
    QStringList groups;

    for (int i = 0; i < teachers.size(); i++) {
        if (teachers.at(i)->getLecturer()) {
            QString group = QString("%1-%2")
                                .arg(teachers.at(i)->getGroup())
                                .arg(teachers.at(i)->getSubGroup());
            if (!groups.contains(group)) {
                groups.append(group);
            }
        }
    }

    for (int i = 0; i < assistants.size(); i++) {
        QString group = QString("%1-%2")
                            .arg(assistants.at(i)->getGroup())
                            .arg(assistants.at(i)->getSubGroup());
        if (!groups.contains(group)) {
            groups.append(group);
        }
    }

    return groups;
}

QStringList AssignmentTab::getAvailableSubjects()
{
    QStringList subjects;

    for (int i = 0; i < teachers.size(); i++) {
        if (teachers.at(i)->getLecturer()) {
            if (!subjects.contains(teachers.at(i)->getSubject())) {
                subjects.append(teachers.at(i)->getSubject());
            }
        }
    }

    for (int i = 0; i < assistants.size(); i++) {
        if (!subjects.contains(assistants.at(i)->getSubject())) {
            subjects.append(assistants.at(i)->getSubject());
        }
    }

    return subjects;
}

QString AssignmentTab::getAssigner(const QString &assignerLogin)
{
    for (int i = 0; i < teachers.size(); i++) {
        if (teachers.at(i)->getLogin() == assignerLogin) {
            return QString("%1 %2").arg(teachers.at(i)->getName()).arg(teachers.at(i)->getSurname());
        }
    }

    for (int i = 0; i < assistants.size(); i++) {
        if (assistants.at(i)->getLogin() == assignerLogin) {
            return QString("%1 %2")
                .arg(assistants.at(i)->getName())
                .arg(assistants.at(i)->getSurname());
        }
    }

    return QString(assignerLogin);
}

void AssignmentTab::buildImageLayout(QVBoxLayout *parentLayout, Assignment *pAssignment)
{
    QHBoxLayout *hFileLayout = new QHBoxLayout(this);

    QtMaterialRaisedButton *pOtherFile = new QtMaterialRaisedButton(this);
    pOtherFile->setBackgroundColor(QColor(204, 204, 204));
    pOtherFile->setText(pAssignment->getAssign().isNull() ? "Add file" : "Replace file");

    connect(pOtherFile, &QAbstractButton::clicked, [=] {
        QString userFile = QFileDialog::getOpenFileName(this,
                                                        "Open data file",
                                                        AppSettings::instance().getLastDirectory(),
                                                        "Images (*.png *.xpm *.jpg *.pdf)");
        if (!userFile.isNull() && !userFile.isEmpty()) {
            AppSettings::instance().setLastDirectory(userFile);

            QFile file(userFile);
            if (!file.open(QIODevice::ReadOnly)) {
                MainAppWindow::instance().postError("Failed open file");
                return;
            }

            m_pCurrentRequest->setAssign(file.readAll());
            m_pCurrentRequest->setIsPdf(userFile.endsWith(".pdf"));
            file.close();

            QTimer::singleShot(100, [=] {
                showAssignment(m_pAssignmentRequestLayout, m_pCurrentRequest);
            });
        }
    });

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
    pOpenFile->setBackgroundColor(QColor(204, 204,204));
    pOpenFile->setText("Open in hint");
    pOpenFile->setDisabled(pAssignment->getAssign().isNull());
    connect(pOpenFile, &QAbstractButton::clicked, [=] {
        hintWindow = std::unique_ptr<Hint>(new Hint(this));
        hintWindow->setHint(pAssignment->getAssign(), pAssignment->getIsPdf());
        hintWindow->show();
    });

    hFileLayout->addSpacing(50);
    hFileLayout->addWidget(pOtherFile);
    hFileLayout->addSpacing(20);
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
        pOtherFile->setDisabled(false);
        pOpenFile->setDisabled(true);
        pSaveFile->setDisabled(true);
    }

    parentLayout->addLayout(imageLayout);

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

void AssignmentTab::showAssignment(QVBoxLayout *pLayout, Assignment *pAssignment)
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
    nameOfAssigner->setText(getAssigner(pAssignment->getExaminerName()));
    nameOfAssigner->setReadOnly(true);
    nameOfAssigner->setMinimumWidth(200);
    nameOfAssigner->setInkColor(QColor(160, 198, 232));
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Name of assigner",
                   nameOfAssigner,
                   fieldWide);

    QtMaterialTextField *nameOfAssignment = new QtMaterialTextField(this);
    nameOfAssignment->setPlaceholderText("Enter name of assignment");
    nameOfAssignment->setText(pAssignment->getName());
    nameOfAssignment->setMinimumWidth(200);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Name of assignment",
                   nameOfAssignment,
                   fieldWide);
    connect(nameOfAssignment, &QLineEdit::textChanged, [=](const QString &text) {
        m_pCurrentRequest->setName(text);
    });

    QtMaterialSelectBox *groupSelector = new QtMaterialSelectBox(this);
    groupSelector->setInkColor(QColor(160, 198, 232));
    groupSelector->setText(
        QString("%1-%2").arg(pAssignment->getGroup()).arg(pAssignment->getSubGroup()));
    groupSelector->setDataSource(getAvailableGroups());
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Group",
                   groupSelector,
                   fieldWide);
    connect(groupSelector, &QtMaterialSelectBox::itemSelected, [=](QString item) {
        m_pCurrentRequest->setGroup(item.split('-').first().toInt());
        m_pCurrentRequest->setSubGroup(item.split('-').last().toInt());

        QByteArray newGradeTable = generateGradeTable(m_pCurrentRequest->getGroup(),
                                                      m_pCurrentRequest->getSubGroup());
        m_pCurrentRequest->setGradingTable(newGradeTable);
        loadGradeTableToWidget(m_pGradeTable, m_pCurrentRequest);
        adjustGradeTableGeometry();
    });

    QtMaterialSelectBox *subjectSelector = new QtMaterialSelectBox(this);
    subjectSelector->setInkColor(QColor(160, 198, 232));
    subjectSelector->setText(pAssignment->getSubject());
    subjectSelector->setDataSource(getAvailableSubjects());
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Subject",
                   subjectSelector,
                   fieldWide);
    connect(subjectSelector, &QtMaterialSelectBox::itemSelected, [=](QString item) {
        m_pCurrentRequest->setSubject(item);
    });

    QtMaterialSelectBox *typeSelector = new QtMaterialSelectBox(this);
    typeSelector->setInkColor(QColor(160, 198, 232));
    typeSelector->setText(pAssignment->getControl());
    typeSelector->setDataSource(
        QStringList{"HW", "Quiz", "Test", "Laboratory", "Classwork", "Bonus Task", "Pesentation"});
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls),
                   "Control form type",
                   typeSelector,
                   fieldWide);
    connect(typeSelector, &QtMaterialSelectBox::itemSelected, [=](QString item) {
        m_pCurrentRequest->setControl(item);
    });

    QtMaterialTextField *deadLine = new QtMaterialTextField(this);
    deadLine->setPlaceholderText("Enter YYYY/MM/DD HH:MM");
    deadLine->setInputMask("9999/99/99 99:99:99");
    deadLine->setMinimumWidth(150);
    deadLine->setText(pAssignment->getDeadLine().isNull()
                          ? QString()
                          : pAssignment->getDeadLine().toString("yyyy/MM/dd HH:mm:ss"));
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls), "Dead line", deadLine, fieldWide);
    connect(deadLine, &QLineEdit::textChanged, [=](const QString &text) {
        try {
            QDateTime value = QDateTime::fromString(QString(text).replace(':', '-'),
                                                    "yyyy/MM/dd HH-mm-ss");
            if (value.isNull() || value < QDateTime::currentDateTime()) {
                deadLine->setTextColor(QColor(255, 0, 0));
            } else {
                m_pCurrentRequest->setDeadLine(value);
                deadLine->setTextColor(QColor(0, 0, 0));
            }
        } catch (...) {
            MainAppWindow::instance().postError("Error in deadline formating");
        }
    });

    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setText(pAssignment->getComment());
    textEdit->setMaximumHeight(100);
    QFont fontSmall("Roboto", 10, QFont::Normal);
    textEdit->setFont(fontSmall);
    addHeaderField(static_cast<QVBoxLayout *>(vBoxLayoutControls), "Comment", textEdit, fieldWide);

    connect(textEdit, &QTextEdit::textChanged, [=] {
        m_pCurrentRequest->setComment(textEdit->toPlainText());
    });

    QtMaterialRaisedButton *viewWorks = new QtMaterialRaisedButton(this);
    viewWorks->setText("View submitted works");
    viewWorks->setBackgroundColor(QColor(160, 198, 232));
    viewWorks->setRole(Material::Role::Primary);
    viewWorks->setDisabled(pAssignment->getId() == 0);

    connect(viewWorks, &QAbstractButton::clicked, [=] {
        SubmittedWorkDialog *workDialog = new SubmittedWorkDialog(pAssignment->getId(), this);
        workDialog->setupLayout();
        workDialog->show();
    });

    vBoxLayoutControls->addWidget(viewWorks);
    vBoxLayoutControls->addSpacing(20);

    QHBoxLayout *opButtonGroup = new QHBoxLayout(this);

    QtMaterialRaisedButton *saveChanges = new QtMaterialRaisedButton(this);
    saveChanges->setText("Save changes");
    saveChanges->setBackgroundColor(QColor(204, 204, 204));
    saveChanges->setForegroundColor(QColor(89, 89, 89));
    opButtonGroup->addWidget(saveChanges);

    connect(saveChanges, &QAbstractButton::clicked, [=] {
        if (m_pCurrentRequest->getId() == 0) {
            if (assignmentRepository->addAssignment(*m_pCurrentRequest)) {
                MainAppWindow::instance().postMessage("Assignment added");
                GetAssignments *getAssignments = new GetAssignments(teacherLogin, true);
                connect(getAssignments,
                        &GetAssignments::result,
                        this,
                        &AssignmentTab::resetAssignmentsSelection);
                getAssignments->start();
            } else {
                MainAppWindow::instance().postError(
                    "Unknown error occured while saving assignment");
            }
        } else {
            if (assignmentRepository->updateAssignment(*m_pCurrentRequest)) {
                MainAppWindow::instance().postMessage("Assignment updated");
                QTimer::singleShot(100, [=] { updateAssignments(assignments); });
            } else {
                MainAppWindow::instance().postError(
                    "Unknown error occured while saving assignment");
            }
        }
    });

    opButtonGroup->addSpacing(10);

    QtMaterialRaisedButton *cancelChanges = new QtMaterialRaisedButton(this);
    cancelChanges->setText("Cancel");
    cancelChanges->setBackgroundColor(QColor(204, 204, 204));
    cancelChanges->setForegroundColor(QColor(89, 89, 89));

    connect(cancelChanges, &QAbstractButton::clicked, [=] {
        if (m_pCurrentRequest != nullptr && m_pCurrentRequest->getId() > 0) {
            std::shared_ptr<Assignment> previousVersion = assignmentRepository->getById(
                pAssignment->getId());
            if (previousVersion != nullptr) {
                for (int i = 0; i < assignments.size(); i++) {
                    if (assignments.at(i)->getId() == previousVersion->getId()) {
                        assignments.replace(i, previousVersion);
                        m_pCurrentRequest = assignments.at(i).get();
                        QTimer::singleShot(100, [=] {
                            showAssignment(m_pAssignmentRequestLayout, m_pCurrentRequest);
                        });
                    }
                }
            }
        }
    });

    opButtonGroup->addWidget(cancelChanges);

    vBoxLayoutControls->addLayout(opButtonGroup);
    vBoxLayoutControls->addStretch(1);

    QHBoxLayout *gradeButtonGroup = new QHBoxLayout(this);

    QtMaterialTextField *columnName = new QtMaterialTextField(this);
    columnName->setMinimumWidth(50);
    columnName->setPlaceholderText("New column name");

    gradeButtonGroup->addWidget(columnName);
    gradeButtonGroup->addSpacing(10);

    QtMaterialRaisedButton *addColumn = new QtMaterialRaisedButton(this);
    addColumn->setText("Add column");
    addColumn->setBackgroundColor(QColor(204, 204, 204));
    addColumn->setForegroundColor(QColor(89, 89, 89));

    connect(addColumn, &QAbstractButton::clicked, [=] {
        int pos = m_pGradeTable->columnCount();
        m_pGradeTable->setColumnCount(pos + 1);
        QTableWidgetItem *newHeader = new QTableWidgetItem(
            columnName->text().isNull() || columnName->text().isEmpty() ? QString::number(pos + 1)
                                                                        : columnName->text());
        m_pGradeTable->setHorizontalHeaderItem(pos, newHeader);

        m_pGradeTable->blockSignals(true);
        for (int i = 0; i < m_pGradeTable->rowCount(); i++) {
            addTableItem(m_pGradeTable, i, pos, QString::number(pos + 1), true);
        }
        m_pCurrentRequest->setGradingTable(getGradeTable());
        m_pGradeTable->blockSignals(false);
    });

    gradeButtonGroup->addWidget(addColumn);
    gradeButtonGroup->addSpacing(10);

    QtMaterialRaisedButton *deleteColumn = new QtMaterialRaisedButton(this);
    deleteColumn->setText("Delete column");
    deleteColumn->setBackgroundColor(QColor(204, 204, 204));
    deleteColumn->setForegroundColor(QColor(89, 89, 89));
    connect(deleteColumn, &QAbstractButton::clicked, [=] {
        if (!m_pGradeTable->selectedItems().isEmpty() && m_pGradeTable->columnCount() > 1) {
            m_pGradeTable->removeColumn(m_pGradeTable->selectedItems().first()->column());
        }
    });

    gradeButtonGroup->addWidget(deleteColumn);

    vBoxLayoutControls->addLayout(gradeButtonGroup);

    buildImageLayout(vBoxLayoutImage, pAssignment);
    vBoxLayoutImage->addStretch();

    removeWidgets(pLayout);
    pLayout->addWidget(pWidget);

    m_pGradeTable = setupGradeTable(pAssignment);
    m_pGradeTable->setMinimumHeight(200);

    pLayout->addWidget(m_pGradeTable);

    adjustGradeTableGeometry();
    m_pCurrentRequest = pAssignment;
}

void AssignmentTab::resetAssignmentsSelection(const QList<std::shared_ptr<Assignment>> &result)
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

Assistant *AssignmentTab::getAssistant(const QString &assistantLogin)
{
    for (int i = 0; i < assistants.size(); i++) {
        if (assistants.at(i)->getLogin() == assistantLogin) {
            return assistants.at(i).get();
        }
    }

    return nullptr;
}

bool AssignmentTab::isAssignmentExpired(Assignment *pAssignment)
{
    return pAssignment->getDeadLine().isNull() || !pAssignment->getDeadLine().isValid()
           || pAssignment->getDeadLine() < QDateTime::currentDateTime();
}

QByteArray AssignmentTab::getGradeTable()
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

QByteArray AssignmentTab::generateGradeTable(int group, int subGroup)
{
    std::unique_ptr<IStudentRepository> studentRepository = RepositoryManager::instance()
                                                                .studentRepository();
    QList<std::shared_ptr<Student>> students = studentRepository->getByGroup(group,
                                                                             subGroup,
                                                                             QString());
    QStringList headers{"Student", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    QStringList rows;
    rows.append(headers.join('\t'));

    if (students.isEmpty()) {
        QStringList rowValue;
        rowValue.append(QString("-"));
        for (int i = 1; i < 11; i++) {
            rowValue.append(QString::number(i));
        }
        rows.append(rowValue.join('\t'));
    } else {
        for (int row = 0; row < students.size(); row++) {
            QStringList rowValue;
            rowValue.append(QString("%1 %2 (%3)")
                                .arg(students.at(row)->getName())
                                .arg(students.at(row)->getSurname())
                                .arg(students.at(row)->getLogin()));
            for (int i = 1; i < 11; i++) {
                rowValue.append(QString::number(0));
            }
            rows.append(rowValue.join('\t'));
        }
    }

    QString result = rows.join('\n');
    return result.toUtf8();
}

QTableWidget *AssignmentTab::setupGradeTable(Assignment *pAssignment)
{
    QTableWidget *tableWidget = new QTableWidget(this);

    QtMaterialScrollBar *verticalScrollbar = new QtMaterialScrollBar(this);
    QtMaterialScrollBar *horizontalScrollbar = new QtMaterialScrollBar(this);

    horizontalScrollbar->setOrientation(Qt::Horizontal);
    horizontalScrollbar->setHideOnMouseOut(false);

    verticalScrollbar->setHideOnMouseOut(false);

    verticalScrollbar->setSliderColor(QColor(207, 226, 243));
    horizontalScrollbar->setSliderColor(QColor(207, 226, 243));

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

    connect(tableWidget, &QTableWidget::cellChanged, [=](int row, int column) {
        m_pCurrentRequest->setGradingTable(getGradeTable());
    });

    return tableWidget;
}

void AssignmentTab::loadGradeTableToWidget(QTableWidget *tableWidget, Assignment *pAssignment)
{
    QList<QByteArray> gradeTable = pAssignment->getGradingTable().split('\n');
    if (!gradeTable.isEmpty()) {
        QStringList checkedTable;
        for (int i = 0; i < gradeTable.size(); i++) {
            QString row = gradeTable.at(i);
            if (!row.isNull() && !row.trimmed().isEmpty()) {
                checkedTable.append(row);
            }
        }

        QStringList headers = checkedTable.first().split('\t');
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

void AssignmentTab::addTableItem(
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

void AssignmentTab::addImageToLayout(QVBoxLayout *imageLayout, const QImage &image)
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

void AssignmentTab::adjustGradeTableGeometry()
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
