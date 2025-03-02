#ifndef HISTORYBOOKMARKPROPS_H
#define HISTORYBOOKMARKPROPS_H

#include <QFont>
#include <QColor>
#include <QMargins>
#include <QJsonObject>
#include <QJsonArray>
#include "IJsonSerializable.h"

struct SavedFileInfo {
    QString fileName;
    long long textPosition;
    QString textLine;
	bool isBookmarked;
    long long savedDateTime;

    QJsonObject toJson() const {
        QJsonObject json;
        json["fileName"] = fileName;
        json["textPosition"] = textPosition;
        json["textLine"] = textLine;
        json["isBookmarked"] = isBookmarked;
        json["savedDateTime"] = savedDateTime;

        return json;
    }

    void fromJson(const QJsonObject& json) {
        fileName = json["fileName"].toString();
        textPosition = json["textPosition"].toVariant().toLongLong();
        textLine = json["textLine"].toString();
        isBookmarked = json["isBookmarked"].toBool();
        savedDateTime = json["savedDateTime"].toVariant().toLongLong();
    }
};

class HistoryProps : public IJsonSerializable
{
public:

    enum SavedType {
        TEXT_HISTORY,
        IMAGE_HISTORY,
    };
    HistoryProps(SavedType value);


    // getter, setter
    SavedType getSavedType() const;

    SavedFileInfo& getFileInfo(QString fileName);
    QHash<QString, SavedFileInfo> getFileInfos();
    void addFileInfo(QString filename, long textPosition, QString textLine);
    void setFixedState(SavedFileInfo fileInfo, bool isFixed);
    void setFileInfo(QHash<QString, SavedFileInfo> m_fileInfos);
    void removeAllNoFixed();

    // JSON 직렬화/역직렬화 메서드
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;
	QString getFileName() override;

private:
    SavedType m_savedType;
    QHash<QString, SavedFileInfo> m_fileInfos;
    QHash<QString, SavedFileInfo> caclulateHistorySize(QHash<QString, SavedFileInfo> allInfos);
};

#endif // HISTORYBOOKMARKPROPS_H
