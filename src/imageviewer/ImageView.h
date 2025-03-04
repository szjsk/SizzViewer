#pragma once
#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QScrollArea>
#include <QLabel>
#include <QPixmap>
#include <QMovie>
#include <QFileInfo>
#include <QPainter>
#include <QBuffer>

class ImageView : public QScrollArea
{
    Q_OBJECT

public:
	enum ScaleMode {
		ORIGINAL,
		FitToWindow,
		FitToWidth,
		FitToHeight,
		ScaleByPercentage,
		FitIfLARGE

	};
	enum Align {
		LEFT,
		CENTER,
		RIGHT,
		CENTER_SPREAD
	};

	struct ImageInfo {
		bool isGif = false;
		QMovie* originMovie;
		QPixmap* originPixmap;
		QPixmap* changePixmap;
		QSize originSize;
		Align align;
	};


    explicit ImageView(QWidget* parent = nullptr);
	void loadImage(QByteArray data, QString fileName, ScaleMode scaleMode, int percentage, Align align);
	void resize(ScaleMode mode, int percentage);
	void movieStop();
	void clear();
	void rotate(int degree, bool isFlip);
	void setAlignment(Align align);

private:
	QPixmap getScaledPixmap(QPixmap* pixmap, QSize originSize, ScaleMode mode, int percentage);
	QMovie* getScaledQMovie(QMovie* movie, QSize originSize, ScaleMode mode, int percentage);
	
private: //variables
	QScrollArea* ui_scrollArea;
	QLabel* ui_label;
	ImageInfo m_imageInfo;

};

#endif // IMAGEVIEW_H
