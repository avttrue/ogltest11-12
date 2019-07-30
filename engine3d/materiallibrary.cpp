#include "materiallibrary.h"
#include "material.h"

#include <QDebug>
#include <QDir>

bool MaterialLibrary::load(const QString &filename)
{
    QFile mtlfile(filename);

    if(! mtlfile.exists()) { qCritical() << "File not exist:" << filename; return false; }
    if(! mtlfile.open(QFile::ReadOnly))  { qCritical() << "File not opened:" << filename; return false; }

    for(auto o: m_Materials) delete o;
    m_Materials.clear();

    auto apath = QFileInfo(filename).absolutePath() + QDir::separator();
    Material* newMtl = nullptr;

    QTextStream input(&mtlfile);
    qDebug() << "Reading" << filename << "...";

    bool ok = true;
    while(!input.atEnd())
    {
        auto str = input.readLine(); if(str.isEmpty()) continue;
        auto strlist = str.split(' '); strlist.removeAll("");
        auto key = strlist.at(0).toLower();

        if (key == "#") { qDebug() << str; }
        else if(key == "newmtl")
        {
            add(newMtl);
            newMtl = new Material();
            if(strlist.size() > 1) newMtl->setName(strlist.at(1));
            else
            {
                newMtl->setName(QString::number(m_Materials.size()));
                qCritical() << "Error at line (material name not present):" << str;
            }
        }
        else if(key == "ns")
        {
            if(!newMtl) { qCritical() << "Material not created"; continue; }
            if(strlist.size() > 1) newMtl->setShines(strlist.at(1).toFloat(&ok));
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key == "ka")
        {
            if(!newMtl) { qCritical() << "Material not created"; continue; }
            if(strlist.size() > 3)
            {
                newMtl->setAmbienceColor(QVector3D(strlist.at(1).toFloat(&ok),
                                                   strlist.at(2).toFloat(&ok),
                                                   strlist.at(3).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key == "kd")
        {
            if(!newMtl) { qCritical() << "Material not created"; continue; }
            if(strlist.size() > 3)
            {
                newMtl->setDiffuseColor(QVector3D(strlist.at(1).toFloat(&ok),
                                                  strlist.at(2).toFloat(&ok),
                                                  strlist.at(3).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key == "ks")
        {
            if(!newMtl) { qCritical() << "Material not created"; continue; }
            if(strlist.size() > 3)
            {
                newMtl->setSpecularColor(QVector3D(strlist.at(1).toFloat(&ok),
                                                   strlist.at(2).toFloat(&ok),
                                                   strlist.at(3).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key == "map_kd")
        {
            if(!newMtl) { qCritical() << "Material not created"; continue; }
            if(strlist.size() > 1)
            {
                auto file = apath + strlist.at(1);
                if(QFile(file).exists()) newMtl->setDiffuseMap(file);
                else { qCritical() << "File not exists:" << file; ok = false; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key == "map_bump")
        {
            if(!newMtl) { qCritical() << "Material not created"; continue; }
            if(strlist.size() > 1)
            {
                auto file = apath + strlist.at(1);
                if(QFile(file).exists()) newMtl->setNormalMap(file);
                else { qCritical() << "File not exists:" << file; ok = false; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }

        add(newMtl);
    }

    mtlfile.close();
    qDebug() << "... done" << filename;
    return ok;
}

void MaterialLibrary::add(Material *m)
{
    if(!m) return;
    for(auto o: m_Materials) if(o == m) return;
    m_Materials.append(m);
    qDebug() << "Material count:" << m_Materials.count();
}

Material *MaterialLibrary::get(int index)
{
    if(index > -1 && index < m_Materials.size()) return m_Materials.at(index);
    return nullptr;
}

Material *MaterialLibrary::get(const QString &name)
{
    for(auto o: m_Materials) if(o->Name() == name) return o;
    return nullptr;
}

int MaterialLibrary::count()
{
    return m_Materials.size();
}
