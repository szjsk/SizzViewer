#include "TextSearchDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QTimer>

TextSearchDialog::TextSearchDialog(QWidget* parent, TextViewContainer* textViewContainer)
	: QDialog(parent)
{
    setWindowTitle("Search");

    // 기본 레이아웃 설정
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topLayout = new QHBoxLayout;

    ui_searchLineEdit = new QLineEdit(this);
    ui_searchButton = new QPushButton("search", this);
    topLayout->addWidget(ui_searchLineEdit);
    topLayout->addWidget(ui_searchButton);
    mainLayout->addLayout(topLayout);

    // 결과를 보여줄 테이블 설정
    ui_resultTable = new QTableWidget(this);
    ui_resultTable->setColumnCount(3);
    QStringList headers;
    headers << "text" << "page" << "line";
    ui_resultTable->setHorizontalHeaderLabels(headers);
    ui_resultTable->horizontalHeader()->setStretchLastSection(true);
    ui_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_resultTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_resultTable->setAlternatingRowColors(true);
    
    // 각 열의 너비 설정
    ui_resultTable->setColumnWidth(0, 400); // 라인 열 너비
    ui_resultTable->setColumnWidth(1, 60); // 페이지번호 열 너비
    ui_resultTable->setColumnWidth(2, 60); // 위치 열 너비

    mainLayout->addWidget(ui_resultTable);
    connect(ui_searchButton, &QPushButton::clicked, this, [this, textViewContainer]() {
        QString searchText = ui_searchLineEdit->text();
        if (searchText.isEmpty()) {
            return;
        }

        // 이전 결과 제거
        ui_resultTable->setRowCount(0);
        textViewContainer->performSearch(ui_searchLineEdit->text(), textViewContainer->getFileInfo());
       });


    connect(ui_resultTable, &QTableWidget::cellClicked, this, [this](int row, int column) {

        long page = ui_resultTable->item(row, 1)->text().toInt();
        long line = ui_resultTable->item(row, 2)->text().toInt();

        emit rowSelected(ui_searchLineEdit->text(), page, line);
        });

    resize(600, 400); // 초기 크기 설정

    // TextViewContainer와 시그널-슬롯 연결
   connect(textViewContainer, &TextViewContainer::searchResultReady, this, &TextSearchDialog::addResult);


}

TextSearchDialog::~TextSearchDialog()
{
}

void TextSearchDialog::addResult(QString searchResult, long page, long line)
{
    int row = ui_resultTable->rowCount();
    ui_resultTable->insertRow(row);

    QTableWidgetItem* searchResultItem = new QTableWidgetItem(searchResult);
    QTableWidgetItem* pageItem = new QTableWidgetItem(QString::number(page));
    QTableWidgetItem* lineItem = new QTableWidgetItem(QString::number(line));

    ui_resultTable->setItem(row, 0, searchResultItem);
    ui_resultTable->setItem(row, 1, pageItem);
    ui_resultTable->setItem(row, 2, lineItem);

}
