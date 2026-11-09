#include "FramelessWindowHelper.h"

#if defined(Q_OS_WIN)
#include <windows.h>
#include <windowsx.h>
#endif

namespace Jonaki {

FramelessWindowHelper::FramelessWindowHelper(QWidget* window)
    : m_window(window) {
    if (m_window) {
        m_window->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
    }
}

void FramelessWindowHelper::setDraggableWidget(QWidget* widget) {
    m_titleBar = widget;
}

bool FramelessWindowHelper::handleNativeEvent(const QByteArray& eventType, void* message, qintptr* result) {
#if defined(Q_OS_WIN)
    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        if (!msg || !m_window) return false;

        switch (msg->message) {
            case WM_NCHITTEST: {
                POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
                RECT winRect;
                GetWindowRect(msg->hwnd, &winRect);

                int x = pt.x - winRect.left;
                int y = pt.y - winRect.top;
                int w = winRect.right - winRect.left;
                int h = winRect.bottom - winRect.top;

                bool left = (x < m_borderWidth);
                bool right = (x >= w - m_borderWidth);
                bool top = (y < m_borderWidth);
                bool bottom = (y >= h - m_borderWidth);

                if (top && left) { *result = HTTOPLEFT; return true; }
                if (top && right) { *result = HTTOPRIGHT; return true; }
                if (bottom && left) { *result = HTBOTTOMLEFT; return true; }
                if (bottom && right) { *result = HTBOTTOMRIGHT; return true; }
                if (left) { *result = HTLEFT; return true; }
                if (right) { *result = HTRIGHT; return true; }
                if (top) { *result = HTTOP; return true; }
                if (bottom) { *result = HTBOTTOM; return true; }

                // Check title bar region (31px height)
                if (m_titleBar && y < m_titleBar->height()) {
                    QPoint localPt = m_window->mapFromGlobal(QPoint(pt.x, pt.y));
                    QWidget* child = m_window->childAt(localPt);
                    // If hovering directly over title bar container (not interactive child button)
                    if (child == m_titleBar || (child && child->property("isTitleBarDraggable").toBool())) {
                        *result = HTCAPTION;
                        return true;
                    }
                }
                break;
            }
            default:
                break;
        }
    }
#else
    Q_UNUSED(eventType);
    Q_UNUSED(message);
    Q_UNUSED(result);
#endif
    return false;
}

} // namespace Jonaki
