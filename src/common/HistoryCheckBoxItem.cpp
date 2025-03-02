#include "HistoryCheckBoxItem.h"
#include <QMouseEvent>
#include <QStyleOptionButton>


HistoryCheckBoxItem::HistoryCheckBoxItem(QString text, QWidget* parent)
	: QCheckBox(text, parent)
{

}

void HistoryCheckBoxItem::mousePressEvent(QMouseEvent* event) {
	QStyleOptionButton option;
	initStyleOption(&option);
	QRect indicatorRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option, this);

	if (indicatorRect.contains(event->pos())) {
		QCheckBox::mousePressEvent(event);
		emit indicatorClicked(!isChecked());
	}
	else {
		emit labelClicked();
	}
}

