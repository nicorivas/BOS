#include "Interaction.h"

void particleElastic(Particle<3>& pA, Particle<3>& pB, Data<3>& data)
{
    Vector<3> d = pA.getPosition() - pB.getPosition();
    Vector<3> v = pA.getVelocity() - pB.getVelocity();
    d /= sqrt(dot(d, d)); //normalise
    pA.setVelocity(pA.getVelocity() - d * dot(v, d));
    pB.setVelocity(pB.getVelocity() + d * dot(v, d));
}

void particleInelastic(Particle<3>& pA, Particle<3>& pB, Data<3>& data)
{
    double massA = data.particleTypes[pA.getType()].mass;
    double massB = data.particleTypes[pB.getType()].mass;
    double r = 0.9;
    double reducedMass = massA*massB/(massA + massB);
    
    Vector<3> n = pB.getPosition() - pA.getPosition();
    Vector<3> rv = pB.getVelocity() - pA.getVelocity();
    n /= sqrt(dot(n, n)); //normalise
    double vn = dot(rv, n);
    double pn = (1+r)/2.0;
    double qn = reducedMass * 2.0 * pn * vn;
    Vector<3> q = qn*n;
    pA.setVelocity(pA.getVelocity() + q/massA);
    pB.setVelocity(pB.getVelocity() - q/massB);
    /*
    dx = PosX[disco2]-PosX[disco1];
    dy = PosY[disco2]-PosY[disco1];
    dz = PosZ[disco2]-PosZ[disco1];
    dr = sqrt(dx*dx+dy*dy+dz*dz);
    norma=1.0/dr;
    *nx = dx*norma;
    *ny = dy*norma;
    *nz = dz*norma;
    vn = (VelX[disco2]-VelX[disco1])*nx +(VelY[disco2]-VelY[disco1])*ny +(VelZ[disco2]-VelZ[disco1])*nz;
    qn = reducedMass*2.0*pn*vn;
    qx = qn*nx;
    qy = qn*ny;
    qz = qn*nz;
    VelX[disco1] += qx/getMass(disco1);
    VelY[disco1] += qy/getMass(disco1);
    VelZ[disco1] += qz/getMass(disco1);
    VelX[disco2] -= qx/getMass(disco2);
    VelY[disco2] -= qy/getMass(disco2);
    VelZ[disco2] -= qz/getMass(disco2);
    */
}

void wallElastic(Particle<3>& p, Wall<3>& w)
{
    p.setVelocity(p.getVelocity() - 2 * dot(p.getVelocity(), w.getNormal()) * w.getNormal());
}


