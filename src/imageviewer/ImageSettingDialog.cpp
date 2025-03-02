#include "ImageSettingDialog.h"

ImageSettingDialog::ImageSettingDialog(ImageSettingProps settings, QWidget* parent)
    : QDialog(parent), m_settings(settings)
{
    setWindowTitle(tr("image settings"));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Image Align 콤보박스
    QHBoxLayout* alignLayout = new QHBoxLayout;
    QLabel* alignLabel = new QLabel(tr("align:"), this);
    ui_alignCombo = new QComboBox(this);
    ui_alignCombo->addItem(tr("Left"), ImageView::LEFT);
    ui_alignCombo->addItem(tr("Center"), ImageView::CENTER);
    ui_alignCombo->addItem(tr("Right"), ImageView::RIGHT);
    ui_alignCombo->addItem(tr("Center Spread"), ImageView::CENTER_SPREAD);

    // 현재 alignment 설정
    int index = ui_alignCombo->findData(settings.getAlign());
    if (index >= 0)
        ui_alignCombo->setCurrentIndex(index);

    alignLayout->addWidget(alignLabel);
    alignLayout->addWidget(ui_alignCombo);
    mainLayout->addLayout(alignLayout);

    // 자동 이동 체크박스
    QHBoxLayout* autoLayout = new QHBoxLayout;
    ui_autoNextCheck = new QCheckBox(tr("when last/first page move prev/next file."), this);
    ui_autoNextCheck->setChecked(m_settings.isAutoNext());
    autoLayout->addWidget(ui_autoNextCheck);
    mainLayout->addLayout(autoLayout);

    // OK / Cancel 버튼
    ui_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(ui_buttonBox);

    // 콘트롤 변경시 프리뷰 업데이트
    connect(ui_alignCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreview()));

    // OK/Cancel 버튼 연결
    connect(ui_buttonBox, &QDialogButtonBox::accepted, this, &ImageSettingDialog::dlgAccept);
    connect(ui_buttonBox, &QDialogButtonBox::rejected, this, &ImageSettingDialog::dlgReject);
}

ImageSettingDialog::~ImageSettingDialog()
{
}

ImageView::Align ImageSettingDialog::selectedAlign() const
{
    return static_cast<ImageView::Align>(ui_alignCombo->currentData().toInt());
}

bool ImageSettingDialog::isAutoNext() const
{
    return ui_autoNextCheck->isChecked();
}

ImageSettingProps ImageSettingDialog::getImageSettings()
{
    m_settings.setAlign(selectedAlign());
    m_settings.setAutoNext(isAutoNext());
    return m_settings;
}

void ImageSettingDialog::updatePreview()
{
    m_settings.setAlign(selectedAlign());
    m_settings.setAutoNext(isAutoNext());
    emit settingsChanged(m_settings);
}

void ImageSettingDialog::dlgAccept()
{
    accept();
}

void ImageSettingDialog::dlgReject()
{
    reject();
}