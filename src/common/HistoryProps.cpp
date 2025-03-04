#include "HistoryProps.h"
#include <QJsonObject>



HistoryProps::HistoryProps(SavedType value)
    : m_savedType(value)
{
}

HistoryProps::SavedType HistoryProps::getSavedType() const { return m_savedType; }

void HistoryProps::setFileInfo(QHash<QString, SavedFileInfo> fileInfos) {
    m_fileInfos = fileInfos;
}

SavedFileInfo& HistoryProps::getFileInfo(QString fileName) {
    return m_fileInfos[fileName];
}

QHash<QString, SavedFileInfo> HistoryProps::getFileInfos() {
    return m_fileInfos;
}

void HistoryProps::setFixedState(SavedFileInfo fileInfo, bool isFixed) {
	fileInfo.isBookmarked = isFixed;
    m_fileInfos.insert(fileInfo.fileName, fileInfo);
}

void HistoryProps::addFileInfo(QString filename, long textPosition, QString textLine) {

    if (filename.isEmpty()) {
        return;
    }

	SavedFileInfo fileInfo;
    fileInfo.fileName = filename;
    fileInfo.textPosition = textPosition;
    fileInfo.textLine = textLine;
    fileInfo.isBookmarked = m_fileInfos.contains(filename) && m_fileInfos.value(filename).isBookmarked ? true : false;
    fileInfo.savedDateTime = QDateTime::currentMSecsSinceEpoch();

    m_fileInfos.insert(fileInfo.fileName, fileInfo);
    m_fileInfos = caclulateHistorySize(m_fileInfos);

}

void HistoryProps::removeAllNoFixed() {

    QHash<QString, SavedFileInfo> newfileInfos;
    for (const SavedFileInfo& info : m_fileInfos) {
		if (info.isBookmarked) {
			newfileInfos.insert(info.fileName, info);
		}
    }
    m_fileInfos = newfileInfos;
}


QHash<QString, SavedFileInfo> HistoryProps::caclulateHistorySize(QHash<QString, SavedFileInfo> allInfos) {

    QList<SavedFileInfo> infos = allInfos.values();

    std::sort(infos.begin(), infos.end(), [](const SavedFileInfo& a, const SavedFileInfo& b) {
        return a.savedDateTime > b.savedDateTime;
        });

    // 분리
    QList<SavedFileInfo> bookmarked;
    QList<SavedFileInfo> unbookmarked;
    for (const SavedFileInfo& info : infos) {
		if (info.fileName.isEmpty()) {
			continue;
		}
        if (info.isBookmarked) {
            bookmarked.append(info);
        }
        else {
            unbookmarked.append(info);
        }
    }

    QList<SavedFileInfo> selectedUnbookmarked;
    if (bookmarked.size() > 15) {
        SavedFileInfo first = unbookmarked.first();
        unbookmarked.clear();
        unbookmarked.append(first);
    }
    else {
        int bookmarkSize = 15 - bookmarked.size();
        selectedUnbookmarked = unbookmarked.mid(0, bookmarkSize);
    }

    QList<SavedFileInfo> combined = bookmarked + selectedUnbookmarked;

    std::sort(combined.begin(), combined.end(), [](const SavedFileInfo& a, const SavedFileInfo& b) {
        return a.savedDateTime > b.savedDateTime;
        });


    QHash<QString, SavedFileInfo> fileInfos;
    for (const SavedFileInfo& info : combined) {
        fileInfos.insert(info.fileName, info);
    }

    return fileInfos;
}

QJsonObject HistoryProps::toJson() const {
    QJsonObject json;
    json["savedType"] = m_savedType;

    QJsonArray fileInfosArray;
    for (const auto& fileInfo : m_fileInfos) {
        fileInfosArray.append(fileInfo.toJson());
    }
    json["fileInfos"] = fileInfosArray;
    return json;
}

void HistoryProps::fromJson(const QJsonObject &json) {
    m_savedType = static_cast<SavedType>(json["savedType"].toInt());

    m_fileInfos.clear();
    QJsonArray fileInfosArray = json["fileInfos"].toArray();
    for (const auto& fileInfoValue : fileInfosArray) {
        SavedFileInfo fileInfo;
        fileInfo.fromJson(fileInfoValue.toObject());
        m_fileInfos.insert(fileInfo.fileName, fileInfo);
    }

}
QString HistoryProps::getFileName() {
    switch (m_savedType) {
    case HistoryProps::TEXT_HISTORY:
        return "text_history.json";
    case HistoryProps::IMAGE_HISTORY:
        return "image_history.json";
    default:
        return "unknown.json";
    }
}