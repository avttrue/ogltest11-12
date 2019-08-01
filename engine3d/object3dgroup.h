#ifndef OBJECT3DGROUP_H
#define OBJECT3DGROUP_H

#include "transformational.h"

#include <QVector>
#include <QQuaternion>
#include <QVector3D>
#include <QMatrix4x4>

class Object3DGroup : public Transformational
{
public:
    Object3DGroup(const QString &name = "");
    void draw(QOpenGLShaderProgram* program, QOpenGLFunctions* functions);
    void rotate(const QQuaternion &r);
    void translate(const QVector3D &t);
    void scale(const float &s);
    void setGlobalTransform(const QMatrix4x4 &gt);
    void add(Transformational* obj);
    bool del(Transformational* obj);
    void del(const int &ind);
    int size();
    Transformational* at(int i);
    QString Name();
    void setName(const QString &n);

private:
    QString m_Name;
    QQuaternion m_Rotate;
    QVector3D m_Translate;
    QMatrix4x4 m_GlobalTransform;
    QVector<Transformational*> m_Objects;
    float m_Scale;
};

#endif // OBJECT3DGROUP_H
