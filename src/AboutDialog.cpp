#include "AboutDialog.h"


AboutDialog::AboutDialog(QWidget* parent)
	:QDialog(parent)
{
	setWindowTitle("About");
	setFixedSize(400, 600);
    //f:\qt\6.8.2\msvc2022_64\bin\windeployqt6.exe
    // 
    // - 압축파일 지원 1
    // - 파일 그룹화 기능 추가 (파일 삭제, 일괄 이름 변경을 위한 그룹)
    // 
    // 이미지 단축키 설정 추가 --- 나아중에 

    const QString titleText = R"(
       <h2>SzViewer</h2>
    )";

    const QString helpText = R"(
        <p><b>텍스트 뷰어 단축키</b><br>
        ← → : 페이지 좌우 이동<br>
        PgUp, PgDn : 폴더내 다음 파일, 이전 파일<br>
        Del : 파일 삭제<br>
        현재 *.txt 만 지원합니다.<br>
        </p>
        

        <p><b>이미지 뷰어 단축키</b><br>
        ← → : 이전/다음 파일<br>
        PgUp, PgDn : 이전/다음 폴더<br>
        Del : 파일 삭제<br>
        1 : 윈도우 맞춤<br>
        2 : 원본 크기<br>
        3 : 가로 맞춤<br>
        4 : 세로 맞춤<br>
        5 : 이미지가 화면보다 클때 맞춤<br>
        + : 확대<br>
        - : 축소<br>
        f : 전체화면/ 전체화면 종료 (esc)<br>
        F2 : 파일 이름 변경<br>
        F3 : 분할 보기 파일 이름 변경<br>
        CTRL + F2 : 폴더 이름 변경<br>
        N : 좌우 보기 변경(일본만화)<br>
        [ : 빈 파일 추가(현재파일 위치)<br>
        ] : 파일 추가(다음파일 위치) <br>
        *.jpg *.jpeg *.png *.bmp *.gif *.webp *.ico *.svg 등을 지원합니다.<br>
        webp는 지원예정입니다.<br>
        </p>
    )";


    const QString aboutText = R"(
        <p><b>작성자</b> : szJs<br>
        <b>이메일</b> : <a href="mailto:sizz.js@gmail.com">sizz.js@gmail.com</a></p>
        <b>버전</b> : 0.2.1 (2025.03.03)</p>
        <p><b>Icons</b><br>
        Material Design Icons by Google<br>
        Licensed under the Apache License 2.0
        </p>
    )";


    // 레이아웃 및 라벨 추가
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* labelTitle = new QLabel(titleText, this);
    labelTitle->setTextFormat(Qt::RichText);
    labelTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(labelTitle);

    QLabel* labelHelp = new QLabel(helpText, this);
    labelHelp->setTextFormat(Qt::RichText);
    labelHelp->setAlignment(Qt::AlignLeft);
    layout->addWidget(labelHelp);

    QLabel* labelAbout = new QLabel(aboutText, this);
    labelAbout->setTextFormat(Qt::RichText);
    labelAbout->setAlignment(Qt::AlignLeft);
    layout->addWidget(labelAbout);
    setLayout(layout);

}

AboutDialog::~AboutDialog()
{
}
