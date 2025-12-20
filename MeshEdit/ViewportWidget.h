#pragma once

#include <QWidget>

#include "D3D11Renderer.h"

class QTimer;

class ViewportWidget final : public QWidget {
    Q_OBJECT

public:
    explicit ViewportWidget(QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void tick();

    D3D11Renderer renderer_;
    QTimer* frameTimer_ = nullptr;
    QPoint lastMousePos_;
    bool rotating_ = false;
};
