#include "consultationassistant.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJSEngine>
#include <QJSValue>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>
#include "../mainappwindow.h"
#include "repositorymanager.h"

#include "workers/fcsgetlecturer.h"
#include "workers/teacherget.h"

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

ConsultationTabAssistant::ConsultationTabAssistant(const QString &assistantLogin, QWidget *parent)
    : QWidget{parent}
    , assistantLogin(assistantLogin)
    , oddColor(new QBrush(QColor(240, 240, 240)))
    , evenColor(new QBrush(Qt::GlobalColor::white))
    , consultationRepository(RepositoryManager::instance().consultationRepository())
    , m_pCurrentRequest(nullptr)
{}

QWidget *ConsultationTabAssistant::setupFilterBox()
{
    QFont font("Roboto", 12, QFont::Normal);
    QFont fontSmall("Roboto", 10, QFont::Normal);

    QGroupBox *groupStageBox = new QGroupBox(tr("Filter requests"), this);
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
        updateRequests(QString(""));
    });

    m_pViewed = new QtMaterialCheckBox(this);
    m_pViewed->setText("Viewed");
    m_pViewed->setChecked(true);
    m_pViewed->setMaximumHeight(100);
    m_pViewed->setFont(fontSmall);
    m_pViewed->setCheckedColor(QColor(207, 226, 243));

    hLayout->addWidget(m_pViewed, 0, Qt::AlignBottom);
    hLayout->addSpacing(30);

    connect(m_pViewed, &QAbstractButton::toggled, [=] { updateRequests(QString("")); });

    m_pNotViewed = new QtMaterialCheckBox(this);
    m_pNotViewed->setText("Not viewed");
    m_pNotViewed->setChecked(true);
    m_pNotViewed->setMaximumHeight(100);
    m_pNotViewed->setFont(fontSmall);
    m_pNotViewed->setCheckedColor(QColor(207, 226, 243));

    hLayout->addWidget(m_pNotViewed, 0, Qt::AlignBottom);
    hLayout->addStretch(1);

    connect(m_pNotViewed, &QAbstractButton::toggled, [=] { updateRequests(QString("")); });

    groupStageBox->setLayout(hLayout);
    groupStageBox->setContentsMargins(30, 10, 30, 10);

    groupStageBox->setMaximumHeight(90);
    groupStageBox->setMaximumWidth(550);

    return groupStageBox;
}

QWidget *ConsultationTabAssistant::setupRequestBox()
{
    QFont font("Roboto", 12, QFont::Normal);

    m_pRequestBox = new QGroupBox(tr("Consultation request"), this);
    m_pRequestBox->setFont(font);
    m_pRequestBox->setAutoFillBackground(true);

    m_pRequestBox->setStyleSheet("QWidget { background: white; }");

    m_pRequestLayout = new QVBoxLayout(this);
    m_pRequestLayout->addSpacing(30);

    m_pRequestBox->installEventFilter(this);

    m_pRequestBox->setLayout(m_pRequestLayout);
    m_pRequestBox->setContentsMargins(30, 10, 30, 10);

    return m_pRequestBox;
}

QWidget *ConsultationTabAssistant::setupRequestListBox()
{
    QFont font("Roboto", 12, QFont::Normal);

    m_pRequestListGroupBox = new QGroupBox(tr("Requests list"), this);
    m_pRequestListGroupBox->setFont(font);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addSpacing(30);

    QWidget *scrollAreaHolder = new QWidget(this);
    QVBoxLayout *scrollAreaLayout = new QVBoxLayout(scrollAreaHolder);
    scrollAreaLayout->setAlignment(Qt::AlignTop);

    m_pStudentArea = new QScrollArea(this);
    m_pStudentLayout = new QVBoxLayout(this);

    m_pStudentArea->setWidget(scrollAreaHolder);
    m_pStudentArea->setWidgetResizable(true);

    QtMaterialScrollBar *verticalScrollbar = new QtMaterialScrollBar(this);
    QtMaterialScrollBar *horizontalScrollbar = new QtMaterialScrollBar(this);

    horizontalScrollbar->setOrientation(Qt::Horizontal);

    horizontalScrollbar->setHideOnMouseOut(false);
    verticalScrollbar->setHideOnMouseOut(false);

    m_pStudentLayout = new QVBoxLayout(this);

    m_pStudentArea->setVerticalScrollBar(verticalScrollbar);
    m_pStudentArea->setHorizontalScrollBar(horizontalScrollbar);
    m_pStudentArea->setFrameShape(QFrame::NoFrame);

    m_pStudentArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_pStudentLayout->addWidget(m_pStudentArea, 10, Qt::AlignTop | Qt::AlignHCenter);

    m_pRequestListGroupBox->setLayout(m_pStudentLayout);
    m_pRequestListGroupBox->setContentsMargins(30, 10, 30, 10);

    m_pRequestListGroupBox->setMaximumWidth(550);
    m_pRequestListGroupBox->installEventFilter(this);

    return m_pRequestListGroupBox;
}

bool ConsultationTabAssistant::eventFilter(QObject *watched, QEvent *event)
{
    if (m_pRequestBox == watched) {
        switch (event->type()) {
        case QEvent::Paint: {
            QPainter painter(m_pRequestBox);
            painter.fillRect(m_pRequestBox->contentsRect(), QColor(255, 5, 5));
            break;
        }
        default:
            break;
        }
    } else if (watched == m_pRequestListGroupBox) {
        switch (event->type()) {
        case QEvent::Resize: {
            m_pStudentArea->setGeometry(
                m_pRequestListGroupBox->contentsRect().adjusted(1, 1, -1, -10));
        } break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void ConsultationTabAssistant::setupTabLayout()
{
    setStyleSheet("QWidget { background: white; }");

    m_pTabLayout = new QHBoxLayout(this);

    QFont font("Roboto", 16, QFont::Normal);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);

    vBoxLayout->addWidget(setupFilterBox());
    vBoxLayout->addWidget(setupRequestListBox());

    m_pTabLayout->addStretch();
    m_pTabLayout->addLayout(vBoxLayout);
    m_pTabLayout->addSpacing(30);
    m_pTabLayout->addWidget(setupRequestBox());
    m_pTabLayout->addStretch();

    setLayout(m_pTabLayout);
    setContentsMargins(0, 0, 0, 0);

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(policy);

    FcsGetProgram *getProgram = new FcsGetProgram(QString());
    connect(getProgram, &FcsGetProgram::result, [=](const QList<std::shared_ptr<Fcs>> &result) {
        for (int i = 0; i < result.size(); i++) {
            m_mapProgramToName[result.at(i)->getProgram()] = result.at(i)->getProgramName();
        }
    });
    getProgram->start();

    GetConsultationRequest *consultationRequest = new GetConsultationRequest(assistantLogin, this);
    connect(consultationRequest,
            &GetConsultationRequest::result,
            [=](const QList<std::shared_ptr<Consultation>> &consultations) {
                this->consultations = consultations;
                // rerequest student
                QStringList studentLogins;
                for (int i = 0; i < consultations.size(); i++) {
                    QString studentLogin = consultations.at(i)->getLogin();
                    if (!studentLogins.contains(studentLogin)) {
                        studentLogins.append(studentLogin);
                        GetStudent *studentRequest = new GetStudent(studentLogin, this);
                        connect(studentRequest,
                                &GetStudent::result,
                                [=](const std::shared_ptr<Student> &result) {
                                    students.append(result);
                                    updateRequests(studentLogin);
                                });

                        studentRequest->start();
                    }
                }
            });
    consultationRequest->start();
}

Student *ConsultationTabAssistant::getStudent(const QString &studentLogin)
{
    for (int i = 0; i < students.size(); i++) {
        if (students.at(i)->getLogin() == studentLogin) {
            return students.at(i).get();
        }
    }

    return nullptr;
}

void ConsultationTabAssistant::updateRequests(const QString &studentLogin)
{
    QList<std::shared_ptr<Consultation>> consList(consultations);

    std::sort(consList.begin(),
              consList.end(),
              [](const std::shared_ptr<Consultation> &p1, const std::shared_ptr<Consultation> &p2) {
                  QDateTime p1Dt(p1->getDate(), p1->getTime());
                  QDateTime p2Dt(p2->getDate(), p2->getTime());

                  if (p1Dt == p2Dt) {
                      return p1->getId() < p2->getId();
                  }

                  return p1Dt < p2Dt;
              });
    std::reverse(consList.begin(), consList.end());

    QVBoxLayout *widgetLayout = ((QVBoxLayout *) m_pStudentArea->widget()->layout());
    QLayoutItem *item;
    while ((item = widgetLayout->takeAt(0)) != 0) {
        if (item->widget()) {
            widgetLayout->removeWidget(item->widget());
            delete item->widget();
        }
        delete item;
    }

    for (int i = 0; i < consList.count(); i++) {
        Consultation *pConsultation = consList.at(i).get();
        Student *pStudent = getStudent(pConsultation->getLogin());
        if (pStudent == nullptr) {
            continue;
        }

        bool showViewed = m_pViewed->isChecked();
        bool showNotViewed = m_pNotViewed->isChecked();

        if (pConsultation->getChecked() && !showViewed) {
            continue;
        }

        if (!pConsultation->getChecked() && !showNotViewed) {
            continue;
        }

        const QString searchText = m_pSearchText->text();
        if (!searchText.isNull() && !searchText.isEmpty()) {
            const QString filter = m_pSearchText->text().toLower();
            bool anyContains = pStudent->getName().toLower().contains(filter)
                               || pStudent->getSurname().toLower().contains(filter)
                               || pStudent->getLogin().toLower().contains(filter)
                               || pStudent->getProgram().toLower().contains(filter)
                               || pConsultation->getSubject().toLower().contains(filter)
                               || pConsultation->getDate().toString().toLower().contains(filter)
                               || pConsultation->getQuestion().toLower().contains(filter)
                               || pConsultation->getExaminerName().toLower().contains(filter)
                               || m_mapProgramToName[pStudent->getProgram()].toLower().contains(
                                   filter);
            if (!anyContains) {
                continue;
            }
        }

        QImage studentAvatar;
        studentAvatar.loadFromData(pStudent->getAvatar());
        QString studentTitle = QString("%1 %2 (%3-%4)")
                                   .arg(pStudent->getName())
                                   .arg(pStudent->getSurname())
                                   .arg(pStudent->getGroup())
                                   .arg(pStudent->getSubgroup());
        QString requestDescription
            = QString("%1 at %2")
                  .arg(pConsultation->getSubject())
                  .arg(QDateTime(pConsultation->getDate(), pConsultation->getTime()).toString());

        QtMaterialCard *pCard = new QtMaterialCard(72,
                                                   studentAvatar,
                                                   studentTitle,
                                                   requestDescription,
                                                   false,
                                                   false,
                                                   QColor(),
                                                   QList<QWidget *>(),
                                                   this);
        pCard->setDescriptionColor(QColor(107, 126, 143));
        pCard->setMinimumWidth(500);
        pCard->setMaximumWidth(500);
        widgetLayout->addWidget(pCard, 0, Qt::AlignHCenter);

        connect(pCard, &QtMaterialCard::clicked, [=]() {
            int widgetCount = widgetLayout->count();
            for (int j = 0; j < widgetCount; j++) {
                QtMaterialCard *itemCard = static_cast<QtMaterialCard *>(
                    widgetLayout->itemAt(j)->widget());
                if (i == j) {
                    itemCard->setPanelColor(QColor(238, 240, 245));
                    showRequest(pStudent, pConsultation);
                } else
                    itemCard->setPanelColor(QColor());

                itemCard->update();
            }
        });

        if (m_pCurrentRequest == nullptr) {
            showRequest(pStudent, pConsultation);
        }
    }
}

void ConsultationTabAssistant::showRequest(Student *pStudent, Consultation *pConsultation)
{
    QLayoutItem *item;
    while ((item = m_pRequestLayout->takeAt(0)) != 0) {
        if (item->widget()) {
            m_pRequestLayout->removeWidget(item->widget());
            delete item->widget();
        }
        delete item;
    }

    QImage studentAvatar;
    studentAvatar.loadFromData(pStudent->getAvatar());
    QString studentTitle = QString("%1 %2").arg(pStudent->getName()).arg(pStudent->getSurname());

    QString requestDescription = QString("Student, %1, %2-%3 %4 ")
                                     .arg(m_mapProgramToName[pStudent->getProgram()].isNull()
                                              ? pStudent->getProgram()
                                              : m_mapProgramToName[pStudent->getProgram()])
                                     .arg(pStudent->getGroup())
                                     .arg(pStudent->getSubgroup())
                                     .arg(pStudent->getPilot() ? "Pilot" : "");
    QtMaterialCard *pCard = new QtMaterialCard(128,
                                               studentAvatar,
                                               studentTitle,
                                               requestDescription,
                                               false,
                                               false,
                                               QColor(),
                                               QList<QWidget *>(),
                                               this);
    pCard->setDescriptionColor(QColor(107, 126, 143));
    pCard->setMinimumWidth(600);
    pCard->setMaximumWidth(600);

    pCard->setMinimumHeight(140);

    m_pRequestLayout->addWidget(pCard, 0, Qt::AlignHCenter);

    QtMaterialDivider *pDivider = new QtMaterialDivider(this);
    pDivider->setMinimumWidth(600);
    pDivider->setMinimumHeight(1);
    m_pRequestLayout->addWidget(pDivider, 0, Qt::AlignHCenter);

    //
    QFont font("Roboto", 10, QFont::Normal);
    QLabel *pQuestionLabel = new QLabel(this);
    pQuestionLabel->setFont(font);
    pQuestionLabel->setWordWrap(true);
    pQuestionLabel->setMinimumWidth(600);

    pQuestionLabel->setText(
        QString("%1, at %2, ask: %3")
            .arg(pStudent->getName(),
                 QDateTime(pConsultation->getDate(), pConsultation->getTime()).toString(),
                 pConsultation->getQuestion()));
    m_pRequestLayout->addWidget(pQuestionLabel, 0, Qt::AlignHCenter);

    QtMaterialRaisedButton *markAsViewed = new QtMaterialRaisedButton(this);
    markAsViewed->setBackgroundColor(QColor(204, 204, 204));
    markAsViewed->setForegroundColor(QColor(89, 89, 89));
    if (pConsultation->getChecked()) {
        markAsViewed->setText("Mark as not viewed");
    } else {
        markAsViewed->setText("Mark as viewed");
    }

    markAsViewed->setContentsMargins(0, 0, 50, 0);
    int consultationId = pConsultation->getId();
    connect(markAsViewed, &QAbstractButton::clicked, [=] {
        for (int i = 0; i < consultations.size(); i++) {
            if (consultations.at(i)->getId() == consultationId) {
                Student *pStudent = getStudent(consultations.at(i)->getLogin());
                consultations.at(i)->setChecked(!consultations.at(i)->getChecked());
                consultationRepository->updateConsultationState(consultationId,
                                                                consultations.at(i)->getChecked());
                QTimer::singleShot(1, [=] { showRequest(pStudent, consultations.at(i).get()); });
                break;
            }
        }

        m_pStudentArea->update();
    });

    m_pRequestLayout->addWidget(markAsViewed, 0, Qt::AlignRight);
    m_pRequestLayout->addStretch();

    m_pCurrentRequest = pConsultation;
}
