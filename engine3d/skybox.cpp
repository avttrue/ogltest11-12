#include "skybox.h"
#include "object3d.h"
#include "material.h"

SkyBox::SkyBox(float width,
               const QImage &img_forward,
               const QImage &img_top,
               const QImage &img_bottom,
               const QImage &img_left,
               const QImage &img_right,
               const QImage &img_back)
{
    float width_div_2 = width / 2.0f;
    m_Box = new Object3D;

    QVector<VertexData> vertexes;
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    fillFace(vertexes, img_forward);

    vertexes.clear();
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    fillFace(vertexes, img_top);

    vertexes.clear();
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    fillFace(vertexes, img_bottom);

    vertexes.clear();
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2), QVector2D(0.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2), QVector2D(1.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2), QVector2D(0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2), QVector2D(1.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    fillFace(vertexes, img_left);

    vertexes.clear();
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2), QVector2D(1.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2), QVector2D(1.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2), QVector2D(0.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2), QVector2D(0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    fillFace(vertexes, img_right);

    vertexes.clear();
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    fillFace(vertexes, img_back);
}

void SkyBox::fillFace(QVector<VertexData> vertexes, const QImage &img)
{
    QVector<GLuint> indexes;
    for(GLuint i = 0; i < 4; i += 4)
    {
        indexes.append(i + 0); indexes.append(i + 2); indexes.append(i + 1);
        indexes.append(i + 2); indexes.append(i + 3); indexes.append(i + 1);
    }

    Material* mtl = new Material;
    mtl->setDiffuseMap(img);
    mtl->setShines(96.0f);
    mtl->setDiffuseColor(QVector3D(1.0f, 1.0f, 1.0f));
    mtl->setAmbienceColor(QVector3D(1.0f, 1.0f, 1.0f));
    mtl->setSpecularColor(QVector3D(1.0f, 1.0f, 1.0f));

    m_Box->add(new Object3DElement(vertexes, indexes, mtl));
}

SkyBox::~SkyBox()
{
    delete m_Box;
}

void SkyBox::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    m_Box->draw(program, functions);
}

void SkyBox::rotate(const QQuaternion &r)
{ (void)r; }

void SkyBox::translate(const QVector3D &t)
{ (void)t; }

void SkyBox::scale(const float &s)
{ (void)s; }

void SkyBox::setGlobalTransform(const QMatrix4x4 &gt)
{ (void)gt; }
