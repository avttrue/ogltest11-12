#ifndef MATERIALLIBRARY_H
#define MATERIALLIBRARY_H

#include <QVector>

class Material;

class MaterialLibrary
{
public:
    MaterialLibrary() {}
    bool load(const QString &filename);
    void add(Material* m);
    Material* get(int index);
    Material* get(const QString &name);
    int count();

private:
    QVector<Material*> m_Materials;
};

#endif // MATERIALLIBRARY_H
