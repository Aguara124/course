#ifndef LISTDTO_H
#define LISTDTO_H

#include <QJsonArray>
#include <QJsonDocument>
#include "dto.h"

template<typename T, typename std::enable_if<std::is_base_of<IDto, T>::value>::type * = nullptr>
class ListDto : public IDto
{
private:
    QList<T> itemList;

public:
    ListDto() {}
    ListDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }

    ListDto(const QList<T> &newList)
        : itemList(newList)
    {}

public:
    void fromJson(const QByteArray &content) override
    {
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);

        if (jsonError.error != QJsonParseError::ParseError::NoError) {
            qDebug() << jsonError.errorString() << " error: " << jsonError.error;
            return;
        };

        for (auto v : doc.array()) {
            QJsonObject element = v.toObject();
            itemList.append(T(element));
        }
    }
    const QByteArray toJson() const override
    {
        QJsonArray arr;

        for (T v : itemList) {
            QJsonObject obj;
            arr.append(QJsonDocument::fromJson(v.toJson()).object());
        }

        QJsonDocument jsonDoc(arr);
        return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
    }

    QList<T> getList() const { return itemList; }
    void setList(const QList<T> &newList) { itemList = QList<T>(newList); }
};

#endif // LISTDTO_H
