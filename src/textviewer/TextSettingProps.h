#ifndef TEXTSETTINGPROPS_H
#define TEXTSETTINGPROPS_H

#include <QFont>
#include <QColor>
#include <QMargins>
#include <QJsonObject>
#include "../common/IJsonSerializable.h"

class TextSettingProps : public IJsonSerializable
{
public:
    TextSettingProps();
    TextSettingProps(bool splitView,
                 bool textView,
                 const QFont &font,
                 const QColor &textColor,
                 const QColor &backgroundColor,
                 const QMargins &padding,
                 double lineSpacing,
                 bool isAuoNext);

    // getter, setter
    bool isSplitView() const;
    void setSplitView(bool value);

    bool isTextView() const;
    void setTextView(bool value);

    QFont getFont() const;
    void setFont(const QFont &font);

    QColor getTextColor() const;
    void setTextColor(const QColor &color);

    QColor getBackgroundColor() const;
    void setBackgroundColor(const QColor &color);

    QMargins getPadding() const;
    void setPadding(const QMargins &margins);

    double getLineSpacing() const;
    void setLineSpacing(double spacing);

    // JSON 직렬화/역직렬화 메서드
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;
	QString getFileName() override;

    bool isAutoNext() const;
    void setAutoNext(bool value);

	//variable
private:
    bool m_splitView;
    bool m_textView;
    QFont m_font;
    QColor m_textColor;
    QColor m_backgroundColor;
    QMargins m_padding;
    double m_lineSpacing;
    bool m_autoNext = true;

};

#endif // TEXTSETTINGS_H
