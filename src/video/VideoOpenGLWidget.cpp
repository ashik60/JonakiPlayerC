#include "VideoOpenGLWidget.h"
#include "../playback/MpvPlaybackEngine.h"
#include "../core/Logger.h"
#include <QOpenGLContext>

namespace Jonaki {

static void* get_proc_address_mpv(void* ctx, const char* name) {
    (void)ctx;
    QOpenGLContext* glctx = QOpenGLContext::currentContext();
    if (!glctx) return nullptr;
    return reinterpret_cast<void*>(glctx->getProcAddress(QByteArray(name)));
}

VideoOpenGLWidget::VideoOpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent) {
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
}

VideoOpenGLWidget::~VideoOpenGLWidget() {
    makeCurrent();
#if defined(HAVE_MPV)
    if (m_mpvGL) {
        mpv_render_context_free(m_mpvGL);
        m_mpvGL = nullptr;
    }
#endif
    doneCurrent();
}

void VideoOpenGLWidget::setEngine(MpvPlaybackEngine* engine) {
    m_engine = engine;
}

void VideoOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();

#if defined(HAVE_MPV)
    if (m_engine && m_engine->rawHandle() && !m_mpvGL) {
        mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr};
        mpv_render_param params[]{
            {MPV_RENDER_PARAM_API_TYPE, const_cast<char*>(MPV_RENDER_API_TYPE_OPENGL)},
            {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
            {MPV_RENDER_PARAM_INVALID, nullptr}
        };

        int err = mpv_render_context_create(&m_mpvGL, m_engine->rawHandle(), params);
        if (err < 0) {
            Logger::error("Failed to create mpv OpenGL render context: " + std::string(mpv_error_string(err)));
        } else {
            mpv_render_context_set_update_callback(m_mpvGL, onMpvUpdate, this);
            Logger::info("mpv OpenGL render context initialized successfully.");
        }
    }
#endif
}

void VideoOpenGLWidget::resizeGL(int w, int h) {
    (void)w;
    (void)h;
}

void VideoOpenGLWidget::paintGL() {
#if defined(HAVE_MPV)
    if (m_mpvGL) {
        int w = width() * devicePixelRatio();
        int h = height() * devicePixelRatio();

        GLint defaultFbo = defaultFramebufferObject();
        mpv_opengl_fbo fbo{defaultFbo, w, h, 0};
        int flip_y = 1;

        mpv_render_param params[]{
            {MPV_RENDER_PARAM_OPENGL_FBO, &fbo},
            {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
            {MPV_RENDER_PARAM_INVALID, nullptr}
        };

        mpv_render_context_render(m_mpvGL, params);
        return;
    }
#endif

    // Fallback: clear to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void VideoOpenGLWidget::onMpvUpdate(void* ctx) {
    auto* self = static_cast<VideoOpenGLWidget*>(ctx);
    if (self) {
        // Trigger paint event on main GUI thread
        QMetaObject::invokeMethod(self, &VideoOpenGLWidget::handleMpvUpdate, Qt::QueuedConnection);
    }
}

void VideoOpenGLWidget::handleMpvUpdate() {
    update();
}

} // namespace Jonaki
