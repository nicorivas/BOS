#include <math/Vector.h>
#include <math/Matrix.h>

#include <iostream>

int main(int argc, char** argv) {
    Matrix<4,4> ident;
    std::cout << ident << std::endl << std::endl;
    
    std::cout << ident + ident << std::endl << std::endl;
    
    Matrix<3,4> q = Matrix<3,4>() + Matrix<3,4>();
    std::cout << q << std::endl << std::endl;
    
    Matrix<4,2> a;
    Matrix<2,3> b;
    a(0,0) = 1; a(0,1) = 2;
    a(1,0) = 3; a(1,1) = 4;
    a(2,0) = 5; a(2,1) = 6;
    a(3,0) = 7; a(3,1) = 8;
    
    std::cout << a << std::endl << std::endl;
    
    b(0,0) = 1; b(0,1) = 2; b(0,2) = 3;
    b(1,0) = 4; b(1,1) = 5; b(1,2) = 6;
    
    std::cout << a << std::endl << std::endl;
    std::cout << b << std::endl << std::endl;
    std::cout << a*b << std::endl << std::endl;
    
    Matrix<3,3> ident3;
    Vector<3> xyz = {2,3,4};
    std::cout << ident3 << std::endl << std::endl;
    std::cout << xyz << std::endl << std::endl;
    std::cout << ident3 * xyz << std::endl << std::endl;
        
    return 0;
}
