#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "object3delement.h"
#include "materiallibrary.h"

#include <QVector>

class Object3D :public Transformational
{
public:
    Object3D(){}
    ~Object3D();
    bool load(const QString &filename);
    void add(Object3DElement* obj);
    Object3DElement* get(int index);
    void calculateTBN(QVector<VertexData> &vertdata);

    virtual void rotate(const QQuaternion &r);
    virtual void translate(const QVector3D &t);
    virtual void scale(const float &s);
    virtual void setGlobalTransform(const QMatrix4x4 &gt);
    virtual void draw(QOpenGLShaderProgram* program, QOpenGLFunctions* functions);

private:
    QVector<Object3DElement*> m_Elements;
    MaterialLibrary m_MatLibrary;
};

#endif // OBJECT3D_H
