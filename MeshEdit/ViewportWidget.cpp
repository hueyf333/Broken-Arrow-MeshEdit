#include "ViewportWidget.h"

#include <QMouseEvent>
#include <QTimer>

ViewportWidget::ViewportWidget(QWidget* parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setFocusPolicy(Qt::StrongFocus);

    frameTimer_ = new QTimer(this);
    frameTimer_->setInterval(16);
    connect(frameTimer_, &QTimer::timeout, this, &ViewportWidget::tick);
}

void ViewportWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (!renderer_.isInitialized()) {
        renderer_.initialize(reinterpret_cast<HWND>(winId()), width(), height());
        frameTimer_->start();
    }
}

void ViewportWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (renderer_.isInitialized()) {
        renderer_.resize(width(), height());
    }
}

void ViewportWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    if (renderer_.isInitialized()) {
        renderer_.render();
    }
}

void ViewportWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        rotating_ = true;
        lastMousePos_ = event->pos();
    }
}

void ViewportWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!rotating_) {
        return;
    }
    const QPoint delta = event->pos() - lastMousePos_;
    lastMousePos_ = event->pos();
    renderer_.addRotation(delta.x() * 0.005f, delta.y() * 0.005f);
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        rotating_ = false;
    }
}

void ViewportWidget::tick() {
    if (renderer_.isInitialized()) {
        renderer_.update(0.016f);
        update();
    }
}
