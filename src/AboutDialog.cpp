#include "AboutDialog.h"


AboutDialog::AboutDialog(QWidget* parent)
	:QDialog(parent)
{
	setWindowTitle("Sizz Viewer");
	setFixedSize(400, 600);
    //f:\qt\6.8.2\msvc2022_64\bin\windeployqt6.exe
    // 
    // 
    // 이미지 단축키 설정 추가 --- 나아중에 
    // - 파일 그룹화 기능 추가 (파일 삭제, 일괄 이름 변경을 위한 그룹)

    const QString titleText = R"(
       <h2>Sizz Viewer 0.5.0 (2025-03-06) </h2>
    )";

    const QString helpText = R"(
        <p><b>뷰어 단축키</b><br>
        Del : 파일 삭제<br>
        F2 : 파일 이름 변경<br>
        F5 : 1번 임시 파일 북마크 이동<br>
        F6 : 2번 임시 파일 북마크 이동<br>
        F7 : 3번 임시 파일 북마크 이동<br>
        F8 : 4번 임시 파일 북마크 이동<br>

        <p><b>텍스트 뷰어 단축키</b><br>
        ← → : 페이지 좌우 이동<br>
        PgUp, PgDn : 폴더내 다음 파일, 이전 파일<br>
        현재 *.txt 만 지원합니다.<br>
        화면 왼쪽 클릭 : 이전페이지, 화면 오른쪽 클릭 : 다음 페이지<br>
        </p>
        

        <p><b>이미지 뷰어 단축키</b><br>
        ← → : 이전/다음 파일<br>
        PgUp, PgDn : 이전/다음 폴더<br>
        1 : 윈도우 맞춤<br>
        2 : 원본 크기<br>
        3 : 가로 맞춤<br>
        4 : 세로 맞춤<br>
        5 : 이미지가 화면보다 클때 맞춤<br>
        + : 확대<br>
        - : 축소<br>
        f : 전체화면/ 전체화면 종료 (esc)<br>
        F3 : 폴더 이름 변경<br>
        N : 좌우 보기 변경(일본만화)<br>
        M : 이미지 첫 페이지 빈 페이지 추가<br>
        w,a,s,d : 스크롤 이동 (방향키 up/down은 상하 스크롤 많이 이동)
        *.jpg *.jpeg *.png *.bmp *.gif *.webp *.ico *.svg 등을 지원합니다.<br>
        화면 왼쪽 클릭 : 이전페이지, 화면 오른쪽 클릭 : 다음 페이지<br>
        마우스 오른쪽 버튼 드래그 : 화면 이동<br>
        </p>
    )";


    const QString aboutText = R"(
        <p><b>작성자</b> : szJs<br>
        <b>이메일</b> : <a href="mailto:sizz.js@gmail.com">sizz.js@gmail.com</a></p></br>
        <b>사이트</b> : https://github.com/szjsk/SizzViewer</br>
        <p><b>Third Party Libraries</b><br>
        <b>Qt 6.8.2 </b><br>
        <b>Icons</b><br>
        Material Design Icons by Google - Licensed under the Apache License 2.0<br/>
        <b>miniz</b><br>
        miniz (Public Domain - Unlicense) - Compression library<br/>
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

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(labelHelp);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedHeight(400); // 스크롤 영역의 높이 설정
    layout->addWidget(scrollArea);

    QLabel* labelAbout = new QLabel(aboutText, this);
    labelAbout->setTextFormat(Qt::RichText);
    labelAbout->setAlignment(Qt::AlignLeft);
    layout->addWidget(labelAbout);
    setLayout(layout);

}

AboutDialog::~AboutDialog()
{
}
