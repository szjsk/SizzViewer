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

    struct FileInfo {
		QString fileName;
        QString fileNameWithPath;
        int currentPageIdx;
        long currentPosition;
        QStringList fileList;
		QString nextFile;
		QString prevFile;
        QString text;
        QHash<long, PageInfo> pageInfos;
    };

    TextViewContainer(QWidget* parent = nullptr);
    ~TextViewContainer();
    void initTextFile(QString filePath);
    void nextPage(const FileInfo* fileInfo);
    void prevPage(const FileInfo* fileInfo);
    bool changeSplitView();
    void findPage(const QString&, long page, long line);
	void deleteFile(const FileInfo* fileInfo);
	void clear();
    int findTextPageBy(const FileInfo* fileInfo, long position);
    const FileInfo* getFileInfo();
    void performSearch(QString searchText, const FileInfo* fileInfo);
    void refreshPage(long textPosition);
    void changeStyle(TextSettingProps s);


private:
    QHash<long, PageInfo> calculatePage(const FileInfo* fileInfo, int maxLine, int maxWidth, QTextBrowser* browser);
    QTextBrowser* createTextBrowser(TextSettingProps settings); //텍스트뷰 생성
    int getMaxHeight(QTextBrowser* tb);
    int getMaxWidth(QTextBrowser* tb);
    int getFontWidth(QFontMetrics* tb, QChar c);
    int setPage(FileInfo* fileInfo, int newPageIdx);
    void refreshStyle(TextSettingProps settings, QTextBrowser* tb);
    void applyLineSpacing(QTextBrowser* tb);
    void saveHistory(HistoryProps& history, const FileInfo* fileInfo);
    SavedFileInfo loadHistory(HistoryProps history, QString filePath);
    void testText();

protected:
    bool eventFilter(QObject* watched, QEvent* event);

signals:
    void deleteKeyPressed(QStringList files, QString nextFile);
	void searchResultReady(QString searchText, long page, int line);

    //variable
private:
    QHash<QChar, int> m_charWidthCache;
    static constexpr int M_TEXT_BROWSER_CNT = 2;
    QTextBrowser* ui_TextBrowsers[M_TEXT_BROWSER_CNT];
    QLabel* ui_QSliderInfo;
    QSlider* ui_QSlider;
    TextViewContainer::FileInfo m_fileInfo;
};

#endif // TEXTVIEWCONTAINER_H
