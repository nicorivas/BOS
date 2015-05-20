/* 
 * File:   InfiniteWall.h
 * Author: dducks
 *
 * Created on May 11, 2015, 3:03 AM
 */

#ifndef INFINITEWALL_H
#define	INFINITEWALL_H

#include <math/Vector.h>
#include <math/Line.h>
#include <math/Intersection.h>
#include <renderer/DisplayList.h>
#include <GL/glew.h>

#include <algorithm>
#include <vector>

#define CROSS_LIM  0.00000001

inline Line<3> planesToLine(Vector<3> n1, Vector<3> n2, double p1, double p2) {
    Vector<3> o;
    Vector<3> d;
    
    d = cross(n1, n2);
    
    double det =   n2[0] * (n1[1] * d[2] - n1[2] * d[1])
                 - n2[1] * (n1[0] * d[2] - n1[2] * d[0])
                 + n2[2] * (n1[0] * d[1] - n1[1] * d[0]);
    o = Vector<3>
        {(n1[1] * d[2] - n1[2] * d[1]) * p2
        -(n2[1] * d[2] - n2[2] * d[1]) * p1,
        -(n1[0] * d[2] - n1[2] * d[0]) * p2
        +(n2[0] * d[2] - n2[2] * d[0]) * p1,
        +(n1[0] * d[1] - n1[1] * d[0]) * p2
        -(n2[0] * d[1] - n2[1] * d[0]) * p1} / det;
        
    return {o,d};
}

inline Vector<3> intersection(Line<3> a, Line<3> b) {
    double tA = dot(cross(b.getDir(),a.getOrig() - b.getOrig()),cross(a.getDir(),b.getDir()) );
    double tB = dot(cross(a.getDir(),a.getOrig() - b.getOrig()),cross(a.getDir(),b.getDir()) );
    
    double d = cross(a.getDir(), b.getDir()).lengthSq();
    
    if (d < CROSS_LIM)
        return Vector<3>{std::numeric_limits<double>::infinity(),
                         std::numeric_limits<double>::infinity(),
                         std::numeric_limits<double>::infinity()};
                         
    Vector<3> A = a.getOrig() + a.getDir() * (tA / d);
    Vector<3> B = b.getOrig() + b.getDir() * (tB / d);
    
    if ((A-B).lengthSq() > CROSS_LIM)
        return Vector<3>{std::numeric_limits<double>::infinity(),
                         std::numeric_limits<double>::infinity(),
                         std::numeric_limits<double>::infinity()};
    
    return A;
    
}

class InfiniteWall : public DisplayList
{
    
    Vector<3> position;
    Vector<3> normal;
public:
    InfiniteWall(Vector<3> pos, Vector<3> nor) : position(pos), normal(nor) { }

    void compile(Vector<3> min, Vector<3> max) {
        double pWall = dot(normal, position);
        
        std::vector<Line<3>> lines;
        
        Line<3> line(Vector<3>{0,0,0},Vector<3>{0,0,0});
        
        if (              cross(normal, { 1, 0, 0}).lengthSq() > CROSS_LIM) {
            line = planesToLine(normal, { 1, 0, 0}, pWall, max[0]);
            lines.push_back(line);
        }
        if (              cross(normal, {-1, 0, 0}).lengthSq() > CROSS_LIM) {
            line = planesToLine(normal, {-1, 0, 0}, pWall, min[0]);
            lines.push_back(line);
        }
        if (              cross(normal, { 0, 1, 0}).lengthSq() > CROSS_LIM) {
            line = planesToLine(normal, { 0, 1, 0}, pWall, max[1]);
            lines.push_back(line);
        }
        if (              cross(normal, { 0,-1, 0}).lengthSq() > CROSS_LIM) {
            line = planesToLine(normal, { 0,-1, 0}, pWall, min[1]);
            lines.push_back(line);
        }
        if (              cross(normal, { 0, 0, 1}).lengthSq() > CROSS_LIM) {
            line = planesToLine(normal, { 0, 0, 1}, pWall, max[2]);
            lines.push_back(line);
        }
        if (              cross(normal, { 0, 0,-1}).lengthSq() > CROSS_LIM) {
            line = planesToLine(normal, { 0, 0,-1}, pWall, min[2]);
            lines.push_back(line);
        }
        
        Vector<3> mx = max + Vector<3>{CROSS_LIM, CROSS_LIM, CROSS_LIM};
        Vector<3> mn = min - Vector<3>{CROSS_LIM, CROSS_LIM, CROSS_LIM};
        
        for (std::size_t i = 0; i < lines.size(); i++) {
            for (std::size_t j = 0; j < i; j++) {
                Vector<3> v = intersection(lines[i], lines[j]);
                if (v[0] > mx[0] || v[0] < mn[0] ||
                    v[1] > mx[1] || v[1] < mn[1] ||
                    v[2] > mx[2] || v[2] < mn[2])
                    continue;
                vertices.emplace_back(v,Vector<3>{0,0,0},Vector<3>{0.2,0,0});
            }
        }
        
        if (vertices.size() <= 2) {
            std::cerr << vertices.size() << " vertices! (Need at least 3)" << std::endl;
            return;
        }
        
        Vector<3> o = vertices[0].position;
        std::sort(std::next(vertices.begin(),1), vertices.end(), [this,o](GLColoredVertex a, GLColoredVertex b) -> bool {
            Vector<3> ao = a.position - o;
            Vector<3> bo = b.position - o;
            double thetaA = dot(normal, ao);
            double thetaB = dot(normal, bo);

            return (thetaA / (ao.lengthSq())) < (thetaB / (bo.lengthSq()));

        });
        /*Vector<3> normal = cross(vertices[2].position - vertices[1].position,
                                 vertices[0].position - vertices[1].position).normalized();
        */

        for (GLColoredVertex& v : vertices)
            v.normal = normal;
        
        for (std::size_t idx = 2; idx < vertices.size(); idx++) {
            indices.emplace_back(0, idx-1, idx);
        }
        
        
        DisplayList::compile();
    }
    
};

class IntersectionOfWalls : public DisplayList
{
public:
    IntersectionOfWalls(Vector<3> pos) : position(pos) { }

    virtual void compile(Vector<3> min, Vector<3> max) {
        find(min,max);
        DisplayList::compile();
    }
    
    void add(Vector<3> pos, Vector<3> norm) {
        positions.push_back(pos);
        normals.push_back(norm);
    }
        
protected:
    Vector<3> position;
    
    std::vector<Vector<3>> positions;
    std::vector<Vector<3>> normals;
    
    
    void find(Vector<3> min, Vector<3> max) {
        for (int i = 0; i < positions.size(); i++) {

            std::vector<GLColoredVertex> tempVertices;
            
            Vector<3> normal = normals[i];
            Vector<3> position = positions[i];
            
            double pWall = dot(normal, position);

            std::vector<Line<3>> lines;

            Line<3> line(Vector<3>{0,0,0},Vector<3>{0,0,0});

            if (              cross(normal, { 1, 0, 0}).lengthSq() > CROSS_LIM) {
                line = planesToLine(normal, { 1, 0, 0}, pWall, max[0]);
                lines.push_back(line);
            }
            if (              cross(normal, {-1, 0, 0}).lengthSq() > CROSS_LIM) {
                line = planesToLine(normal, {-1, 0, 0}, pWall, min[0]);
                lines.push_back(line);
            }
            if (              cross(normal, { 0, 1, 0}).lengthSq() > CROSS_LIM) {
                line = planesToLine(normal, { 0, 1, 0}, pWall, max[1]);
                lines.push_back(line);
            }
            if (              cross(normal, { 0,-1, 0}).lengthSq() > CROSS_LIM) {
                line = planesToLine(normal, { 0,-1, 0}, pWall, min[1]);
                lines.push_back(line);
            }
            if (              cross(normal, { 0, 0, 1}).lengthSq() > CROSS_LIM) {
                line = planesToLine(normal, { 0, 0, 1}, pWall, max[2]);
                lines.push_back(line);
            }
            if (              cross(normal, { 0, 0,-1}).lengthSq() > CROSS_LIM) {
                line = planesToLine(normal, { 0, 0,-1}, pWall, min[2]);
                lines.push_back(line);
            }
            
            for (int j = 0; j < positions.size(); j++) {
                if (i == j)
                    continue;
                
                if (cross(normal, normals[j]).lengthSq() > CROSS_LIM) {
                    line = planesToLine(normal, normals[j], pWall, dot(normals[j],positions[j]));
                    lines.push_back(line);
                }
            }

            Vector<3> mx = max + Vector<3>{CROSS_LIM, CROSS_LIM, CROSS_LIM};
            Vector<3> mn = min - Vector<3>{CROSS_LIM, CROSS_LIM, CROSS_LIM};

            for (std::size_t k = 0; k < lines.size(); k++) {
                for (std::size_t l = 0; l < k; l++) {
                    Vector<3> v = intersection(lines[k], lines[l]);
                    if (v[0] > mx[0] || v[0] < mn[0] ||
                        v[1] > mx[1] || v[1] < mn[1] ||
                        v[2] > mx[2] || v[2] < mn[2])
                        continue;
                    
                    bool ignore = false;
                    for (int j = 0; j < positions.size(); j++) {
                        if (i == j)
                            continue;
                        
                        if (dot(positions[j]-v,normals[j]) < -CROSS_LIM) {
                            ignore = true;
                            break;
                        }
                    }
                    if (ignore)
                        continue;
                
                    tempVertices.emplace_back(v,normal,Vector<3>{0.2,0,0});
                }
            }

            if (tempVertices.size() <= 2) {
                std::cerr << tempVertices.size() << " vertices! (Need at least 3)" << std::endl;
                return;
            }

            Vector<3> o = tempVertices[0].position;
            std::sort(std::next(tempVertices.begin(),1), tempVertices.end(), [this,i,o](GLColoredVertex a, GLColoredVertex b) -> bool {
                Vector<3> ao = a.position - o;
                Vector<3> bo = b.position - o;
                double thetaA = dot(normals[i], ao);
                double thetaB = dot(normals[i], bo);

                return (thetaA / (ao.lengthSq())) < (thetaB / (bo.lengthSq()));

            });
            
            for (std::size_t idx = 2; idx < tempVertices.size(); idx++) {
                indices.emplace_back(vertices.size(), vertices.size() + idx-1, vertices.size() + idx);
            }
            
            for (const GLColoredVertex& vt : tempVertices)
                vertices.push_back(vt);
            
        }
        /*Vector<3> normal = cross(vertices[2].position - vertices[1].position,
                                 vertices[0].position - vertices[1].position).normalized();
        */

            
        
        
    }
    
};

Matrix<3,3> rotFromAxisAndAngle(Vector<3> axis, double angle) {
    Matrix<3,3> rVal;
    double cosT = std::cos(angle);
    double sinT = std::sin(angle);
    double mcosT = 1 - cosT;
    
    rVal(0,0) = cosT  + axis[0]*axis[0]*mcosT;    
    rVal(0,1) = axis[0]*axis[1]*mcosT - axis[2]*sinT;
    rVal(0,2) = axis[0]*axis[2]*mcosT + axis[1]*sinT;
    
    rVal(1,0) = axis[1]*axis[0]*mcosT + axis[2]*sinT;
    rVal(1,1) = cosT  + axis[1]*axis[1]*mcosT;    
    rVal(1,2) = axis[1]*axis[2]*mcosT - axis[0]*sinT;
            
    rVal(2,0) = axis[2]*axis[0]*mcosT - axis[1]*sinT;
    rVal(2,1) = axis[2]*axis[1]*mcosT + axis[0]*sinT;
    rVal(2,2) = cosT  + axis[2]*axis[2]*mcosT;
    
    return rVal;
}

class AxisymmetricIntersectionOfWalls : public IntersectionOfWalls
{
    
    Vector<3> orientation;
public:
    AxisymmetricIntersectionOfWalls(Vector<3> pos, Vector<3> ori)
         : IntersectionOfWalls(pos), orientation(ori.normalized()) { }

    void compile(Vector<3> min, Vector<3> max) override {
        find({0, 0, min[2]},{1000, 2*M_PI, max[2]});
        
        refine(4);
        
        Vector<3> normAxis = {1,0,0};
        if (cross(orientation, normAxis).lengthSq() < CROSS_LIM) {
            normAxis = {0,1,0};
        }
        normAxis = cross(normAxis,orientation).normalized();
        
        for (GLColoredVertex& vert : vertices) {
            Vector<3> vp = vert.position;
            Vector<3> axis = orientation * vp[2];
            Vector<3> tang = rotFromAxisAndAngle(orientation, vp[1]) * (normAxis * vp[0]);
            vert.position = axis + tang + position;            
        }
        
        DisplayList::compile();
    }
    
};


#endif	/* INFINITEWALL_H */

