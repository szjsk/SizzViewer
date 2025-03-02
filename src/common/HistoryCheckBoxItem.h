#ifndef HISTORYCHECKBOXITEM_H
#define HISTORYCHECKBOXITEM_H

#include <QStringList>
#include <QCheckBox>

class HistoryCheckBoxItem : public QCheckBox 
{
	Q_OBJECT

public:
	HistoryCheckBoxItem(QString text, QWidget* parent = nullptr);

signals:
	void indicatorClicked(bool checked);
	void labelClicked();


protected:
	void mousePressEvent(QMouseEvent* event) override;
};

#endif 