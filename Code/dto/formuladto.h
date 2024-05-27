#ifndef FORMULADTO_H
#define FORMULADTO_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include "VariableDto.h"
#include <dto.h>
#include <listdto.h>

class FormulaDto : public IDto
{
private:
    QString formula;
    QList<VariableDto> variables;

    void initFromJsonObject(const QJsonObject &object)
    {
        formula = object.value("formula").toString();
        for (auto v : object.value("variables").toArray()) {
            QJsonObject element = v.toObject();
            variables.append(VariableDto(element));
        }
    }

public:
    FormulaDto() {}

    FormulaDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }

    FormulaDto(const QJsonObject &object) { initFromJsonObject(object); }

public:
    void fromJson(const QByteArray &content) override
    {
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);

        if (jsonError.error != QJsonParseError::ParseError::NoError) {
            qDebug() << jsonError.errorString() << " error: " << jsonError.error;
            return;
        };

        QJsonObject obj = doc.object();
        initFromJsonObject(obj);
    }
    const QByteArray toJson() const override
    {
        QJsonObject obj;

        obj["formula"] = getFormula();
        QJsonArray arr;

        for (VariableDto v : variables) {
            QJsonObject obj;
            arr.append(QJsonDocument::fromJson(v.toJson()).object());
        }

        obj["variables"] = arr;

        QJsonDocument jsonDoc(obj);
        return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
    }

    QString getFormula() const { return formula; }
    void setFormula(const QString &newFormula) { formula = newFormula; }
    QList<VariableDto> getVariables() const { return variables; }
    void setVariables(const QList<VariableDto> &newVariables) { variables = newVariables; }
};

#endif // FORMULADTO_H
