#ifndef TRANSFORMATIONAL_H
#define TRANSFORMATIONAL_H

class QQuaternion;
class QVector3D;
class QMatrix4x4;
class QOpenGLShaderProgram;
class QOpenGLFunctions;

class Transformational
{
public:
    virtual ~Transformational();

    virtual void rotate(const QQuaternion &r) = 0; // вращение
    virtual void translate(const QVector3D &t) = 0; // перенос
    virtual void scale(const float &s) = 0; // масштаб
    virtual void setGlobalTransform(const QMatrix4x4 &gt) = 0; // позиция и поворот в лок.сис.коорд. в пределах род.глоб.сис.коорд.
    virtual void draw(QOpenGLShaderProgram* program, QOpenGLFunctions* functions) = 0; // отрисовка
};

#endif // TRANSFORMATIONAL_H
