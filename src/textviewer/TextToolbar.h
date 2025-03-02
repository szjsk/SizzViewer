#ifndef TEXTTOOLBAR_H
#define TEXTTOOLBAR_H

#include <QToolBar>
#include <QWidget>
#include "TextViewContainer.h"

class TextToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit TextToolBar(QWidget* parent = nullptr, TextViewContainer* textViewContainer = nullptr);

private:

private slots:
	void openSettingDialog(TextViewContainer* textViewContainer);
	void openSearchDialog(TextViewContainer* textViewContainer);
};

#endif // TEXTTOOLBAR_H