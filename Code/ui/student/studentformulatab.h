#ifndef StudentFormulaTab_H
#define StudentFormulaTab_H

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QTableWidget;
class QtMaterialCard;

class QtMaterialRaisedButton;
class QtMaterialTextField;
class QtMaterialSelectBox;

class QScrollArea;
class QButtonGroup;
class QAbstractButton;
class QTableWidgetItem;
class QTableWidget;
class FormulaDto;
class ProfileEditDialog;

#include "assistantrepository.h"
#include "fcsrepository.h"
#include "graderepository.h"
#include "studentrepository.h"
#include "teacherrepository.h"

class StudentFormulaTab : public QWidget
{
    Q_OBJECT
public:
    explicit StudentFormulaTab(const QString &studentLogin, QWidget *parent = nullptr);
    void setupTabLayout();

protected slots:
    void redrawGrades(const QList<std::shared_ptr<Grade>> &gradeList);
    void recalcGrades(const QList<std::shared_ptr<Grade>> &gradeList);
    void updateStudentCard(const QList<std::shared_ptr<Fcs>> &fcsList);
    void updateSubjects(const QList<std::shared_ptr<Fcs>> &fcsList);

private:
    QHBoxLayout *m_pGradeLayout;
    QtMaterialCard *m_pStudentCard;

    QtMaterialRaisedButton *m_pReloadButton;

    QScrollArea *m_pSubjectsArea;
    QVBoxLayout *m_pSubjectsLayout;
    QButtonGroup *m_pButtonGroup;

    QMap<QAbstractButton *, std::shared_ptr<Fcs>> m_buttonToMap;

    const QString m_studentLogin;
    std::shared_ptr<Student> m_student;
    QList<std::shared_ptr<Grade>> m_gradeList;
    QList<std::shared_ptr<Fcs>> m_fcsList;

    std::shared_ptr<QBrush> oddColor;
    std::shared_ptr<QBrush> evenColor;

    std::unique_ptr<IGradeRepository> gradeRepository;

    QTableWidget *m_tables[3];
    QtMaterialTextField *m_results[3];

    ProfileEditDialog *m_pEditProfile;

private:
    QTableWidgetItem *addTableItem(
        QTableWidget *table, int row, int column, const QString &text, bool isEditable = false);

    void setStudentFormulaTableRecord(int index, const Grade &grade);
    static bool matchBySearchField(const QString &searchText, const Grade &grade);
    QTableWidget *createFormulaTableWidget();
    QLayout *setupGradeTableLayout();
    static std::shared_ptr<Grade> getGradeById(int id,
                                               const QList<std::shared_ptr<Grade>> &gradeList);

    template<typename T>
    inline void truncate(QList<T> &list, int length)
    {
        if (0 <= length && length < list.length()) {
            list.erase(list.begin() + length, list.end());
        }
    }

    template<typename T>
    inline QList<T> reversed(const QList<T> &in)
    {
        QList<T> result;
        result.reserve(in.size()); // reserve is new in Qt 4.7
        std::reverse_copy(in.begin(), in.end(), std::back_inserter(result));
        return result;
    }

    void addHeaderField(QVBoxLayout *headerLayout,
                        const QString &labelText,
                        QWidget *fieldContent,
                        int labelWidth);
    QVBoxLayout *addStage(int stage);
    void fillTableData(const QString &subject,
                       const FormulaDto &formula,
                       QTableWidget *table,
                       QtMaterialTextField *result,
                       QList<double> &rawResult);
    void calcTableData(const QString &subject,
                       const FormulaDto &formula,
                       QTableWidget *table,
                       QtMaterialTextField *result,
                       QList<double> &rawResult);
    double calculatePeriod(const QString &subject,
                           const QString &period,
                           const QString &controlType,
                           const QList<double> &rawResult);
};

#endif // StudentFormulaTab_H
