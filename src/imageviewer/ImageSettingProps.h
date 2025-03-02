#ifndef IMAGESETTINGPROPS_H
#define IMAGESETTINGPROPS_H

#include <QFont>
#include <QColor>
#include <QMargins>
#include <QJsonObject>
#include "../common/IJsonSerializable.h"
#include "ImageView.h"

class ImageSettingProps : public IJsonSerializable
{
public:
	ImageSettingProps();
	ImageSettingProps(bool splitView,
		ImageView::ScaleMode scaleMode,
		bool isAuoNext,
		ImageView::Align centerAlignedSplit);

	// getter, setter
	bool isSplitView() const;
	void setSplitView(bool value);

	ImageView::ScaleMode getScaleMode() const;
	void setScaleMode(ImageView::ScaleMode value);

	bool isAutoNext() const;
	void setAutoNext(bool value);

	ImageView::Align getAlign() const;
	void setAlign(ImageView::Align value);

	// JSON 직렬화/역직렬화 메서드
	QJsonObject toJson() const override;
	void fromJson(const QJsonObject& json) override;
	QString getFileName() override;

private:
	bool m_splitView;
	ImageView::ScaleMode m_scaleMode;
	bool m_autoNext;
	ImageView::Align m_align;

};

#endif // IMAGESETTINGPROPS_H
