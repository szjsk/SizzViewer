#ifndef TEXTVIEWCONTAINER_H
#define TEXTVIEWCONTAINER_H

#include <QWidget>
#include <QTextBrowser>
#include "TextSettingProps.h"
#include "../common/HistoryProps.h"
#include "../common/StatusStore.h"
#include "../common/FileUtils.h"
#include "../common/StatusStore.h"

#include <QHBoxLayout>
#include <QFile>
#include <QTextCursor>
#include <QTextDocument>
#include <QSizeF>
#include <QAbstractTextDocumentLayout>
#include <QTimer>
#include <QSlider>
#include <QLabel>
#include <QFileInfo>
#include <QDir>
#include <QCollator>
#include <QHash>

class TextViewContainer : public QWidget
{
    Q_OBJECT

public:

    struct PageInfo {
        long firstPosition = -1;
        QVector<QString> lines;
    };

    struct TextFileInfo {
        QString zipFileName;
		QString fileName;
        int currentPageIdx;
        long currentPosition;
        QString text;
        QHash<long, PageInfo> pageInfos;
    };

    TextViewContainer(QWidget* parent = nullptr);
    ~TextViewContainer();
    void initTextFile(QString filePath, FileUtils::MoveMode moveMode);
    void nextPage(TextFileInfo* fileInfo);
    void prevPage(TextFileInfo* fileInfo);
    bool changeSplitView();
    void findPage(const QString&, long page, long line);
	void deleteFile(const TextFileInfo* fileInfo);
	void clear();
    int findTextPageBy(const TextFileInfo* fileInfo, long position);
    TextFileInfo* getFileInfo();
    void performSearch(QString searchText, const TextFileInfo* fileInfo);
    void refreshPage(long textPosition);
    void changeStyle(TextSettingProps s);

private:
    QHash<long, PageInfo> calculatePage(const TextFileInfo* fileInfo, int maxLine, int maxWidth, QTextBrowser* browser);
    QTextBrowser* createTextBrowser(TextSettingProps settings); //텍스트뷰 생성
    int getMaxHeight(QTextBrowser* tb);
    int getMaxWidth(QTextBrowser* tb);
    int getFontWidth(QFontMetrics* tb, QChar c);
    int setPage(TextFileInfo* fileInfo, int newPageIdx);
    void refreshStyle(TextSettingProps settings, QTextBrowser* tb);
    void applyLineSpacing(QTextBrowser* tb);
    void saveHistory(HistoryProps& history, const TextFileInfo* fileInfo);
	bool isUtf8Text(QByteArray& data);

protected:
    bool eventFilter(QObject* watched, QEvent* event);

signals:
    void deleteKeyPressed(QStringList files, FileUtils::SupportType type);
	void searchResultReady(QString searchText, long page, int line);
    void renameFile(QString file);
    void appendFileControl(int keyEvent, QString fileName);

    //variable
private:
    QHash<QChar, int> m_charWidthCache;
    static constexpr int M_TEXT_BROWSER_CNT = 2;
    QTextBrowser* ui_TextBrowsers[M_TEXT_BROWSER_CNT];
    QLabel* ui_QSliderInfo;
    QSlider* ui_QSlider;
    TextViewContainer::TextFileInfo m_fileInfo;
};

#endif // TEXTVIEWCONTAINER_H
