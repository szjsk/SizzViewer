#include "TextToolBar.h"
#include "TextSettingDialog.h"
#include "TextSearchDialog.h"
#include "TextSettingProps.h"
#include "../common/StatusStore.h"
#include <QAction>
#include <QComboBox>
#include <QPushButton>


TextToolBar::TextToolBar(QWidget* parent, TextViewContainer* textViewContainer)
	: QToolBar(parent)
{
	setMovable(true);

	QAction* move2Action = new QAction(QIcon(":/icon/resources/icon/skip_previous_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "<<", this);
	move2Action->setToolTip("prev file (page up)");
	connect(move2Action, &QAction::triggered, this, [this, textViewContainer]() {
		textViewContainer->initTextFile(textViewContainer->getFileInfo()->fileName, FileUtils::MoveMode::Prev);
		});
	this->addAction(move2Action);

	QAction* move3Action = new QAction(QIcon(":/icon/resources/icon/chevron_left_24dp_1F1F1F.svg"), "<", this);
	move3Action->setToolTip("prev page (<-)");
	connect(move3Action, &QAction::triggered, this, [this, textViewContainer]() {
		textViewContainer->prevPage(textViewContainer->getFileInfo());
		});
	this->addAction(move3Action);

	QAction* move4Action = new QAction(QIcon(":/icon/resources/icon/chevron_right_24dp_1F1F1F.svg"), ">", this);
	move4Action->setToolTip("next page (->)");
	connect(move4Action, &QAction::triggered, this, [this, textViewContainer]() {
		textViewContainer->nextPage(textViewContainer->getFileInfo());
		});
	this->addAction(move4Action);

	QAction* move5Action = new QAction(QIcon(":/icon/resources/icon/skip_next_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), ">>", this);
	move5Action->setToolTip("next file(page down)");
	connect(move5Action, &QAction::triggered, this, [this, textViewContainer]() {
		textViewContainer->initTextFile(textViewContainer->getFileInfo()->fileName, FileUtils::MoveMode::Next);

		});
	this->addAction(move5Action);

	//text search
	QAction* searchAction = new QAction(QIcon(":/icon/resources/icon/find_in_page_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "Search", this);
	connect(searchAction, &QAction::triggered, this, [this, textViewContainer]() {
		openSearchDialog(textViewContainer);
		});
	this->addAction(searchAction);

	//container split
	QAction* splitAction = new QAction(QIcon(":/icon/resources/icon/auto_stories_24dp_1F1F1F.svg"), "Split", this);
	this->addAction(splitAction);
	connect(splitAction, &QAction::triggered, this, [this, textViewContainer]() {
		textViewContainer->changeSplitView();
		textViewContainer->refreshPage(textViewContainer->getFileInfo()->currentPosition);
		});

	//delete file
	QAction* deleteAction = new QAction(QIcon(":/icon/resources/icon/delete_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "delete", this);
	connect(deleteAction, &QAction::triggered, this, [this, textViewContainer]() {
		textViewContainer->deleteFile(textViewContainer->getFileInfo());
		});
	this->addAction(deleteAction);

	//text setting
	QAction* settingAction = new QAction(QIcon(":/icon/resources/icon/settings_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "Setting", this);
	connect(settingAction, &QAction::triggered, this, [this, textViewContainer]() {
		openSettingDialog(textViewContainer);
		});
	this->addAction(settingAction);

}

void TextToolBar::openSearchDialog(TextViewContainer* textViewContainer)
{
	TextSearchDialog dialog(this, textViewContainer);
	connect(&dialog, &TextSearchDialog::rowSelected, this, [this, textViewContainer](QString searchText, long page, long line) {
		textViewContainer->findPage(searchText, page, line);
		});
	dialog.exec();
}

void TextToolBar::openSettingDialog(TextViewContainer* textViewContainer)
{
	TextSettingDialog dialog(StatusStore::instance().getTextSettings(), this);
	connect(&dialog, &TextSettingDialog::settingsChanged, this, [this, textViewContainer](TextSettingProps settings) {
		textViewContainer->changeStyle(settings);
		});

	if (dialog.exec() == QDialog::Accepted) {
		StatusStore::instance().saveTextSettings(dialog.getTextSettings());
		textViewContainer->refreshPage(textViewContainer->getFileInfo()->currentPosition);
	}
	else {
		textViewContainer->changeStyle(StatusStore::instance().getTextSettings());
		textViewContainer->refreshPage(textViewContainer->getFileInfo()->currentPosition);
	}

}