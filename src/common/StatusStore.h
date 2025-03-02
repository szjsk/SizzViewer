#ifndef STATUSSTORE_H
#define STATUSSTORE_H

#include <QFont>
#include <QColor>
#include <QMargins>
#include "../textviewer/TextSettingProps.h"
#include "../imageviewer/ImageSettingProps.h"
#include "HistoryProps.h"
#include "IJsonSerializable.h"
#include <QFileInfo>

class StatusStore
{
public:
    static StatusStore& instance() {
        static StatusStore instance;
        return instance;
    }

    TextSettingProps& getTextSettings();
    void saveTextSettings(TextSettingProps settings);

    ImageSettingProps& getImageSettings();
    void saveImageSettings(ImageSettingProps newSettings);

    HistoryProps& getTextHistory();
    void saveTextHistory(HistoryProps newSettings);

    HistoryProps& getImageHistory();
    void saveImageHistory(HistoryProps settings);

    void savedFixedState(HistoryProps::SavedType type, QString fileName, bool isBookmarked);

private:
    StatusStore();
    ~StatusStore();
    StatusStore(const StatusStore&) = delete;
    StatusStore& operator=(const StatusStore&) = delete;
    IJsonSerializable* loadSetting(IJsonSerializable* fileName);
    void saveSetting(IJsonSerializable* jsonObj);
};

#endif // StatusStore_H
