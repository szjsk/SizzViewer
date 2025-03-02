#include "StatusStore.h"
#include <QFile>
#include <QJsonDocument>


TextSettingProps m_textSettingProps;
ImageSettingProps m_imageSettingProps;
HistoryProps m_textHistory(HistoryProps::SavedType::TEXT_HISTORY);
HistoryProps m_imageHistory(HistoryProps::SavedType::IMAGE_HISTORY);

StatusStore::StatusStore()
{
    loadSetting(&m_textSettingProps);
    loadSetting(&m_imageSettingProps);
    loadSetting(&m_textHistory);
    loadSetting(&m_imageHistory);

}

StatusStore::~StatusStore()
{
	saveSetting(&m_textSettingProps);
	saveSetting(&m_imageSettingProps);
	saveTextHistory(m_textHistory);
    saveImageHistory(m_imageHistory);
}

IJsonSerializable* StatusStore::loadSetting(IJsonSerializable* jsonObj)
{

    QString fileName = jsonObj->getFileName();
    QFileInfo fileInfo(fileName);
    QString absolutePath = fileInfo.absoluteFilePath();

    QFile loadFile(jsonObj->getFileName());
    if (!loadFile.open(QIODevice::ReadOnly)) {
        return jsonObj;
    }

    QByteArray data = loadFile.readAll();
    loadFile.close();

    QJsonDocument loadDoc = QJsonDocument::fromJson(data);
    if (!loadDoc.isNull() && loadDoc.isObject())
    {
        QJsonObject json = loadDoc.object();
        jsonObj->fromJson(json);
    }
    return jsonObj;
}

void StatusStore::saveSetting(IJsonSerializable* jsonObj)
{

    QFile saveFile(jsonObj->getFileName());

    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return;
    }

    QJsonObject json = jsonObj->toJson();
    QJsonDocument saveDoc(json);
    saveFile.write(saveDoc.toJson());

    saveFile.close();
}


TextSettingProps& StatusStore::getTextSettings() {
	return m_textSettingProps;
}

void StatusStore::saveTextSettings(TextSettingProps newSettings) {
	m_textSettingProps = newSettings;
    saveSetting(&newSettings);
}

ImageSettingProps& StatusStore::getImageSettings() {
	return m_imageSettingProps;
}

void StatusStore::saveImageSettings(ImageSettingProps newSettings) {
	m_imageSettingProps = newSettings;
    saveSetting(&newSettings);
}

HistoryProps& StatusStore::getTextHistory() {
	return m_textHistory;
}
void StatusStore::saveTextHistory(HistoryProps newSettings) {
	m_textHistory = newSettings;
    saveSetting(&newSettings);
}

void StatusStore::savedFixedState(HistoryProps::SavedType type, QString fileName, bool isBookmarked) {
	if (type == HistoryProps::TEXT_HISTORY) {
		m_textHistory.getFileInfo(fileName).isBookmarked = isBookmarked;
        //saveSetting(&m_textHistory);
	}
	else if (type == HistoryProps::IMAGE_HISTORY) {
		m_imageHistory.getFileInfo(fileName).isBookmarked = isBookmarked;
        //saveSetting(&m_imageHistory);
	}


}

HistoryProps& StatusStore::getImageHistory() {
	return m_imageHistory;
}
void StatusStore::saveImageHistory(HistoryProps newSettings) {
    m_imageHistory = newSettings;
    saveSetting(&newSettings);
}

/*
HistoryProps StatusStore::caclulateHistorySize(HistoryProps::SavedType type, QHash<QString, SavedFileInfo> allInfos) {

    QList<SavedFileInfo> infos = allInfos.values();

    std::sort(infos.begin(), infos.end(), [](const SavedFileInfo& a, const SavedFileInfo& b) {
        return a.savedDateTime < b.savedDateTime;
        });

    // 분리
    QList<SavedFileInfo> bookmarked;
    QList<SavedFileInfo> unbookmarked;
    for (const SavedFileInfo& info : infos) {
        if (info.isBookmarked) {
            bookmarked.append(info);
        }
        else {
            unbookmarked.append(info);
        }
    }

    HistoryProps newHistory(type);
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
        return a.savedDateTime < b.savedDateTime;
        });

    QHash<QString, SavedFileInfo> fileInfos;
	for (const SavedFileInfo& info : combined) {
		fileInfos.insert(info.fileName, info);
	}

	newHistory.setFileInfo(fileInfos);

    return newHistory;
}*/