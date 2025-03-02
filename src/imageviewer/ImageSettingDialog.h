#ifndef IMAGESETTINGDIALOG_H
#define IMAGESETTINGDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include "ImageSettingProps.h"

class ImageSettingDialog : public QDialog
{
    Q_OBJECT

public:
    ImageSettingDialog(ImageSettingProps settings, QWidget* parent = nullptr);
    ~ImageSettingDialog();

    ImageSettingProps getImageSettings();

signals:
    void settingsChanged(const ImageSettingProps& settings);

private slots:
    void updatePreview();
    void dlgAccept();
    void dlgReject();

private:
    ImageView::Align selectedAlign() const;
    bool isAutoNext() const;

    QComboBox* ui_alignCombo;
    QCheckBox* ui_autoNextCheck;
    QDialogButtonBox* ui_buttonBox;

    ImageSettingProps m_settings;
};

#endif // IMAGESETTINGDIALOG_H
