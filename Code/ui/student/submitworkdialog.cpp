#include "submitworkdialog.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include "../mainappwindow.h"
#include "imageutils.h"
#include "mainappwindow.h"
#include "qfiledialog.h"
#include "qtmaterialselectbox.h"
#include "repositorymanager.h"
#include <poppler/qt6/poppler-qt6.h>

SubmitWorkDialog::SubmitWorkDialog(std::shared_ptr<Student> student,
                                   const Assignment *assignment,
                                   QWidget *parent)
    : QtMaterialDialog(parent)
    , vertLayout(new QVBoxLayout(this))
    , horzLayout(new QHBoxLayout(this))
    , submitButton(new QtMaterialRaisedButton(this))
    , cancelButton(new QtMaterialRaisedButton(this))
    , helpButton(
          new QtMaterialIconButton(QIcon(":/icons/icons/action/svg/production/ic_help_24px.svg"),
                                   this))
    , m_student(student)
    , m_pAssignment(assignment)
    , m_bIsPdf(false)
{}

void SubmitWorkDialog::addHeaderField(QVBoxLayout *headerLayout,
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

void SubmitWorkDialog::clickSubmit()
{
    qDebug() << "submitwork request attempt ";
    Work *work = new Work;

    work->setAssign(m_currentFile);
    work->setIsPdf(m_bIsPdf);
    work->setAssignmentId(m_pAssignment->getId());
    work->setComment(m_pTextEdit->toPlainText());
    work->setDateTime(QDateTime::currentDateTime());
    work->setGroup(m_student->getGroup());
    work->setIsChecked(false);
    work->setIsLate(false);
    work->setLogin(m_student->getLogin());
    work->setName(m_student->getName());
    work->setSubGroup(m_student->getSubgroup());
    work->setSurName(m_student->getSurname());
    work->setId(0);

    m_work = std::shared_ptr<Work>(work);

    std::unique_ptr<IWorkRepository> workRepository = RepositoryManager::instance().workRepository();

    if (workRepository->addWork(*work)) {
        MainAppWindow::instance().postMessage("Work submitted");
        QList<std::shared_ptr<Work>> works = workRepository->getByStudentLogin(
            m_student->getLogin());
        // search max id
        std::shared_ptr<Work> submittedWork;
        for (int i = 0; i < works.size(); i++) {
            if (submittedWork == nullptr || submittedWork->getId() < works.at(i)->getId()) {
                submittedWork = works.at(i);
            }
        }

        emit doSubmit(submittedWork);
    } else {
        MainAppWindow::instance().postError("Request unsuccessfull");
    }
}

void SubmitWorkDialog::clickCancel()
{
    qDebug() << "cancel submitwork request ";
    emit doCancel();
}

void SubmitWorkDialog::clickHelp()
{
    qDebug() << "help submitwork";
    emit doHelp();
}

void SubmitWorkDialog::setupForm()
{
    vertLayout->addSpacing(30);

    QFont fontLarge("Roboto", 18, QFont::Normal);

    QLabel *label = new QLabel(this);
    label->setText("Submit work");
    label->setAlignment(Qt::AlignHCenter);
    label->setFont(fontLarge);
    vertLayout->addWidget(label);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(this);
    QVBoxLayout *controlsLayout = new QVBoxLayout(this);
    QVBoxLayout *imageLayout = new QVBoxLayout(this);
    hBoxLayout->addSpacing(10);
    hBoxLayout->addLayout(controlsLayout);
    hBoxLayout->addSpacing(10);
    hBoxLayout->addLayout(imageLayout);
    hBoxLayout->addSpacing(10);
    vertLayout->addLayout(hBoxLayout);

    m_pAssignmentNameField = new QtMaterialTextField(this);
    m_pAssignmentNameField->setInkColor(QColor(160, 198, 232));
    m_pAssignmentNameField->setLabel("Assignment name");
    m_pAssignmentNameField->setText(m_pAssignment->getName());
    m_pAssignmentNameField->setMinimumWidth(200);
    addHeaderField(controlsLayout, "Assignment name", m_pAssignmentNameField, 200);

    QtMaterialRaisedButton *selectFile = new QtMaterialRaisedButton(this);
    selectFile->setBackgroundColor(QColor(204, 204, 204));
    selectFile->setForegroundColor(QColor(89, 89, 89));
    selectFile->setText("Upload Image/PDF");
    addHeaderField(controlsLayout, " ", selectFile, 200);

    QtMaterialTextField *pFileNameField = new QtMaterialTextField(this);
    pFileNameField->setInkColor(QColor(160, 198, 232));
    pFileNameField->setMinimumWidth(200);
    addHeaderField(controlsLayout, " ", pFileNameField, 200);

    connect(selectFile, &QAbstractButton::clicked, [=]() {
        if (m_images.size() != 0) {
            m_images.clear();
        }

        QString fileName = QFileDialog::getOpenFileName(this,
                                                        "Open data file",
                                                        AppSettings::instance().getLastDirectory(),
                                                        "Images (*.png *.xpm *.jpg *.pdf)", nullptr, QFileDialog::Option::DontUseNativeDialog);
        if (!fileName.isEmpty()) {
            AppSettings::instance().setLastDirectory(fileName);
            QFileInfo info(fileName);

            m_bIsPdf = fileName.endsWith(".pdf");

            if (m_bIsPdf) {
                std::unique_ptr<Poppler::Document> doc = Poppler::Document::load(fileName);
                doc->setRenderHint(Poppler::Document::TextAntialiasing);
                for (int i = 0; i < doc->numPages(); i++) {
                    std::unique_ptr<Poppler::Page> pdfPage = doc->page(i);
                    if (pdfPage == nullptr) {
                        MainAppWindow::instance().postError("Failed get page from PDF");
                        pFileNameField->setText("");
                        return;
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

            } else {
                QImage image(fileName);

                if (!image.isNull()) {
                    m_images.push_back(image);
                    addImageToLayout(imageLayout, image);
                }
            }

            if (!m_images.isEmpty()) {
                pFileNameField->setText(info.fileName());
                m_currentFile = readFile(fileName);
            }
        }
    });

    connect(pFileNameField, &QLineEdit::textChanged, [=](const QString &text) {
        if (text.isEmpty() && m_pTextEdit->toPlainText().isEmpty()) {
            submitButton->setDisabled(true);
        } else {
            submitButton->setDisabled(false);
        }
    });

    m_pTextEdit = new QTextEdit(this);
    m_pTextEdit->setMinimumHeight(100);
    QFont fontSmall("Roboto", 10, QFont::Normal);
    m_pTextEdit->setFont(fontSmall);
    addHeaderField(controlsLayout, "Comment", m_pTextEdit, 200);

    connect(m_pTextEdit, &QTextEdit::textChanged, [=]() {
        if (pFileNameField->text().isEmpty() && m_pTextEdit->toPlainText().isEmpty()) {
            submitButton->setDisabled(true);
        } else {
            submitButton->setDisabled(false);
        }
    });

    controlsLayout->addSpacing(30);

    submitButton->setText("Submit");
    submitButton->setBackgroundColor(QColor(204, 204, 204));
    submitButton->setForegroundColor(QColor(89, 89, 89));
    submitButton->setMinimumWidth(100);
    submitButton->setMaximumWidth(150);
    submitButton->setHaloVisible(true);
    submitButton->setRole(Material::Role::Primary);
    submitButton->useThemeColors();
    submitButton->setOverlayStyle(Material::OverlayStyle::TintedOverlay);
    submitButton->setRippleStyle(Material::RippleStyle::PositionedRipple);
    submitButton->setDisabled(true);

    horzLayout->addWidget(submitButton, 1, Qt::AlignRight | Qt::AlignVCenter);

    cancelButton->setText("Cancel");
    cancelButton->setBackgroundColor(QColor(204, 204, 204));
    cancelButton->setForegroundColor(QColor(89, 89, 89));
    cancelButton->setMinimumWidth(100);
    cancelButton->setMaximumWidth(150);
    cancelButton->setHaloVisible(true);
    cancelButton->setRole(Material::Role::Default);
    cancelButton->useThemeColors();
    cancelButton->setOverlayStyle(Material::OverlayStyle::TintedOverlay);
    cancelButton->setRippleStyle(Material::RippleStyle::PositionedRipple);

    horzLayout->addWidget(cancelButton, 1, Qt::AlignLeft | Qt::AlignVCenter);
    vertLayout->addLayout(horzLayout);

    helpButton->setIconSize(QSize(16, 16));
    helpButton->setOpacity(0.5);
    vertLayout->addWidget(helpButton, 1, Qt::AlignRight | Qt::AlignBottom);

    setWindowLayout(vertLayout);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(clickCancel()));
    connect(submitButton, SIGNAL(clicked()), this, SLOT(clickSubmit()));
    connect(helpButton, SIGNAL(clicked()), this, SLOT(clickHelp()));
}

void SubmitWorkDialog::removeWidgets(QLayout *pLayout)
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

void SubmitWorkDialog::addImageToLayout(QVBoxLayout *imageLayout, const QImage &image)
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

QByteArray SubmitWorkDialog::readFile(const QString &filePath)
{
    QByteArray result;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        result = file.readAll();
        file.close();
    }

    return result;
}
