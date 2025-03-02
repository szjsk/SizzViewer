#include "TextSettingProps.h"
#include <QJsonObject>

TextSettingProps::TextSettingProps()
	: m_splitView(false),
	m_textView(true),
	m_font(QFont("맑은 고딕", 16, QFont::Bold)),
	m_textColor(Qt::black),
	m_backgroundColor(QColor("#e8e4d2")), //#e8e4d2
	m_padding(10, 10, 10, 10),
	m_lineSpacing(1.0),
	m_autoNext(true)
{
}

TextSettingProps::TextSettingProps(bool splitView,
	bool textView,
	const QFont& font,
	const QColor& textColor,
	const QColor& backgroundColor,
	const QMargins& padding,
	double lineSpacing,
	bool isAutoNext)
	: m_splitView(splitView),
	m_textView(textView),
	m_font(font),
	m_textColor(textColor),
	m_backgroundColor(backgroundColor),
	m_padding(padding),
	m_lineSpacing(lineSpacing),
	m_autoNext(isAutoNext)
{
}

bool TextSettingProps::isSplitView() const { return m_splitView; }
void TextSettingProps::setSplitView(bool value) { m_splitView = value; }

bool TextSettingProps::isTextView() const { return m_textView; }
void TextSettingProps::setTextView(bool value) { m_textView = value; }

QFont TextSettingProps::getFont() const { return m_font; }
void TextSettingProps::setFont(const QFont& font) { this->m_font = font; }

QColor TextSettingProps::getTextColor() const { return m_textColor; }
void TextSettingProps::setTextColor(const QColor& color) { m_textColor = color; }

QColor TextSettingProps::getBackgroundColor() const { return m_backgroundColor; }
void TextSettingProps::setBackgroundColor(const QColor& color) { m_backgroundColor = color; }

QMargins TextSettingProps::getPadding() const { return m_padding; }
void TextSettingProps::setPadding(const QMargins& margins) { m_padding = margins; }

double TextSettingProps::getLineSpacing() const { return m_lineSpacing; }
void TextSettingProps::setLineSpacing(double spacing) { m_lineSpacing = spacing; }

bool TextSettingProps::isAutoNext() const { return m_autoNext; }
void TextSettingProps::setAutoNext(bool value) { m_autoNext = value; }

QJsonObject TextSettingProps::toJson() const {
	QJsonObject json;
	json["splitView"] = m_splitView;
	json["textView"] = m_textView;

	// QFont를 string으로 변환 (예: QFont::toString 활용)
	json["font"] = m_font.toString();

	json["textColor"] = m_textColor.name();
	json["backgroundColor"] = m_backgroundColor.name();

	json["paddingLeft"] = m_padding.left();
	json["paddingTop"] = m_padding.top();
	json["paddingRight"] = m_padding.right();
	json["paddingBottom"] = m_padding.bottom();

	json["lineSpacing"] = m_lineSpacing;
	json["autoNext"] = m_autoNext;
	return json;
}

void TextSettingProps::fromJson(const QJsonObject& json) {
	if (json.contains("splitView"))
		m_splitView = json["splitView"].toBool();
	if (json.contains("textView"))
		m_textView = json["textView"].toBool();

	if (json.contains("font"))
		m_font.fromString(json["font"].toString());

	if (json.contains("textColor"))
		m_textColor = QColor(json["textColor"].toString());
	if (json.contains("backgroundColor"))
		m_backgroundColor = QColor(json["backgroundColor"].toString());

	int left = json.contains("paddingLeft") ? json["paddingLeft"].toInt() : 10;
	int top = json.contains("paddingTop") ? json["paddingTop"].toInt() : 10;
	int right = json.contains("paddingRight") ? json["paddingRight"].toInt() : 10;
	int bottom = json.contains("paddingBottom") ? json["paddingBottom"].toInt() : 10;
	m_padding = QMargins(left, top, right, bottom);

	if (json.contains("lineSpacing"))
		m_lineSpacing = json["lineSpacing"].toDouble();

	if (json.contains("autoNext"))
		m_autoNext = json["autoNext"].toBool();
}
QString TextSettingProps::getFileName() {
	return "TextSettingProps.json";
}