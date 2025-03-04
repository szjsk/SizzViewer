#include "ImageToolBar.h"

ImageToolBar::ImageToolBar(QWidget* parent, ImageViewContainer* container)
	: QToolBar(parent)
{
    setMovable(true);


    QAction* move1Action = new QAction(QIcon(":/icon/resources/icon/fast_rewind_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "<<<", this);
    move1Action->setToolTip("prev folder (PAGE UP)");
    connect(move1Action, &QAction::triggered, this, [this, container]() {move(container, FileUtils::MoveMode::PrevFolder);});
    this->addAction(move1Action);

    QAction* move2Action = new QAction(QIcon(":/icon/resources/icon/skip_previous_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "<<", this);
    move2Action->setToolTip("first file");
    connect(move2Action, &QAction::triggered, this, [this, container]() {move(container, FileUtils::MoveMode::First);});
    this->addAction(move2Action);

    QAction* move3Action = new QAction(QIcon(":/icon/resources/icon/chevron_left_24dp_1F1F1F.svg"), "<", this);
    move3Action->setToolTip("prev file (<-)");
    connect(move3Action, &QAction::triggered, this, [this, container]() {move(container, FileUtils::MoveMode::Prev);});
    this->addAction(move3Action);

    QAction* move4Action = new QAction(QIcon(":/icon/resources/icon/chevron_right_24dp_1F1F1F.svg"), ">", this);
    move4Action->setToolTip("next file (->)");
    connect(move4Action, &QAction::triggered, this, [this, container]() {move(container, FileUtils::MoveMode::Next);});
    this->addAction(move4Action);

    QAction* move5Action = new QAction(QIcon(":/icon/resources/icon/skip_next_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), ">>", this);
    move5Action->setToolTip("last file");
    connect(move5Action, &QAction::triggered, this, [this, container]() {move(container, FileUtils::MoveMode::Last);});
    this->addAction(move5Action);

    QAction* move6Action = new QAction(QIcon(":/icon/resources/icon/fast_forward_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), ">>>", this);
    move6Action->setToolTip("next folder (PAGE DOWN)");
    connect(move6Action, &QAction::triggered, this, [this, container]() {move(container, FileUtils::MoveMode::NextFolder);});
    this->addAction(move6Action);

    QComboBox* scaleModeAction = new QComboBox(this);
    scaleModeAction->addItems({ "size", "FitWindow(1)", "ORIGINAL(2)", "FitByWidth(3)" , "FitByHeight(4)", "FitIfLagrge(5)" });
    connect(scaleModeAction, QOverload<int>::of(&QComboBox::activated), this, [this, scaleModeAction, container](int index) {
        scaleModeAction->setCurrentIndex(0);
        ImageView::ScaleMode mode;
        switch (index) {
        case 0:
            return;
        case 1:
            mode = ImageView::FitToWindow;
            break;
        case 2:
            mode = ImageView::ORIGINAL;
            break;
        case 3:
            mode = ImageView::FitToWidth;
            break;
        case 4:
            mode = ImageView::FitToHeight;
            break;
        case 5:
            mode = ImageView::FitIfLARGE;
            break;
        default:
            mode = ImageView::FitIfLARGE;
            break;
        }
        container->resizeImage(mode);
        });
    this->addWidget(scaleModeAction);



    QAction* plusAction = new QAction(QIcon(":/icon/resources/icon/zoom_in_24dp_1F1F1F.svg"), "+", this);
    plusAction->setToolTip("Zoom in 10% of original size 10% (+)");
    connect(plusAction, &QAction::triggered, this, [this, container]() {container->resizeImage(ImageView::ScaleByPercentage, true);});
    this->addAction(plusAction);

    QAction* minusAction = new QAction(QIcon(":/icon/resources/icon/zoom_out_24dp_1F1F1F.svg"), "-", this);
    minusAction->setToolTip("Zoom out 10% of original size 10% (+)");
    connect(minusAction, &QAction::triggered, this, [this, container]() {container->resizeImage(ImageView::ScaleByPercentage, false);});
    this->addAction(minusAction);


    QAction* rotationVAction = new QAction(QIcon(":/icon/resources/icon/screen_rotation_alt_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "rotation 180", this);
    rotationVAction->setToolTip("rotation 180 degree");
    connect(rotationVAction, &QAction::triggered, this, [this, container]() {container->flip();});
    this->addAction(rotationVAction);

    QAction* roationRAction = new QAction(QIcon(":/icon/resources/icon/rotate_right_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "rotation r 90", this);
    roationRAction->setToolTip("rotation 90 degree right");
    connect(roationRAction, &QAction::triggered, this, [this, container]() {container->rotate(45);});
    this->addAction(roationRAction);

    QAction* roationLAction = new QAction(QIcon(":/icon/resources/icon/rotate_left_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "rotation l 90 ", this);
    roationLAction->setToolTip("rotation 90 degree left");
    connect(roationLAction, &QAction::triggered, this, [this, container]() {container->rotate(-45);});
    this->addAction(roationLAction);

    QAction* fullScreenAction = new QAction(QIcon(":/icon/resources/icon/zoom_out_map_24dp_1F1F1F.svg"), "full-screan", this);
    fullScreenAction->setToolTip("Full Screen (f)");
    connect(fullScreenAction, &QAction::triggered, this, [this, container]() {container->toggleFullScreen();});
    this->addAction(fullScreenAction);

    //for split
    QAction* splitAction = new QAction(QIcon(":/icon/resources/icon/auto_stories_24dp_1F1F1F.svg"), "Split", this);
    splitAction->setToolTip("split page");
    this->addAction(splitAction);

    // 클릭 시 동작 정의
    connect(splitAction, &QAction::triggered, this, [this, container]() {
        ImageSettingProps& setting = StatusStore::instance().getImageSettings();
        setting.setSplitView(container->changeSplitView());
        StatusStore::instance().saveImageSettings(setting);
        });

    QAction* deleteAction = new QAction(QIcon(":/icon/resources/icon/delete_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "delete", this);
    deleteAction->setToolTip("delete file or folder (del)");
    connect(deleteAction, &QAction::triggered, this, [this, container]() {container->deleteImageFile();});
    this->addAction(deleteAction);

    //text setting
    QAction* settingAction = new QAction(QIcon(":/icon/resources/icon/settings_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "Setting", this);
    connect(settingAction, &QAction::triggered, this, [this, container]() {
        openSettingDialog(container);
        });
    this->addAction(settingAction);

}

void ImageToolBar::move(ImageViewContainer* container, FileUtils::MoveMode mode) {
    if (mode == FileUtils::MoveMode::NextFolder || mode == FileUtils::MoveMode::PrevFolder) {
        container->navigate(mode);
    }
    else {
        container->navigate(mode);
    }
}


void ImageToolBar::openSettingDialog(ImageViewContainer* container)
{
    ImageSettingDialog dialog(StatusStore::instance().getImageSettings(), this);

    if (dialog.exec() == QDialog::Accepted) {
        StatusStore::instance().saveImageSettings(dialog.getImageSettings());
        container->applySettings();
    }

}