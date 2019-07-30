#include "eye.h"
#include <QOpenGLShaderProgram>

Eye::Eye()
{
    m_Scale = 1.0f;
    m_GlobalTransform.setToIdentity();
}

void Eye::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    if(functions != nullptr) return;

    m_ViewMatrix.setToIdentity();
    m_ViewMatrix.translate(m_Translate);
    m_ViewMatrix.rotate(m_Rotate);
    m_ViewMatrix.scale(m_Scale);
    m_ViewMatrix = m_ViewMatrix * m_GlobalTransform.inverted();

    program->setUniformValue("u_ViewMatrix", m_ViewMatrix);
}

void Eye::rotate(const QQuaternion &r)
{
    m_Rotate = r * m_Rotate;

    m_ViewMatrix.setToIdentity();
    m_ViewMatrix.translate(m_Translate);
    m_ViewMatrix.rotate(m_Rotate);
    m_ViewMatrix.scale(m_Scale);
    m_ViewMatrix = m_ViewMatrix * m_GlobalTransform.inverted();
}

void Eye::translate(const QVector3D &t)
{
    m_Translate += t;

    m_ViewMatrix.setToIdentity();
    m_ViewMatrix.translate(m_Translate);
    m_ViewMatrix.rotate(m_Rotate);
    m_ViewMatrix.scale(m_Scale);
    m_ViewMatrix = m_ViewMatrix * m_GlobalTransform.inverted();
}

void Eye::scale(const float &s)
{
    m_Scale *= s;

    m_ViewMatrix.setToIdentity();
    m_ViewMatrix.translate(m_Translate);
    m_ViewMatrix.rotate(m_Rotate);
    m_ViewMatrix.scale(m_Scale);
    m_ViewMatrix = m_ViewMatrix * m_GlobalTransform.inverted();
}

void Eye::setGlobalTransform(const QMatrix4x4 &gt)
{
    m_GlobalTransform = gt;

    m_ViewMatrix.setToIdentity();
    m_ViewMatrix.translate(m_Translate);
    m_ViewMatrix.rotate(m_Rotate);
    m_ViewMatrix.scale(m_Scale);
    m_ViewMatrix = m_ViewMatrix * m_GlobalTransform.inverted();
}
