#ifndef FRAMELESSWINDOWHELPER_H
#define FRAMELESSWINDOWHELPER_H

#include <QWidget>
#include <QDebug>

#ifdef Q_OS_WIN
#include <Windowsx.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "user32.lib")
#endif

template <typename T>
class FramelessWindowHelper : public T
{
public:
    FramelessWindowHelper(QWidget *parent = nullptr)
        : T(parent)
    {
        setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
        updateResizeable(m_resizeable);
    }

    void setResizeable(bool resizeable)
    {
        if (m_resizeable == resizeable) {
            return;
        }
        m_resizeable = resizeable;
        updateResizeable(resizeable);
    }

    bool resizeable()
    {
        return m_resizeable;
    }

    void setTitleHeight(int titleHeight)
    {
        m_titleHeight = titleHeight;
    }

    int titleHeight()
    {
        return m_titleHeight;
    }

    void setBorderWidth(int borderWidth)
    {
        m_borderWidth = borderWidth;
    }

    int borderWidth()
    {
        return m_borderWidth;
    }

protected:
    void updateResizeable(bool resizeable)
    {
        bool visible = isVisible();
        if (resizeable) {
            //setWindowFlag(Qt::WindowMaximizeButtonHint);

            // 此行代码是为了带回Aero效果，但副作用同时也带回了标题栏和边框,在nativeEvent::WM_NCCALCSIZE会再次去掉标题栏
            HWND hwnd = (HWND)this->winId();
            DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
            // WS_MAXIMIZEBOX 不加没有双击标题栏最大化，拖动到屏幕边缘aero特效
            // WS_THICKFRAME 不加没有阴影
            // WS_CAPTION 不加会多出边框
            ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);
        } else {
            //setWindowFlag(Qt::WindowMaximizeButtonHint,false);

            HWND hwnd = (HWND)this->winId();
            DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
            ::SetWindowLong(hwnd, GWL_STYLE, style & ~WS_MAXIMIZEBOX);
        }

        //保留一个像素的边框宽度，否则系统不会绘制边框阴影
        const MARGINS shadow = { 1, 1, 1, 1 };
        ::DwmExtendFrameIntoClientArea(HWND(winId()), &shadow);

        setVisible(visible);
    }

    bool nativeEvent(const QByteArray &eventType, void *message, long *result)
    {
        //Workaround for known bug -> check Qt forum : https://forum.qt.io/topic/93141/qtablewidget-itemselectionchanged/13
#if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
        MSG* msg = *reinterpret_cast<MSG**>(message);
#else
        MSG* msg = reinterpret_cast<MSG*>(message);
#endif

        switch (msg->message)
        {
        case WM_NCCALCSIZE:
        {
            NCCALCSIZE_PARAMS& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
            if (params.rgrc[0].top != 0) {
                params.rgrc[0].top -= 1;
            }
            // 清除WS_THICKFRAME和WS_CAPTION的副作用
            *result = WVR_REDRAW;
            return true;
        }
        case WM_NCHITTEST:
        {
            *result = 0;

            long x = GET_X_LPARAM(msg->lParam);
            long y = GET_Y_LPARAM(msg->lParam);

#if 1
            double dpr = this->devicePixelRatio();
            POINT pt;
            pt.x = x;
            pt.y = y;
            ::ScreenToClient(HWND(winId()), &pt);
            QPoint pos(pt.x, pt.y);
            qDebug() << "xy:" << x << ":" << y  << "dpr:" << dpr << "pos" << pos << "width:" << width() << "height" << height();
#else
            double dpr = this->devicePixelRatio();
            QPoint pos = mapFromGlobal(QPointF(x / dpr, y / dpr).toPoint());
            qDebug() << "xy:" << x << ":" << y << "dpr:" << dpr << "pos" << pos << "win pos";
#endif            
            QRect titleRect = QRect(0, 0, width() * dpr, m_titleHeight * dpr);

            // 如果子控件要处理消息则直接返回
            QWidget* childWidget = childAt(pos);
            if (childWidget && !childWidget->windowFlags().testFlag(Qt::WindowTransparentForInput)) {
                return false;
            }

            if(m_resizeable)
            {
                // 缩放区域
                const LONG borderWidth = m_borderWidth;
                RECT winRect;
                GetWindowRect(HWND(winId()), &winRect);
                bool resizeWidth = minimumWidth() != maximumWidth();
                bool resizeHeight = minimumHeight() != maximumHeight();

                if(resizeWidth) {
                    //left border
                    if (x >= winRect.left && x < winRect.left + borderWidth)
                    {
                        *result = HTLEFT;
                    }
                    //right border
                    if (x < winRect.right && x >= winRect.right - borderWidth)
                    {
                        *result = HTRIGHT;
                    }
                }
                if(resizeHeight) {
                    //bottom border
                    if (y < winRect.bottom && y >= winRect.bottom - borderWidth)
                    {
                        *result = HTBOTTOM;
                    }
                    //top border
                    if (y >= winRect.top && y < winRect.top + borderWidth)
                    {
                        *result = HTTOP;
                    }
                }
                if(resizeWidth && resizeHeight) {
                    //bottom left corner
                    if (x >= winRect.left && x < winRect.left + borderWidth &&
                            y < winRect.bottom && y >= winRect.bottom - borderWidth) {
                        *result = HTBOTTOMLEFT;
                    }
                    //bottom right corner
                    if (x < winRect.right && x >= winRect.right - borderWidth &&
                            y < winRect.bottom && y >= winRect.bottom - borderWidth) {
                        *result = HTBOTTOMRIGHT;
                    }
                    //top left corner
                    if (x >= winRect.left && x < winRect.left + borderWidth &&
                            y >= winRect.top && y < winRect.top + borderWidth) {
                        *result = HTTOPLEFT;
                    }
                    //top right corner
                    if (x < winRect.right && x >= winRect.right - borderWidth &&
                            y >= winRect.top && y < winRect.top + borderWidth) {
                        *result = HTTOPRIGHT;
                    }
                }
            }

            // 如果被缩放区域处理的话，则直接返回
            if (0 != *result) {
                return true;
            }

            // *result 依然是 0,指针不在缩放区域，再判断是否在标题栏区域
            // 没有标题栏则不需要处理
            if (0 == m_titleHeight) {
                return false;
            }

            // 处理标题栏点击
            if (titleRect.contains(pos)) {
                *result = HTCAPTION;
                return true;
            }
        } //end case WM_NCHITTEST
        }

        return T::nativeEvent(eventType, message, result);
    }
private:
    int m_borderWidth = 5;
    bool m_resizeable = true;
    int m_titleHeight = 30;
};

#endif // FRAMELESSWINDOWHELPER_H
