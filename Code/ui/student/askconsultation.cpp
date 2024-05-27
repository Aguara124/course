#include "askconsultation.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include "../mainappwindow.h"
#include "imageutils.h"
#include "mainappwindow.h"
#include "qtmaterialselectbox.h"
#include "repositorymanager.h"

AskConsultationDialog::AskConsultationDialog(std::shared_ptr<Student> student,
                                             QList<std::shared_ptr<Assistant>> assistants,
                                             QList<std::shared_ptr<Teacher>> teachers,
                                             QWidget *parent)
    : QtMaterialDialog(parent)
    , vertLayout(new QVBoxLayout(this))
    , horzLayout(new QHBoxLayout(this))
    , requestButton(new QtMaterialRaisedButton(this))
    , cancelButton(new QtMaterialRaisedButton(this))
    , helpButton(
          new QtMaterialIconButton(QIcon(":/icons/icons/action/svg/production/ic_help_24px.svg"),
                                   this))
    , m_student(student)
    , m_assistants(assistants)
    , m_teachers(teachers)
{}

void AskConsultationDialog::addHeaderField(QVBoxLayout *headerLayout,
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

void AskConsultationDialog::clickRequest()
{
    qDebug() << "consultation request attempt ";
    const QString selected = m_pAssistantSelector->text();

    QString login;
    for (int i = 0; i < m_assistants.size(); i++) {
        QString name = QString("%1 %2")
                           .arg(m_assistants.at(i)->getName())
                           .arg(m_assistants.at(i)->getSurname());
        if (name == selected) {
            login = m_assistants.at(i)->getLogin();
            break;
        }
    }

    for (int i = 0; i < m_teachers.size(); i++) {
        QString name
            = QString("%1 %2").arg(m_teachers.at(i)->getName()).arg(m_teachers.at(i)->getSurname());
        if (name == selected) {
            login = m_teachers.at(i)->getLogin();
            break;
        }
    }

    if (!login.isNull() && !login.isEmpty()) {
        Consultation consultation;
        consultation.setChecked(false);
        consultation.setDate(QDate::currentDate());
        consultation.setTime(QTime::currentTime());
        consultation.setExaminerName(login);

        consultation.setSubject(m_pSubjectSelector->text());

        consultation.setGroup(m_student->getGroup());
        consultation.setSubGroup(m_student->getSubgroup());
        consultation.setLogin(m_student->getLogin());
        consultation.setName(m_student->getName());
        consultation.setSurName(m_student->getSurname());

        consultation.setQuestion(m_pTextEdit->toPlainText());

        if (RepositoryManager::instance().consultationRepository()->addConsultation(consultation)) {
            MainAppWindow::instance().postMessage("Request success");
            emit doRequest(m_pAssistantSelector->text(), m_pTextEdit->toPlainText());
        } else {
            MainAppWindow::instance().postError("Request unsuccessfull");
        }
    } else {
        emit doRequest(m_pAssistantSelector->text(), m_pTextEdit->toPlainText());
    }
}

void AskConsultationDialog::clickCancel()
{
    qDebug() << "cancel consultation request ";

    emit doCancel();
}

void AskConsultationDialog::clickHelp()
{
    qDebug() << "help requested";
    emit doHelp();
}

void AskConsultationDialog::setupForm()
{
    vertLayout->addSpacing(30);

    QFont fontLarge("Roboto", 18, QFont::Normal);

    QLabel *label = new QLabel(this);
    label->setText("Consultation\nrequest");
    label->setAlignment(Qt::AlignHCenter);
    label->setFont(fontLarge);
    vertLayout->addWidget(label);

    QStringList names;
    for (int i = 0; i < m_assistants.size(); i++) {
        QString name = QString("%1 %2")
                           .arg(m_assistants.at(i)->getName())
                           .arg(m_assistants.at(i)->getSurname());
        if (!names.contains(name)) {
            names.append(name);
        }
    }

    for (int i = 0; i < m_teachers.size(); i++) {
        QString name
            = QString("%1 %2").arg(m_teachers.at(i)->getName()).arg(m_teachers.at(i)->getSurname());
        if (!names.contains(name)) {
            names.append(name);
        }
    }

    std::sort(names.begin(), names.end(), [](const QString &p1, const QString &p2) {
        return p1.compare(p2);
    });

    m_pAssistantSelector = new QtMaterialSelectBox(this);
    m_pAssistantSelector->setInkColor(QColor(160, 198, 232));
    m_pAssistantSelector->setDataSource(names);
    m_pAssistantSelector->setLabel("Assistant");
    m_pAssistantSelector->setPlaceholderText("Select assistant");
    m_pAssistantSelector->setMinimumWidth(200);
    m_pAssistantSelector->setMenuMaxHeight(200);
    addHeaderField(vertLayout, "Consultant", m_pAssistantSelector, 200);

    m_pSubjectSelector = new QtMaterialSelectBox(this);
    m_pSubjectSelector->setInkColor(QColor(160, 198, 232));
    m_pSubjectSelector->setDataSource(getAvailableSubjects());
    m_pSubjectSelector->setLabel("Subject");
    m_pSubjectSelector->setPlaceholderText("Select subject");
    m_pSubjectSelector->setMinimumWidth(200);
    m_pSubjectSelector->setMenuMaxHeight(200);
    addHeaderField(vertLayout, "Subjects", m_pSubjectSelector, 200);

    m_pTextEdit = new QTextEdit(this);
    m_pTextEdit->setMinimumHeight(100);
    QFont fontSmall("Roboto", 10, QFont::Normal);
    m_pTextEdit->setFont(fontSmall);
    addHeaderField(vertLayout, "Question", m_pTextEdit, 200);

    connect(m_pAssistantSelector, &QtMaterialSelectBox::itemSelected, [=](QString text) {
        if (!text.isEmpty() && !m_pTextEdit->toPlainText().isEmpty()) {
            requestButton->setDisabled(false);
        } else {
            requestButton->setDisabled(true);
        }
    });

    connect(m_pTextEdit, &QTextEdit::textChanged, [=]() {
        if (!m_pTextEdit->toPlainText().isEmpty() && !m_pAssistantSelector->text().isEmpty()) {
            requestButton->setDisabled(false);
        } else {
            requestButton->setDisabled(true);
        }
    });

    vertLayout->addSpacing(30);

    requestButton->setText("Request");
    requestButton->setMinimumWidth(100);
    requestButton->setMaximumWidth(150);
    requestButton->setHaloVisible(true);
    requestButton->setRole(Material::Role::Primary);
    requestButton->useThemeColors();
    requestButton->setOverlayStyle(Material::OverlayStyle::TintedOverlay);
    requestButton->setRippleStyle(Material::RippleStyle::PositionedRipple);
    requestButton->setDisabled(true);
    requestButton->setBackgroundColor(QColor(160, 198, 232));

    horzLayout->addWidget(requestButton, 1, Qt::AlignRight | Qt::AlignVCenter);

    cancelButton->setText("Cancel");
    cancelButton->setMinimumWidth(100);
    cancelButton->setMaximumWidth(150);
    cancelButton->setHaloVisible(true);
    cancelButton->setRole(Material::Role::Default);
    cancelButton->useThemeColors();
    cancelButton->setOverlayStyle(Material::OverlayStyle::TintedOverlay);
    cancelButton->setRippleStyle(Material::RippleStyle::PositionedRipple);
    cancelButton->setBackgroundColor(QColor(204, 204, 204));

    horzLayout->addWidget(cancelButton, 1, Qt::AlignLeft | Qt::AlignVCenter);
    vertLayout->addLayout(horzLayout);

    helpButton->setIconSize(QSize(16, 16));
    helpButton->setOpacity(0.5);
    vertLayout->addWidget(helpButton, 1, Qt::AlignRight | Qt::AlignBottom);

    setWindowLayout(vertLayout);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(clickCancel()));
    connect(requestButton, SIGNAL(clicked()), this, SLOT(clickRequest()));
    connect(helpButton, SIGNAL(clicked()), this, SLOT(clickHelp()));
}

QStringList AskConsultationDialog::getAvailableSubjects()
{
    QStringList subjects;

    for (int i = 0; i < m_teachers.size(); i++) {
        if (m_teachers.at(i)->getLecturer()) {
            if (!subjects.contains(m_teachers.at(i)->getSubject())) {
                subjects.append(m_teachers.at(i)->getSubject());
            }
        }
    }

    for (int i = 0; i < m_assistants.size(); i++) {
        if (!subjects.contains(m_assistants.at(i)->getSubject())) {
            subjects.append(m_assistants.at(i)->getSubject());
        }
    }

    return subjects;
}
