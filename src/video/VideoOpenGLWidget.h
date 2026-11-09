#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <memory>

#if defined(HAVE_MPV)
#include <mpv/client.h>
#include <mpv/render_gl.h>
#endif

namespace Jonaki {

class MpvPlaybackEngine;

/**
 * @brief Hardware-accelerated OpenGL rendering widget hosting mpv_render_context.
 *
 * Implements off-screen / swapchain frame rendering with automatic resize handling
 * and DPI scaling support.
 */
class VideoOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit VideoOpenGLWidget(QWidget* parent = nullptr);
    ~VideoOpenGLWidget() override;

    void setEngine(MpvPlaybackEngine* engine);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    static void onMpvUpdate(void* ctx);
    void handleMpvUpdate();

    MpvPlaybackEngine* m_engine = nullptr;

#if defined(HAVE_MPV)
    mpv_render_context* m_mpvGL = nullptr;
#endif
};

} // namespace Jonaki
