#include "ImageView.h"

#include "../common/StatusStore.h"

ImageView::ImageView(QWidget* parent)
	: QScrollArea(parent)
{

	setWidgetResizable(true);
	setContentsMargins(0, 0, 0, 0);  // 스크롤 영역의 마진을 0으로 설정
	setFrameStyle(QFrame::NoFrame);  // 스크롤 영역의 경계를 제거

	ui_label = new QLabel(this);
	ui_label->setAlignment(Qt::AlignCenter);
	ui_label->setContentsMargins(0, 0, 0, 0);  // QLabel의 마진을 0으로 설정
	ui_label->setFrameStyle(QFrame::NoFrame);

	this->setWidget(ui_label);
}

void ImageView::clear() {
	movieStop();
	if (m_imageInfo.originPixmap) {
		delete m_imageInfo.originPixmap;
		m_imageInfo.originPixmap = nullptr;
	}

	if (m_imageInfo.changePixmap) {
		delete m_imageInfo.changePixmap;
		m_imageInfo.changePixmap = nullptr;
	}

	if (m_imageInfo.originMovie) {
		QBuffer* buffer = qobject_cast<QBuffer*>(m_imageInfo.originMovie->device());
		delete m_imageInfo.originMovie;
		m_imageInfo.originMovie = nullptr;
		if (buffer) {
			delete buffer;
		}

	}

	ui_label->clear();
	ui_label->setText("");
	//ui_label->setPixmap(nullptr); // 기존 이미지는 초기화
	//ui_label->setMovie(movie);
}

void ImageView::loadImage(QByteArray data, QString fileName, ScaleMode scaleMode, int percentage, Align align)
{

	clear();
	if (data.isEmpty()) {
		ui_label->setText("");
		return;
	}


	QFileInfo fileInfo(fileName);
	QString suffix = fileInfo.suffix().toLower();


	m_imageInfo = ImageInfo();

	if (suffix == "gif") {
		m_imageInfo.isGif = true;
		QBuffer* buffer = new QBuffer();
		buffer->setData(data);
		buffer->open(QIODevice::ReadOnly);
		m_imageInfo.originMovie = new QMovie(buffer, QByteArray(), ui_label);
		m_imageInfo.originMovie->jumpToFrame(0);
		m_imageInfo.originSize = m_imageInfo.originMovie->currentPixmap().size();
		m_imageInfo.align = align;
	}
	else {
		m_imageInfo.isGif = false;
		m_imageInfo.originPixmap = new QPixmap();
		m_imageInfo.originPixmap->loadFromData(data);
		m_imageInfo.originSize = m_imageInfo.originPixmap->size();
		m_imageInfo.align = align;
	}
	resize(scaleMode, percentage);
	setAlignment(align);
}

void ImageView::setAlignment(Align align) {
	m_imageInfo.align = align;
	if (align == Align::LEFT)
		ui_label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	else if (align == Align::CENTER)
		ui_label->setAlignment(Qt::AlignCenter);
	else if (align == Align::RIGHT)
		ui_label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	else
		ui_label->setAlignment(Qt::AlignCenter);
}

void ImageView::resize(ScaleMode mode, int percentage) {

	if (m_imageInfo.isGif) {
		m_imageInfo.originMovie->jumpToFrame(0);
		m_imageInfo.originMovie = getScaledQMovie(m_imageInfo.originMovie, m_imageInfo.originSize, mode, percentage);

		ui_label->setMovie(m_imageInfo.originMovie);
		m_imageInfo.originMovie->start();
	}
	else {
		QPixmap newPixMap = getScaledPixmap(m_imageInfo.originPixmap, m_imageInfo.originSize, mode, percentage);
		if (m_imageInfo.changePixmap) {
			delete m_imageInfo.changePixmap;
		}
		m_imageInfo.changePixmap = new QPixmap(newPixMap);
		ui_label->setPixmap(*m_imageInfo.changePixmap);
	}
	ui_label->adjustSize();
}

QMovie* ImageView::getScaledQMovie(QMovie* movie, QSize originSize, ScaleMode mode, int percentage) {
	int scrollSize = 20;

	switch (mode) {
	case FitToWindow: {
		QSize containerSize = this->size();
		QSize scaledSize = originSize.scaled(containerSize, Qt::KeepAspectRatio);
		movie->setScaledSize(scaledSize);
		break;
	}case FitToWidth: {
		QSize newSize(this->size().width() - scrollSize, originSize.height());
		movie->setScaledSize(newSize);
		break;
	} case FitToHeight: {
		QSize newSize(originSize.width(), this->size().height() - scrollSize);
		movie->setScaledSize(newSize);
		break;
	}case ScaleByPercentage: {
		QSize newSize(originSize.width() * percentage / 100, originSize.height() * percentage / 100);
		movie->setScaledSize(newSize);
		break;
	}case ORIGINAL: {
		movie->setScaledSize(originSize);
		break;
	}case FitIfLARGE: {
		if (originSize.width() > this->size().width() || originSize.height() > this->size().height()) {
			QSize containerSize = this->size();
			QSize scaledSize = originSize.scaled(containerSize, Qt::KeepAspectRatio);
			movie->setScaledSize(scaledSize);
		}
		else {
			movie->setScaledSize(originSize);
		}
		break;
	}
	}

	return movie;
}

QPixmap ImageView::getScaledPixmap(QPixmap* pixmap, QSize originSize, ScaleMode mode, int percentage) {
	int scrollSize = 20;
	switch (mode) {
	case FitToWindow:
		return pixmap->scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	case FitToWidth: {
		return pixmap->scaled(this->size().width() - scrollSize, originSize.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}case FitToHeight:
		return pixmap->scaled(originSize.width(), this->size().height() - scrollSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	case ScaleByPercentage:
		return pixmap->scaled(pixmap->size().width() * percentage / 100, pixmap->size().height() * percentage / 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	case ORIGINAL:
		return pixmap->scaled(originSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	case FitIfLARGE: {
		if (originSize.width() > this->size().width() || originSize.height() > this->size().height()) {
			return pixmap->scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}
		return pixmap->scaled(originSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}default:
		return *pixmap;
	}
}

void ImageView::rotate(int degree, bool isFlip) {

	if (m_imageInfo.isGif && m_imageInfo.originMovie) {

		// GIF의 경우 현재 프레임을 가져와서 회전
		QPixmap currentFrame = m_imageInfo.originMovie->currentPixmap();
		QTransform transform;
		if (isFlip) {
			transform.scale(-1, 1);
		}
		transform.rotate(degree);
		currentFrame = currentFrame.transformed(transform, Qt::SmoothTransformation);
	}
	else if (!m_imageInfo.isGif && m_imageInfo.originPixmap) {

		// 일반 이미지의 경우 QPixmap 회전
		QTransform transform;
		if (isFlip) {
			transform.scale(-1, 1);
		}
		transform.rotate(degree);
		m_imageInfo.changePixmap = new QPixmap(m_imageInfo.originPixmap->transformed(transform, Qt::SmoothTransformation));
		ui_label->setPixmap(*m_imageInfo.changePixmap);


	}

	// 회전 후 크기 조정
	//m_originSize = m_isGif ? ui_label->pixmap().size() : m_pixmap->size();
	//resize(m_scaleMode, m_percentage);
}

void ImageView::movieStop() {
	if (m_imageInfo.isGif && m_imageInfo.originMovie) {
		m_imageInfo.originMovie->stop();
		ui_label->setMovie(nullptr);
		QBuffer* buffer = qobject_cast<QBuffer*>(m_imageInfo.originMovie->device());
		delete m_imageInfo.originMovie;
		m_imageInfo.originMovie = nullptr;
		if (buffer) {
			delete buffer;
		}
	}
}