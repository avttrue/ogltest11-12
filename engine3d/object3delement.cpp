#include "object3delement.h"
#include "material.h"

#include<QOpenGLTexture>
#include<QOpenGLShaderProgram>
#include<QOpenGLFunctions>

Object3DElement::Object3DElement()
    : m_VertexBuffer(QOpenGLBuffer::VertexBuffer), // это значение по-умолчанию
      m_IndexBuffer(QOpenGLBuffer::IndexBuffer),
      m_DiffuseMap(nullptr),
      m_NormalMap(nullptr)
{
    m_Scale = 1.0f;
}

Object3DElement::Object3DElement(const QVector<VertexData> &vert, const QVector<GLuint> &ind, Material *mat)
    : m_VertexBuffer(QOpenGLBuffer::VertexBuffer), // это значение по-умолчанию
      m_IndexBuffer(QOpenGLBuffer::IndexBuffer),
      m_DiffuseMap(nullptr),
      m_NormalMap(nullptr)
{
    m_Scale = 1.0f;
    init(vert, ind, mat);
}

Object3DElement::~Object3DElement()
{
    free();
}

void Object3DElement::free()
{
    if(m_VertexBuffer.isCreated()) m_VertexBuffer.destroy();
    if(m_IndexBuffer.isCreated()) m_IndexBuffer.destroy();

    if(m_DiffuseMap != nullptr && m_DiffuseMap->isCreated())
    { delete m_DiffuseMap; m_DiffuseMap = nullptr; }

    if(m_NormalMap != nullptr && m_NormalMap->isCreated())
    { delete m_NormalMap; m_NormalMap = nullptr; }
}

void Object3DElement::init(const QVector<VertexData> &vert, const QVector<GLuint> &ind, Material *mat)
{
    free();

    m_VertexBuffer.create();
    m_VertexBuffer.bind();
    m_VertexBuffer.allocate(vert.constData(), vert.size() * static_cast<int>(sizeof(VertexData)));
    m_VertexBuffer.release();

    m_IndexBuffer.create();
    m_IndexBuffer.bind();
    m_IndexBuffer.allocate(ind.constData(), ind.size() * static_cast<int>(sizeof(GLuint)));
    m_IndexBuffer.release();

    m_Material = mat;

    if(m_Material->isUseDiffuseMap())
    {
        m_DiffuseMap = new QOpenGLTexture(m_Material->DiffuseMap().mirrored());
        m_DiffuseMap->setMinificationFilter(QOpenGLTexture::Nearest);
        m_DiffuseMap->setMagnificationFilter(QOpenGLTexture::Linear);
        m_DiffuseMap->setWrapMode(QOpenGLTexture::Repeat);
    }

    if(m_Material->isUseNormalMap())
    {
        m_NormalMap = new QOpenGLTexture(m_Material->NormalMap().mirrored());
        m_NormalMap->setMinificationFilter(QOpenGLTexture::Nearest);
        m_NormalMap->setMagnificationFilter(QOpenGLTexture::Linear);
        m_NormalMap->setWrapMode(QOpenGLTexture::Repeat);
    }
}

void Object3DElement::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    if(!m_VertexBuffer.isCreated()) {qCritical() << "Vertex buffer is not created"; return; }
    if(!m_IndexBuffer.isCreated()) {qCritical() << "Index buffer is not created"; return; }

    if(m_Material->isUseDiffuseMap())
    {
        m_DiffuseMap->bind(0); // текстурный слот основной текстуры
        program->setUniformValue("u_DiffuseMap", 0);
    }

    if(m_Material->isUseNormalMap())
    {
        m_NormalMap->bind(1); // текстурный слот карты нормалей
        program->setUniformValue("u_NormalMap", 1);
    }

    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(m_Translate); // здесь важен порядок преобразований *
    modelMatrix.rotate(m_Rotate);       // *
    modelMatrix.scale(m_Scale);         // *
    modelMatrix = m_GlobalTransform * modelMatrix;

    program->setUniformValue("u_ModelMatrix", modelMatrix);
    program->setUniformValue("u_MaterialProperty.DiffuseColor", m_Material->DiffuseColor());
    program->setUniformValue("u_MaterialProperty.AmbienceColor", m_Material->AmbienceColor());
    program->setUniformValue("u_MaterialProperty.SpecularColor", m_Material->SpecularColor());
    program->setUniformValue("u_MaterialProperty.Shines", m_Material->Shines());
    program->setUniformValue("u_IsUseDiffuseMap", m_Material->isUseDiffuseMap());
    program->setUniformValue("u_IsUseNormalMap", m_Material->isUseNormalMap());

    m_VertexBuffer.bind();

    int offset = 0;

    int vertloc = program->attributeLocation("a_position");
    program->enableAttributeArray(vertloc);
    program->setAttributeBuffer(vertloc, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int texloc = program->attributeLocation("a_textcoord");
    program->enableAttributeArray(texloc);
    program->setAttributeBuffer(texloc, GL_FLOAT, offset, 2, sizeof(VertexData));

    offset += sizeof(QVector2D);

    int normloc = program->attributeLocation("a_normal");
    program->enableAttributeArray(normloc);
    program->setAttributeBuffer(normloc, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int tangentloc = program->attributeLocation("a_tangent");
    program->enableAttributeArray(tangentloc);
    program->setAttributeBuffer(tangentloc, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int bitangentloc = program->attributeLocation("a_bitangent");
    program->enableAttributeArray(bitangentloc);
    program->setAttributeBuffer(bitangentloc, GL_FLOAT, offset, 3, sizeof(VertexData));

    m_IndexBuffer.bind();

    functions->glDrawElements(GL_TRIANGLES, m_IndexBuffer.size(), GL_UNSIGNED_INT, nullptr);
    functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_VertexBuffer.release();
    m_IndexBuffer.release();

    if(m_Material->isUseDiffuseMap()) m_DiffuseMap->release();
    if(m_Material->isUseNormalMap()) m_NormalMap->release();
}

void Object3DElement::rotate(const QQuaternion &r)
{
    m_Rotate *= r;
}

void Object3DElement::translate(const QVector3D &t)
{
    m_Translate += t;
}

void Object3DElement::scale(const float &s)
{
    m_Scale *= s;
}

void Object3DElement::setGlobalTransform(const QMatrix4x4 &gt)
{
    m_GlobalTransform = gt;
}
