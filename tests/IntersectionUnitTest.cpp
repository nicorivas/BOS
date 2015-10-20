#include <math/Line.h>
#include <math/Plane.h>
#include <math/Intersection.h>

#include <vector>
#include <random>
#include <iostream>
#include <array>
#include <fstream>

/*
 * We want to extensively check the intersection routine.
 * This involves computing an intersection time (parameter of lines)
 * And then evaluating if at this time there is an actual intersection.
 * Numerical errors may prevent this, and we want to characterize them.
 */
int main(int argc, char** argv) {
    
    std::mt19937_64 gen(1);

    std::ofstream outfile;
    outfile.open ("dist.dat", std::ofstream::out);
    
    std::array<int, 10> distribution;
    
    double precision = 1.0;
    
    double size;    // Domain size in # of particle radii
    int errorCount; // Number of fails
    int i;          // Index of cycle (every cycle is a random set)
    int n = 0;      // precision = 2^n
    for (size=4.0; size <= pow(2.0,10); size*=2.0) {
        for (n=0; n <= 10; n++) {
            errorCount = 0;
            precision = pow(2,n);
            std::cout << "size=" << size << " precision=" << precision << std::endl;
            for (i = 0; i < 100; i++) {
                std::uniform_real_distribution<double> randDir(-1,1);
                std::uniform_real_distribution<double> randDist(0,1.0);
                double radii = randDist(gen);

                // The box
                randDist.param(std::uniform_real_distribution<double>::param_type{radii*2,radii*size});
                std::vector<Plane<3> > box;
                double plx = randDist(gen);
                double ply = randDist(gen);
                double plz = randDist(gen);
                box.push_back(Plane<3>({0.0, 0.0, 0.0},{ 1.0, 0.0, 0.0}));
                box.push_back(Plane<3>({0.0, 0.0, 0.0},{ 0.0, 1.0, 0.0}));
                box.push_back(Plane<3>({0.0, 0.0, 0.0},{ 0.0, 0.0, 1.0}));
                box.push_back(Plane<3>({plx, ply, plz},{-1.0, 0.0, 0.0}));
                box.push_back(Plane<3>({plx, ply, plz},{ 0.0,-1.0, 0.0}));
                box.push_back(Plane<3>({plx, ply, plz},{ 0.0, 0.0,-1.0}));

                // The point
                std::uniform_real_distribution<double> randX(radii, plx-radii);
                std::uniform_real_distribution<double> randY(radii, ply-radii);
                std::uniform_real_distribution<double> randZ(radii, plz-radii);
                Line<3> line = Line<3>(
                    {randX(gen), randY(gen), randZ(gen)},
                    {randDir(gen), randDir(gen), randDir(gen)});

                int j;
                int good = 0;
                for (j = 0; j < 6; j++) {
                    double t = intersection<3>(line, box[j], radii, 0.0);
                    Vector<3> pointOfIntersection = line(t);
                    Vector<3> sizeDirectional({radii,radii,radii});
                    sizeDirectional = ewise(sizeDirectional, -box[j].getNormal());
                    Vector<3> check = (pointOfIntersection + sizeDirectional);    

                    //std::cout << "line=" << line << std::endl;
                    //std::cout << "plane=" << box[j] << std::endl;
                    //std::cout << "intersection time=" << t << std::endl;
                    //std::cout << "sizeDirectional=" << sizeDirectional << std::endl;
                    //std::cout << check << std::endl;

                    if (std::abs(check[0]) == std::numeric_limits<double>::infinity() ||
                        std::abs(check[1]) == std::numeric_limits<double>::infinity() ||
                        std::abs(check[2]) == std::numeric_limits<double>::infinity()) {
                        // We use std::abs because it can also be -infty
                        continue;
                    }

                    if (check[0] <= std::numeric_limits<double>::epsilon()*precision || 
                            std::abs(check[0] - plx) <= std::numeric_limits<double>::epsilon()*precision) {
                        good++;
                        //std::cout << "Intersection check OK (x="<<plx<<"||0) = ";
                        //std::cout << (pointOfIntersection + sizeDirectional) << std::endl;
                    }else if (check[1] <= std::numeric_limits<double>::epsilon()*precision || 
                            std::abs(check[1] - ply) <= std::numeric_limits<double>::epsilon()*precision) {
                        good++;
                        //std::cout << "Intersection check OK (y="<<ply<<"||0) = ";
                        //std::cout << (pointOfIntersection + sizeDirectional) << std::endl;
                    }else if (check[2] <= std::numeric_limits<double>::epsilon()*precision || 
                            std::abs(check[2] - plz) <= std::numeric_limits<double>::epsilon()*precision) {
                        good++;
                        //std::cout << "Intersection check OK (z="<<plz<<"||0) = ";
                        //std::cout << (pointOfIntersection + sizeDirectional) << std::endl;
                    }else{
                        // Solution should either be infty or coinciding with the
                        // limits (within a precision), so if we arrive here
                        // something is wrong
                        /*
                        std::cout << "1. Something is terribly wrong, try again:" << std::endl;    
                        std::cout << "line=" << line << std::endl;
                        std::cout << "plane=" << box[j] << std::endl;
                        std::cout << "intersection time=" << t << std::endl;
                        std::cout << "sizeDirectional=" << sizeDirectional << std::endl;
                        std::cout << check << std::endl;
                        */
                        /*
                        Vector<3> lineInPlaneSpace = line.getOrig() - box[j].getPoint();
                        double a = dot(lineInPlaneSpace, box[j].getNormal());
                        double b = dot(line.getDir(), box[j].getNormal());
                        double c = (a - radii);
                        double d = c/b;
                        std::cout << "lx=" << plx << " ly=" << ply << " lz=" << plz << std::endl;
                        std::cout << "a=" << a << " b=" << b << " c=" << c << " d=" << d << std::endl;
                        */
                        errorCount++;
                    }
                }
                if (good != 3) {
                    //std::cout << "2. Something is terribly wrong, try again" << std::endl;
                    //exit(0);
                } else {
                    //std::cout << "Wuha #"<<i<<"!" << std::endl;
                }
            }
            distribution[n] = errorCount;
        }
        for (n = 0; n <= 10; n++) {
            std::cout << distribution[n] << std::endl;
            outfile << size << " " << n << " " << distribution[n] << std::endl;
            distribution[n] = 0;
        }
    }
    outfile.close();
    return 0;
}