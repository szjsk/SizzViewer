#include "TextSettingDialog.h"

TextSettingDialog::TextSettingDialog(TextSettingProps settings, QWidget* parent)
    : QDialog(parent), m_settings(settings)
{

    setWindowTitle(tr("font"));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 검색 입력란
    QHBoxLayout* searchLayout = new QHBoxLayout;
    QLabel* searchLabel = new QLabel(tr("search:"), this);
    ui_searchEdit = new QLineEdit(this);
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(ui_searchEdit);
    mainLayout->addLayout(searchLayout);

    // 폰트 콤보박스
    QHBoxLayout* fontLayout = new QHBoxLayout;
    QLabel* fontLabel = new QLabel(tr("font:"), this);
    ui_fontCombo = new QComboBox(this);
    fontLayout->addWidget(fontLabel);
    fontLayout->addWidget(ui_fontCombo);
    mainLayout->addLayout(fontLayout);

    populateFontCombo();

    // 현재 폰트 설정
    int index = ui_fontCombo->findText(settings.getFont().family());
    if (index >= 0)
        ui_fontCombo->setCurrentIndex(index);

    // 폰트 크기 선택
    QHBoxLayout* sizeLayout = new QHBoxLayout;
    QLabel* sizeLabel = new QLabel(tr("size:"), this);
    ui_fontSizeSpin = new QSpinBox(this);
    ui_fontSizeSpin->setRange(6, 100);
    ui_fontSizeSpin->setValue(settings.getFont().pointSize() > 0 ? settings.getFont().pointSize() : 14);
    sizeLayout->addWidget(sizeLabel);
    sizeLayout->addWidget(ui_fontSizeSpin);
    mainLayout->addLayout(sizeLayout);

    // 볼드, 이탤릭 체크박스
    QHBoxLayout* styleLayout = new QHBoxLayout;
    ui_boldCheck = new QCheckBox(tr("bold"), this);
    ui_boldCheck->setChecked(settings.getFont().bold());
    ui_italicCheck = new QCheckBox(tr("italic"), this);
    ui_italicCheck->setChecked(settings.getFont().italic());
    styleLayout->addWidget(ui_boldCheck);
    styleLayout->addWidget(ui_italicCheck);
    mainLayout->addLayout(styleLayout);

    // 줄 간격 입력
    QHBoxLayout* lineSpacingLayout = new QHBoxLayout;
    QLabel* lineSpacingLabel = new QLabel(tr("line spacing:"), this);
    ui_lineSpacingSpin = new QDoubleSpinBox(this);
    ui_lineSpacingSpin->setRange(0.5, 3.0);
    ui_lineSpacingSpin->setSingleStep(0.1);
    ui_lineSpacingSpin->setValue(m_settings.getLineSpacing());

    lineSpacingLayout->addWidget(lineSpacingLabel);
    lineSpacingLayout->addWidget(ui_lineSpacingSpin);
    mainLayout->addLayout(lineSpacingLayout);

    // 글자색 선택
	ui_textColor = settings.getTextColor();
    QHBoxLayout* textColorLayout = new QHBoxLayout;
    QLabel* textColorLabel = new QLabel(tr("font color:"), this);
    ui_textColorButton = new QPushButton(tr("choose"), this);
    connect(ui_textColorButton, &QPushButton::clicked, this, &TextSettingDialog::chooseTextColor);
    textColorLayout->addWidget(textColorLabel);
    textColorLayout->addWidget(ui_textColorButton);
    mainLayout->addLayout(textColorLayout);

    // 배경색 선택
	ui_backgroundColor = settings.getBackgroundColor();
    QHBoxLayout* backgroundColorLayout = new QHBoxLayout;
    QLabel* backgroundColorLabel = new QLabel(tr("backgroud-color:"), this);
    ui_backgroundColorButton = new QPushButton(tr("choose"), this);
    connect(ui_backgroundColorButton, &QPushButton::clicked, this, &TextSettingDialog::chooseBackgroundColor);
    backgroundColorLayout->addWidget(backgroundColorLabel);
    backgroundColorLayout->addWidget(ui_backgroundColorButton);
    mainLayout->addLayout(backgroundColorLayout);

    // 패딩 입력 (상/하/좌/우)
    QGridLayout* paddingLayout = new QGridLayout;
    QLabel* paddingLabel = new QLabel(tr("padding:"), this);
    QLabel* paddingTopLabel = new QLabel(tr("top:"), this);
    QLabel* paddingBottomLabel = new QLabel(tr("bottom:"), this);
    QLabel* paddingLeftLabel = new QLabel(tr("left:"), this);
    QLabel* paddingRightLabel = new QLabel(tr("right:"), this);
    ui_paddingTopSpin = new QSpinBox(this);
    ui_paddingBottomSpin = new QSpinBox(this);
    ui_paddingLeftSpin = new QSpinBox(this);
    ui_paddingRightSpin = new QSpinBox(this);
    ui_paddingTopSpin->setRange(0, 100);
	ui_paddingTopSpin->setValue(m_settings.getPadding().top());
    ui_paddingBottomSpin->setRange(0, 100);
    ui_paddingBottomSpin->setValue(m_settings.getPadding().bottom());
    ui_paddingLeftSpin->setRange(0, 100);
    ui_paddingLeftSpin->setValue(m_settings.getPadding().left());
    ui_paddingRightSpin->setRange(0, 100);
    ui_paddingRightSpin->setValue(m_settings.getPadding().right());
    paddingLayout->addWidget(paddingLabel, 0, 0, 1, 2);
    paddingLayout->addWidget(paddingTopLabel, 1, 0);
    paddingLayout->addWidget(ui_paddingTopSpin, 1, 1);
    paddingLayout->addWidget(paddingBottomLabel, 2, 0);
    paddingLayout->addWidget(ui_paddingBottomSpin, 2, 1);
    paddingLayout->addWidget(paddingLeftLabel, 3, 0);
    paddingLayout->addWidget(ui_paddingLeftSpin, 3, 1);
    paddingLayout->addWidget(paddingRightLabel, 4, 0);
    paddingLayout->addWidget(ui_paddingRightSpin, 4, 1);
    mainLayout->addLayout(paddingLayout);

    // 볼드, 이탤릭 체크박스
    QHBoxLayout* styleAutoLayout = new QHBoxLayout;
    ui_autoNextCheck = new QCheckBox(tr("when last/first page move prev/next file."), this);
    ui_autoNextCheck->setChecked(m_settings.isAutoNext());
    styleAutoLayout->addWidget(ui_autoNextCheck);
    mainLayout->addLayout(styleAutoLayout);


    // OK / Cancel 버튼
    ui_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(ui_buttonBox);

    // 기본적으로 컨트롤값 변경 시 프리뷰 업데이트 슬롯 연결
    connect(ui_searchEdit, &QLineEdit::textChanged, this, &TextSettingDialog::filterFonts);
    connect(ui_fontCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreview()));
    connect(ui_fontSizeSpin, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
    connect(ui_boldCheck, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
    connect(ui_italicCheck, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
    connect(ui_lineSpacingSpin, SIGNAL(valueChanged(double)), this, SLOT(updatePreview()));
    connect(ui_paddingTopSpin, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
    connect(ui_paddingBottomSpin, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
    connect(ui_paddingLeftSpin, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
    connect(ui_paddingRightSpin, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));

    // OK/Cancel에 따른 처리: dlgReject()에서 미리보기 복원, dlgAccept()에서 최종 적용.
    connect(ui_buttonBox, &QDialogButtonBox::accepted, this, &TextSettingDialog::dlgAccept);
    connect(ui_buttonBox, &QDialogButtonBox::rejected, this, &TextSettingDialog::dlgReject);

}

TextSettingDialog::~TextSettingDialog()
{
}

void TextSettingDialog::populateFontCombo(const QString& filter)
{
    ui_fontCombo->clear();

    QFontDatabase fd;
    fd.writingSystemName(QFontDatabase::Korean);
    QStringList families = fd.families();

    if (!filter.isEmpty()) {
        QString filterLower = filter.toLower();
        for (const QString& family : families) {
            if (family.toLower().contains(filterLower))
                ui_fontCombo->addItem(family);
        }
    }
    else {
        ui_fontCombo->addItems(families);
    }
}

void TextSettingDialog::filterFonts(const QString& text)
{
    populateFontCombo(text);
}

QFont TextSettingDialog::selectedFont() const
{
    QFont font;
    font.setFamily(ui_fontCombo->currentText());
    font.setPointSize(ui_fontSizeSpin->value());
    font.setBold(ui_boldCheck->isChecked());
    font.setItalic(ui_italicCheck->isChecked());
    return font;
}

double TextSettingDialog::selectedLineSpacing() const
{
    return ui_lineSpacingSpin->value();
}

QColor TextSettingDialog::selectedTextColor() const
{
    return ui_textColor;
}

QColor TextSettingDialog::selectedBackgroundColor() const
{
    return ui_backgroundColor;
}

QMargins TextSettingDialog::selectedPadding() const
{
    return QMargins(ui_paddingLeftSpin->value(), ui_paddingTopSpin->value(),
        ui_paddingRightSpin->value(), ui_paddingBottomSpin->value());
}

void TextSettingDialog::chooseTextColor()
{
    QColor color = QColorDialog::getColor(m_settings.getTextColor(), this, tr("font color"));
    if (color.isValid()) {
        ui_textColor = color;
        ui_textColorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        updatePreview();
    }
}

void TextSettingDialog::chooseBackgroundColor()
{
    QColor color = QColorDialog::getColor(m_settings.getBackgroundColor(), this, tr("backgroud-color"));
    if (color.isValid()) {
        ui_backgroundColor = color;
        ui_backgroundColorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        updatePreview();
    }
}

TextSettingProps TextSettingDialog::getTextSettings()
{
    m_settings.setFont(selectedFont());
    m_settings.setLineSpacing(selectedLineSpacing());
    m_settings.setTextColor(selectedTextColor());
    m_settings.setBackgroundColor(selectedBackgroundColor());
    m_settings.setPadding(selectedPadding());
	m_settings.setAutoNext(ui_autoNextCheck->isChecked());

    return m_settings;
}

// 이 슬롯은 모든 컨트롤의 변경시 호출되어 preview를 emit합니다.
void TextSettingDialog::updatePreview()
{
	m_settings.setFont(selectedFont());
	m_settings.setLineSpacing(selectedLineSpacing());
	m_settings.setTextColor(selectedTextColor());
	m_settings.setBackgroundColor(selectedBackgroundColor());
	m_settings.setPadding(selectedPadding());

    emit settingsChanged(m_settings);
}

// OK를 눌렀을 때 최종 설정 적용
void TextSettingDialog::dlgAccept()
{
    accept();
}

// Cancel을 눌렀을 때 원래 상태로 복원하기 위한 슬롯
void TextSettingDialog::dlgReject()
{
    reject();
}
