#include "object3d.h"

#include <QDir>

Object3D::~Object3D()
{
    for(auto o: m_Elements) delete o;
}

bool Object3D::load(const QString &filename)
{
    if(m_Elements.size()) {qCritical() << "EngineObject not empty!"; return false; }

    QFile objfile(filename);

    if(! objfile.exists()) { qCritical() << "File not exist:" << filename; return false; }
    if(! objfile.open(QFile::ReadOnly))  { qCritical() << "File not opened:" << filename; return false; }

    QTextStream input(&objfile);
    QVector<QVector3D> coords;
    QVector<QVector2D> texturcoords;
    QVector<QVector3D> normals;

    QVector<VertexData> vertexes;
    QVector<GLuint> indexes;
    QString mtlName;
    Object3DElement* object = nullptr;

    qDebug() << "Reading" << filename << "...";

    bool ok = true;
    while(!input.atEnd() && ok)
    {
        auto str = input.readLine(); if(str.isEmpty()) continue;
        auto strlist = str.split(' '); strlist.removeAll("");
        auto key = strlist.at(0).toLower();

        if (key == "#") { qDebug() << str; }
        else if(key == "mtllib")
        {
            if(strlist.size() > 1)
            {
                auto file = QFileInfo(filename).absolutePath() + QDir::separator() + strlist.at(1);
                if(QFile(file).exists()) ok = m_MatLibrary.load(file);
                else { qCritical() << "File not exists:" << file; ok = false; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key == "v")
        {
            if(strlist.size() > 3)
            {
                coords.append(QVector3D(strlist.at(1).toFloat(&ok),
                                        strlist.at(2).toFloat(&ok),
                                        strlist.at(3).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key == "vt")
        {
            if(strlist.size() > 2)
            {
                texturcoords.append(QVector2D(strlist.at(1).toFloat(&ok),
                                              strlist.at(2).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key == "vn")
        {
            if(strlist.size() == 4)
            {
                normals.append(QVector3D(strlist.at(1).toFloat(&ok),
                                         strlist.at(2).toFloat(&ok),
                                         strlist.at(3).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key == "f")
        {
            for(int i = 1; i < strlist.size(); i++)
            {
                auto v = strlist.at(i).split('/');
                if(v.size() == 3 && !v.at(1).isEmpty() && !v.at(2).isEmpty())
                {
                    vertexes.append(VertexData(coords.at(v.at(0).toInt(&ok, 10) - 1),
                                               texturcoords.at(v.at(1).toInt(&ok, 10) - 1),
                                               normals.at(v.at(2).toInt(&ok, 10) - 1)));
                    indexes.append(static_cast<GLuint>(indexes.size()));
                }
                else
                {
                    qCritical() << "Unsupported OBJ data format:" << strlist.at(i);
                    ok = false; break;
                }
            }
            if(!ok) { qCritical() << "Error at line (format):" << str; }
        }
        else if(key == "usemtl")
        {
            if(strlist.size() > 1)
            {
                if(object)
                {
                    calculateTBN(vertexes);
                    object->init(vertexes, indexes, m_MatLibrary.get(mtlName));
                }
                mtlName = strlist.at(1);
                add(object);

                object = new Object3DElement;
                vertexes.clear();
                indexes.clear();
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
    }

    objfile.close(); qDebug() <<  "... done" << filename;

    if(!ok) return false;

    if(object)
    {
        calculateTBN(vertexes);
        object->init(vertexes, indexes, m_MatLibrary.get(mtlName));
    }
    add(object);

    qDebug() << "Object is loaded successfully:" << m_Elements.size() << "elements";
    return true;
}

void Object3D::add(Object3DElement *obj)
{
    if(! obj) return;
    for(auto o: m_Elements) if (o == obj) return;
    m_Elements.append(obj);
}

Object3DElement *Object3D::get(int index)
{
    if(index > -1 && index < m_Elements.size()) return m_Elements.at(index);
    return nullptr;
}

void Object3D::calculateTBN(QVector<VertexData> &vertdata)
{        
    for(int i = 0; i < vertdata.size(); i += 3)
    {
        auto v1 = vertdata.at(i).position;
        auto v2 = vertdata.at(i + 1).position;
        auto v3 = vertdata.at(i + 2).position;

        auto uv1 = vertdata.at(i).textcoord;
        auto uv2 = vertdata.at(i + 1).textcoord;
        auto uv3 = vertdata.at(i + 2).textcoord;

        // схема вычисления T и B
        // deltaPos1 = deltaUV1.x * T + deltaUV1.y * B;
        // deltaPos2 = deltaUV2.x * T + deltaUV2.y * B;

        auto deltaPos1 = v2 - v1; auto deltaPos2 = v3 - v1;
        auto deltaUV1 = uv2 - uv1; auto deltaUV2 = uv3 - uv1;
        float r = 0.1f / (deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x());
        QVector3D tangent = (deltaPos1 * deltaUV2.y() - deltaPos2 * deltaUV1.y()) * r;
        QVector3D bitangent = (deltaPos2 * deltaUV1.x() - deltaPos1 * deltaUV2.x()) * r;

        vertdata[i].tangent = tangent;
        vertdata[i + 1].tangent = tangent;
        vertdata[i + 2].tangent = tangent;

        vertdata[i].bitangent = bitangent;
        vertdata[i + 1].bitangent = bitangent;
        vertdata[i + 2].bitangent = bitangent;
    }
}

void Object3D::rotate(const QQuaternion &r)
{
    for(auto o: m_Elements) o->rotate(r);
}

void Object3D::translate(const QVector3D &t)
{
    for(auto o: m_Elements) o->translate(t);
}

void Object3D::scale(const float &s)
{
    for(auto o: m_Elements) o->scale(s);
}

void Object3D::setGlobalTransform(const QMatrix4x4 &gt)
{
    for(auto o: m_Elements) o->setGlobalTransform(gt);
}

void Object3D::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    for(auto o: m_Elements) o->draw(program, functions);
}
