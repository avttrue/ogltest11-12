#include "object3dgroup.h"

Object3DGroup::Object3DGroup(const QString &name)
{
    m_Name = name;
    m_Scale = 1.0f;
}

void Object3DGroup::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    for(auto o: m_Objects) o->draw(program, functions);
}

void Object3DGroup::rotate(const QQuaternion &r)
{
    m_Rotate = r * m_Rotate;

    QMatrix4x4 localmatrix;
    localmatrix.setToIdentity();
    localmatrix.translate(m_Translate);
    localmatrix.rotate(m_Rotate);
    localmatrix.scale(m_Scale);
    localmatrix = m_GlobalTransform * localmatrix;

    for(auto o: m_Objects) o->setGlobalTransform(localmatrix);
}

void Object3DGroup::translate(const QVector3D &t)
{
    m_Translate += t;

    QMatrix4x4 localmatrix;
    localmatrix.setToIdentity();
    localmatrix.translate(m_Translate);
    localmatrix.rotate(m_Rotate);
    localmatrix.scale(m_Scale);
    localmatrix = m_GlobalTransform * localmatrix;

    for(auto o: m_Objects) o->setGlobalTransform(localmatrix);
}

void Object3DGroup::scale(const float &s)
{
    m_Scale *= s;

    QMatrix4x4 localmatrix;
    localmatrix.setToIdentity();
    localmatrix.translate(m_Translate);
    localmatrix.rotate(m_Rotate);
    localmatrix.scale(m_Scale);
    localmatrix = m_GlobalTransform * localmatrix;

    for(auto o: m_Objects) o->setGlobalTransform(localmatrix);
}

void Object3DGroup::setGlobalTransform(const QMatrix4x4 &gt)
{
    m_GlobalTransform = gt;

    QMatrix4x4 localmatrix;
    localmatrix.setToIdentity();
    localmatrix.translate(m_Translate);
    localmatrix.rotate(m_Rotate);
    localmatrix.scale(m_Scale);
    localmatrix = m_GlobalTransform * localmatrix;

    for(auto o: m_Objects) o->setGlobalTransform(localmatrix);
}

void Object3DGroup::add(Transformational *obj)
{
    m_Objects.append(obj);

    QMatrix4x4 localmatrix;
    localmatrix.setToIdentity();
    localmatrix.translate(m_Translate);
    localmatrix.rotate(m_Rotate);
    localmatrix.scale(m_Scale);
    localmatrix = m_GlobalTransform * localmatrix;

    obj->setGlobalTransform(localmatrix);
    qDebug() << "Objects in group" << m_Name << ":" << m_Objects.count();
}

bool Object3DGroup::del(Transformational *obj)
{
    bool result = m_Objects.removeAll(obj) > 0 ? true : false;

    qDebug() << "Objects in group" << m_Name << ":" << m_Objects.count();
    return result;
}

void Object3DGroup::del(const int &ind)
{
    m_Objects.remove(ind);
    qDebug() << "Objects in group" << m_Name << ":" << m_Objects.count();
}

QString Object3DGroup::Name()
{
    return m_Name;
}

void Object3DGroup::setName(const QString &n)
{
    m_Name = n;
}
