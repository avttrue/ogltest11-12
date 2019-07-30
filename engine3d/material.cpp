#include "material.h"

#include <QFile>
#include <QDebug>

Material::Material()
{
    m_IsUseDiffuseMap = false;
    m_IsUseNormalMap = false;
}

QString Material::Name()
{
    return m_Name;
}

void Material::setName(const QString &n)
{
    m_Name = n;
}

QVector3D Material::DiffuseColor() const
{
    return m_DiffuseColor;
}

void Material::setDiffuseColor(const QVector3D &dc)
{
    m_DiffuseColor = dc;
}

QVector3D Material::AmbienceColor() const
{
    return m_AmbienceColor;
}

void Material::setAmbienceColor(const QVector3D &ac)
{
    m_AmbienceColor = ac;
}

QVector3D Material::SpecularColor() const
{
    return m_SpecularColor;
}

void Material::setSpecularColor(const QVector3D &sc)
{
    m_SpecularColor = sc;
}

float Material::Shines() const
{
    return m_Shines;
}

void Material::setShines(const float &s)
{
    m_Shines = s;
}

QImage Material::DiffuseMap() const
{
    return m_DiffuseMap;
}

void Material::setDiffuseMap(const QString &filename)
{
    if(!QFile(filename).exists()) {qDebug() << "File not found:" << filename; return; }

    m_DiffuseMap = QImage(filename);
    m_IsUseDiffuseMap = true;
}

void Material::setDiffuseMap(const QImage &image)
{
    m_DiffuseMap = image;
    m_IsUseDiffuseMap = true;
}

bool Material::isUseDiffuseMap() const
{
    return m_IsUseDiffuseMap;
}

QImage Material::NormalMap() const
{
    return m_NormalMap;
}

void Material::setNormalMap(const QString &filename)
{
    if(!QFile(filename).exists()) {qDebug() << "File not found:" << filename; return; }

    m_NormalMap = QImage(filename);
    m_IsUseNormalMap = true;
}

void Material::setNormalMap(const QImage &image)
{
    m_NormalMap = image;
    m_IsUseNormalMap = true;
}

bool Material::isUseNormalMap() const
{
    return m_IsUseNormalMap;
}

