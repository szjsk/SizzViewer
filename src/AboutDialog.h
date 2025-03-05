#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QString>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget* parent = nullptr);
    ~AboutDialog();
};

#endif // ABOUTDIALOG_H
