#include "oglwidget.h"
#include "engine3d/object3d.h"
#include "engine3d/object3dgroup.h"
#include "engine3d/eye.h"
#include "engine3d/skybox.h"
#include "engine3d/material.h"

#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QtMath>

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_ShadowTextureSlot = GL_TEXTURE2;  // 0 и 1 текстурные слоты уже заняты

    m_PointCloudFilteringQuality = 1.5f; // влияет на производительность, можно параметризировать
    m_ShadowMapSize = 1024;// влияет на производительность, можно параметризировать
    //m_FBWidth = m_FBHeight = static_cast<int>(m_ShadowMapSize);

    m_LightRotateX = 50.0f; // * угол наклона освещения можно параметризировать
    m_LightRotateY = 60.0f; // *

    m_ProjectionLightMatrix.setToIdentity();
    m_ProjectionLightMatrix.ortho(-40.0f, 40.0f, -40.0f, 40.0f, -40.0f, 40.0f); // можно параметризировать

    m_ShadowLightMatrix.setToIdentity();
    m_ShadowLightMatrix.rotate(m_LightRotateX, 1.0f, 0.0f, 0.0f); // * Важен порядок
    m_ShadowLightMatrix.rotate(m_LightRotateY, 0.0f, 1.0f, 0.0f); // *

    m_LightMatrix.setToIdentity();
    m_LightMatrix.rotate(-m_LightRotateY, 0.0f, 1.0f, 0.0f); // *
    m_LightMatrix.rotate(-m_LightRotateX, 1.0f, 0.0f, 0.0f); // *

    m_GlobalGroup = new Object3DGroup("Global");
    m_Eye = new Eye;
    m_Eye->translate(QVector3D(0.0f, 0.0f, 0.0f));

    currentGroup = 0;
}

OGLWidget::~OGLWidget()
{
    makeCurrent(); // убирает варнинг "QOpenGLTexturePrivate::destroy() called without a current context"
    for(auto o: m_Objects) delete o;
    for(auto o: m_Groups) delete o;
    delete m_Eye;
    delete m_SkyBox;
    delete m_GlobalGroup;
}

void OGLWidget::initializeGL()
{
    context()->functions()->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    context()->functions()->glEnable(GL_DEPTH_TEST);
    context()->functions()->glEnable(GL_CULL_FACE);

    initShaders();

    float step = 2.0f;

    m_Groups.append(new Object3DGroup("cube1"));
    for(float x = -step; x <= step; x += step)
    {
        for(float y = -step; y <= step; y += step)
        {
            for(float z = -step; z <= step; z += step)
            {
                initParallelogram(1.0f, 1.0f, 1.0f, new QImage(":/textures/cube1.png"), new QImage(":/textures/cube1_n.png"));
                m_Objects.last()->translate(QVector3D(x, y, z));
                m_Groups.last()->add(m_Objects.last());
            }
        }
    }
    m_Groups.last()->translate(QVector3D(-4.0f, 0.0f, 0.0f));

    m_Groups.append(new Object3DGroup("cube2"));
    for(float x = -step; x <= step; x += step)
    {
        for(float y = -step; y <= step; y += step)
        {
            for(float z = -step; z <= step; z += step)
            {
                initParallelogram(1.0f, 1.0f, 1.0f, new QImage(":/textures/cube2.png"), new QImage(":/textures/cube2_n.png"));
                m_Objects.last()->translate(QVector3D(x, y, z));
                m_Groups.last()->add(m_Objects.last());
            }
        }
    }
    m_Groups.last()->translate(QVector3D(4.0f, 0.0f, 0.0f));

    m_Groups.append(new Object3DGroup("cube3"));
    for(float x = -step; x <= step; x += step)
    {
        for(float y = -step; y <= step; y += step)
        {
            for(float z = -step; z <= step; z += step)
            {
                initParallelogram(1.0f, 1.0f, 1.0f, new QImage(":/textures/cube3.png"), new QImage(":/textures/cube3_n.png"));
                m_Objects.last()->translate(QVector3D(x, y, z));
                m_Groups.last()->add(m_Objects.last());
            }
        }
    }
    m_Groups.last()->translate(QVector3D(0.0f, 0.0f, -8.0f));

    m_Groups.append(new Object3DGroup("sphere-cube-pyramid"));

    m_Objects.append(new Object3D);
    if(m_Objects.last()->load(":/models/sphere.obj"))
    {
        m_Objects.last()->scale(1.5f);
        m_Objects.last()->translate(QVector3D(-2.0f, 2.0f, 2.0f));
        m_Groups.last()->add(m_Objects.last());

    }

    m_Objects.append(new Object3D);
    if(m_Objects.last()->load(":/models/cube.obj"))
    {
        m_Objects.last()->scale(2.0f);
        m_Objects.last()->translate(QVector3D(2.0f, -2.0f, 2.0f));
        m_Groups.last()->add(m_Objects.last());
    }

    m_Objects.append(new Object3D);
    if(m_Objects.last()->load(":/models/pyramid.obj"))
    {
        m_Objects.last()->scale(1.5f);
        m_Objects.last()->translate(QVector3D(2.0f, 2.0f, -2.0f));
        m_Groups.last()->add(m_Objects.last());
    }
    m_Groups.last()->translate(QVector3D(10.0f, -10.0f, -10.0f));

    m_Groups.append(new Object3DGroup("All sphere-cube-pyramid + cubes"));
    for(int i = 0; i < m_Groups.size() -1; i++)
        m_Groups.last()->add(m_Groups.at(i));
     m_Groups.last()->translate(QVector3D(0.0f, 10.0f, 0.0f));
    m_GlobalGroup->add(m_Groups.last());

    m_Groups.append(new Object3DGroup("big cube"));
    m_Objects.append(new Object3D);
    if(m_Objects.last()->load(":/models/cube.obj"))
    {
        m_Objects.last()->scale(3.0f);
        m_Objects.last()->translate(QVector3D(0.0f, -4.9f, 0.0f));
        m_Groups.last()->add(m_Objects.last());
    }
    m_GlobalGroup->add(m_Groups.last());

    m_Groups.append(new Object3DGroup("table"));
    initParallelogram(60.0, 5.0, 60.0, new QImage(":/textures/cube3.png"));//, new QImage(":/textures/cube3_n.png"));
    m_Objects.last()->translate(QVector3D(0.0, -10.0, 0.0));
    m_Groups.last()->add(m_Objects.last());
    m_GlobalGroup->add(m_Groups.last());

    m_SkyBox = new SkyBox(1000.0f,
                          QImage(":/textures/sky/sky_forward.png"),
                          QImage(":/textures/sky/sky_top.png"),
                          QImage(":/textures/sky/sky_bottom.png"),
                          QImage(":/textures/sky/sky_left.png"),
                          QImage(":/textures/sky/sky_right.png"),
                          QImage(":/textures/sky/sky_back.png"));

    m_DepthBuffer = new QOpenGLFramebufferObject(m_ShadowMapSize, m_ShadowMapSize, QOpenGLFramebufferObject::Depth);

    timerStart();
}

void OGLWidget::resizeGL(int w, int h)
{
    float aspect = w / (h? static_cast<float>(h) : 1);
    m_ProjectionMatrix.setToIdentity();
    m_ProjectionMatrix.perspective(45, aspect, 0.01f, 1000.0f); // посл. 2 параметра - настройка плоскостей отсечения
}

void OGLWidget::paintGL()
{
    // отрисовка во фреймбуфер
    m_DepthBuffer->bind();

    context()->functions()->glViewport(0, 0, m_ShadowMapSize, m_ShadowMapSize);
    context()->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_ProgramDepth.bind();
    m_ProgramDepth.setUniformValue("u_ProjectionLightMatrix", m_ProjectionLightMatrix);
    m_ProgramDepth.setUniformValue("u_ShadowLightMatrix", m_ShadowLightMatrix);
    for(auto o: m_Objects) o->draw(&m_ProgramDepth, context()->functions());
    m_ProgramDepth.release();

    m_DepthBuffer->release();

    GLuint texture = m_DepthBuffer->texture();
    context()->functions()->glActiveTexture(m_ShadowTextureSlot);
    context()->functions()->glBindTexture(GL_TEXTURE_2D, texture);

    // Отрисовка на экран
    context()->functions()->glViewport(0, 0, width(), height());
    context()->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_ProgramSkyBox.bind();
    m_ProgramSkyBox.setUniformValue("u_ProjectionMatrix", m_ProjectionMatrix);
    m_Eye->draw(&m_ProgramSkyBox);
    m_SkyBox->draw(&m_ProgramSkyBox, context()->functions());
    m_ProgramSkyBox.release();

    m_ProgramObject.bind();
    m_ProgramObject.setUniformValue("u_ShadowMap", m_ShadowTextureSlot - GL_TEXTURE0);
    m_ProgramObject.setUniformValue("u_ShadowMapSize", static_cast<float>(m_ShadowMapSize));
    m_ProgramObject.setUniformValue("u_PointCloudFilteringQuality", m_PointCloudFilteringQuality);
    m_ProgramObject.setUniformValue("u_ProjectionMatrix", m_ProjectionMatrix);
    m_ProgramObject.setUniformValue("u_LightDirection", QVector4D(0.0f, 0.0f, -1.0f, 0.0f));
    m_ProgramObject.setUniformValue("u_ProjectionLightMatrix", m_ProjectionLightMatrix);
    m_ProgramObject.setUniformValue("u_ShadowLightMatrix", m_ShadowLightMatrix);
    m_ProgramObject.setUniformValue("u_LightMatrix", m_LightMatrix);
    m_ProgramObject.setUniformValue("u_LightPower", 1.0f); // сила освещения, можно параметризировать
    m_Eye->draw(&m_ProgramObject);
    for(auto o: m_Objects) o->draw(&m_ProgramObject, context()->functions());
    m_ProgramObject.release();
}

void OGLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        m_MousePosition = QVector2D(event->localPos());
    }

    event->accept();
}

void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    switch (event->buttons())
    {
        case Qt::LeftButton:
        {
            QVector2D diffpos = QVector2D(event->localPos()) - m_MousePosition;
            m_MousePosition = QVector2D(event->localPos());

            float angle = diffpos.length() / 2.0f;
            QVector3D axis = QVector3D(diffpos.y(), diffpos.x(), 0.0f);
            m_Eye->rotate(QQuaternion::fromAxisAndAngle(axis, angle));

            update();
        }

    }
    event->accept();
}

void OGLWidget::wheelEvent(QWheelEvent *event)
{
    float mod = event->modifiers() & Qt::ControlModifier ? 3.0f : 1.0f;

    if(event->delta() > 0) m_Eye->translate(QVector3D(0.0f, 0.0f, 0.25f * mod));
    else if(event->delta() < 0) m_Eye->translate(QVector3D(0.0f, 0.0f, -0.25f * mod));

    update();
}

void OGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Q:
        {
            close();
            break;
        }
        case Qt::Key_Tab:
        {
            currentGroup >= m_Groups.size() - 1 ? currentGroup = 0 : currentGroup++;
            for(auto g: m_Groups) g->del(m_Eye);
            m_Groups.at(currentGroup)->add(m_Eye);
            qDebug() << "Current group:" << m_Groups.at(currentGroup)->Name();
            break;
        }
        case Qt::Key_Delete:
        {
            for(auto g: m_Groups) g->del(m_Eye);
            break;
        }
        case Qt::Key_Escape:
        {
            for(auto g: m_Groups) g->del(m_Eye);
            QMatrix4x4 m; m.setToIdentity();
            m_Eye->setGlobalTransform(m);
            qDebug() << "Current group: None";
            break;
        }
        case Qt::Key_Space:
        {
            m_Timer.isActive() ? timerStop() : timerStart();
            qDebug() << "Timer is active:" << m_Timer.isActive();
            break;
        }
    }
}

void OGLWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    for(int i = 0; i < m_Objects.count() - 2; i++) // кроме последних 2х объектов
    {
        if(i % 2 == 0)
        {
            m_Objects.at(i)->rotate(QQuaternion::fromAxisAndAngle(
                                        1.0f, 0.0f, 0.0f, static_cast<float>(qSin(m_AngleObject))));
            m_Objects.at(i)->rotate(QQuaternion::fromAxisAndAngle(
                                        0.0f, 1.0f, 0.0f, static_cast<float>(qCos(m_AngleObject))));
        }
        else
        {
            m_Objects.at(i)->rotate(QQuaternion::fromAxisAndAngle(
                                        0.0f, 1.0f, 0.0f, static_cast<float>(qSin(m_AngleObject))));
            m_Objects.at(i)->rotate(QQuaternion::fromAxisAndAngle(
                                        1.0f, 0.0f, 0.0f, static_cast<float>(qCos(m_AngleObject))));
        }
    }

    m_Groups.at(0)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 0.0f, 1.0f, static_cast<float>(qSin(m_AngleGrop1))));
    m_Groups.at(0)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 1.0f, 0.0f, static_cast<float>(-qSin(m_AngleGrop1))));

    m_Groups.at(1)->rotate(QQuaternion::fromAxisAndAngle(
                               1.0f, 0.0f, 0.0f, static_cast<float>(qCos(m_AngleGrop2))));
    m_Groups.at(1)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 1.0f, 0.0f, static_cast<float>(-qCos(m_AngleGrop2))));

    m_Groups.at(2)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 1.0f, 0.0f, static_cast<float>(qSin(m_AngleGrop3))));
    m_Groups.at(2)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 0.0f, 1.0f, static_cast<float>(qCos(m_AngleGrop3))));

    m_Groups.at(3)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 1.0f, 0.0f, static_cast<float>(qSin(m_AngleGrop4))));
    m_Groups.at(3)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 0.0f, 1.0f, static_cast<float>(qCos(m_AngleGrop4))));

    m_Groups.at(4)->rotate(QQuaternion::fromAxisAndAngle(
                               1.0f, 0.0f, 0.0f, static_cast<float>(-qSin(m_AngleGropMain))));
    m_Groups.at(4)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 1.0f, 0.0f, static_cast<float>(-qCos(m_AngleGropMain))));

    m_AngleObject += M_PI / 360.0;
    m_AngleGrop1 += M_PI / 270.0;
    m_AngleGrop2 -= M_PI / 270.0;
    m_AngleGrop3 -= M_PI / 270.0;
    m_AngleGrop4 -= M_PI / 270.0;
    m_AngleGropMain += M_PI / 720.0;

    update();
}

void OGLWidget::initShaders()
{
    if(! m_ProgramObject.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/object.vsh"))
        close();
    if(! m_ProgramObject.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/object.fsh"))
        close();
    if(! m_ProgramObject.link()) close();

    if(! m_ProgramSkyBox.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/skybox.vsh"))
        close();
    if(! m_ProgramSkyBox.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/skybox.fsh"))
        close();
    if(! m_ProgramSkyBox.link()) close();

    if(! m_ProgramDepth.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/depth.vsh"))
        close();
    if(! m_ProgramDepth.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/depth.fsh"))
        close();
    if(! m_ProgramDepth.link()) close();
}

void OGLWidget::initParallelogram(float width, float height, float depth,
                                  QImage *texturemap, QImage *normalmap)
{
    float width_div_2 = width / 2.0f;
    float height_div_2 = height / 2.0f;
    float depth_div_2 = depth / 2.0f;

    QVector<VertexData> vertexes;
    vertexes.append(VertexData(QVector3D(-width_div_2, height_div_2, depth_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -height_div_2, depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, height_div_2, depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, height_div_2, depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -height_div_2, depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -height_div_2, depth_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));

    vertexes.append(VertexData(QVector3D(width_div_2, height_div_2, depth_div_2), QVector2D(0.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -height_div_2, depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, height_div_2, -depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, height_div_2, -depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -height_div_2, depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -height_div_2, -depth_div_2), QVector2D(1.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));

    vertexes.append(VertexData(QVector3D(width_div_2, height_div_2, depth_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, height_div_2, -depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, height_div_2, depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, height_div_2, depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, height_div_2, -depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, height_div_2, -depth_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));

    vertexes.append(VertexData(QVector3D(width_div_2, height_div_2, -depth_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 0.0f, -1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -height_div_2, -depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, height_div_2, -depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 0.0f, -1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, height_div_2, -depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 0.0f, -1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -height_div_2, -depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -height_div_2, -depth_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f)));

    vertexes.append(VertexData(QVector3D(-width_div_2, height_div_2, depth_div_2), QVector2D(0.0f, 1.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, height_div_2, -depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -height_div_2, depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -height_div_2, depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, height_div_2, -depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -height_div_2, -depth_div_2), QVector2D(1.0f, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f)));

    vertexes.append(VertexData(QVector3D(-width_div_2, -height_div_2, depth_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, -1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -height_div_2, -depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -height_div_2, depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, -1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -height_div_2, depth_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, -1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -height_div_2, -depth_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -height_div_2, -depth_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f)));

    QVector<GLuint> indexes;
    for(GLuint i = 0; i < 36; ++i) indexes.append(i);

    Material* mtl = new Material;
    if (texturemap) mtl->setDiffuseMap(*texturemap);
    if (normalmap) mtl->setNormalMap(*normalmap);
    mtl->setShines(96.0f);
    mtl->setDiffuseColor(QVector3D(1.0f, 1.0f, 1.0f));
    mtl->setAmbienceColor(QVector3D(1.0f, 1.0f, 1.0f));
    mtl->setSpecularColor(QVector3D(1.0f, 1.0f, 1.0f));

    Object3D* eo3d = new Object3D;
    eo3d->calculateTBN(vertexes);
    eo3d->add(new Object3DElement(vertexes, indexes, mtl));
    m_Objects.append(eo3d);
}

void OGLWidget::timerStop()
{
  m_Timer.stop();
}

void OGLWidget::timerStart()
{
   m_Timer.start(30, this);
}

