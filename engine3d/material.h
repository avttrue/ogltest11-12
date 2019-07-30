#ifndef MATERIAL_H
#define MATERIAL_H

#include <QImage>
#include <QString>
#include <QVector3D>

class Material
{
public:
    Material();

    QString Name();
    void setName(const QString &n);

    QVector3D DiffuseColor() const;
    void setDiffuseColor(const QVector3D &dc);

    QVector3D AmbienceColor() const;
    void setAmbienceColor(const QVector3D &ac);

    QVector3D SpecularColor() const;
    void setSpecularColor(const QVector3D &sc);

    float Shines() const;
    void setShines(const float &s);

    QImage DiffuseMap() const;
    void setDiffuseMap(const QString &filename);
    void setDiffuseMap(const QImage &image);
    bool isUseDiffuseMap() const;

    QImage NormalMap() const;
    void setNormalMap(const QString &filename);
    void setNormalMap(const QImage &image);
    bool isUseNormalMap() const;

private:
    QString m_Name;
    QVector3D m_DiffuseColor;
    QVector3D m_AmbienceColor;
    QVector3D m_SpecularColor;
    float m_Shines;
    QImage m_DiffuseMap;
    QImage m_NormalMap;
    bool m_IsUseDiffuseMap;
    bool m_IsUseNormalMap;
};

#endif // MATERIAL_H
