#pragma once

#include <QWidget>
#include <QPoint>

namespace Jonaki {

/**
 * @brief Helper for frameless top-level windows providing resizing borders,
 * aero-snap behavior, and dragging title bar logic on Windows and Linux.
 */
class FramelessWindowHelper {
public:
    explicit FramelessWindowHelper(QWidget* window);

    bool handleNativeEvent(const QByteArray& eventType, void* message, qintptr* result);
    void setDraggableWidget(QWidget* widget);
    void setResizeBorderWidth(int width) { m_borderWidth = width; }

private:
    QWidget* m_window = nullptr;
    QWidget* m_titleBar = nullptr;
    int m_borderWidth = 6;
};

} // namespace Jonaki
