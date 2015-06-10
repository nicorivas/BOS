/* 
 * File:   main.cpp
 * Author: dducks
 *
 * Created on April 20, 2015, 6:41 PM
 */

#include <physics/Simulation.h>
#include <physics/Particle.h>
#include <physics/Wall.h>

#include <math/Intersection.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <renderer/Shader.h>
#include <renderer/Camera.h>
#include <renderer/GLBall.h>

#include <chrono>
#include <thread>

#include <random>

#include <data/shaders/shader_fragment.h>
#include <data/shaders/shader_vertexED.h>
using namespace std;

static Camera cam;
static bool capture = false;
static Vector<3> movedir;
static float moveSpeed = 0.1;


static void resizeFunc(GLFWwindow * win, int w, int h) {
    cam.setProjection((float) M_PI_2, 1000.f, 0.001f, w, h);
}

static void keyFunc(GLFWwindow * win, int key, int scan, int action, int mod) {
    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_Q:
                capture = !capture;
                movedir = {0,0,0};
                if (capture) {
                    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    glfwSetCursorPos(win, 0, 0);
                } else {
                    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                break;
            default:
                break;
        }
    }

    if (capture && action != GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_W:
                movedir -= Vector<3>{0,0,1} * ((action != GLFW_PRESS) ? moveSpeed : -moveSpeed);
                break;
            case GLFW_KEY_S:
                movedir += Vector<3>{0,0,1}* ((action != GLFW_PRESS) ? moveSpeed : -moveSpeed);
                break;
            case GLFW_KEY_A:
                movedir -= Vector<3>{1,0,0} * ((action != GLFW_PRESS) ? moveSpeed : -moveSpeed);
                break;
            case GLFW_KEY_D:
                movedir += Vector<3>{1,0,0} * ((action != GLFW_PRESS) ? moveSpeed : -moveSpeed);
                break;
            case GLFW_KEY_LEFT_CONTROL:
                movedir -= Vector<3>{0,1,0} * ((action != GLFW_PRESS) ? moveSpeed : -moveSpeed);
                break;
            case GLFW_KEY_SPACE:
                movedir += Vector<3>{0,1,0} * ((action != GLFW_PRESS) ? moveSpeed : -moveSpeed);
                break;
            case GLFW_KEY_LEFT_SHIFT:
            {
                float factor = action != GLFW_PRESS ? 0.1f : 10.0f;
                moveSpeed *= factor;
                movedir *= factor;
            }
            default:
                break;
        };
    }
}

static void cursorFunc(GLFWwindow * win, double x, double y) {
    if (capture) {
        cam.rotate((float) y / 400, (float) x / 400);
        glfwSetCursorPos(win, 0, 0);
    }
}

static void vis() {
    /*
    //Window pointer
    GLFWwindow* win;

    //We check for errors every frame!
    GLenum error;

    //Init the GLFW library
    if (!glfwInit())
    {
        std::cerr << "GLFW has not initialised properly. Quitting" << std::endl;
        return -1;
    }
    
    
    //Setup OpenGL 3.3 strict profile
    //4x AA
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
 
       
    //Make a nice window etc
    win = glfwCreateWindow(800, 600, "Balls of Steel", nullptr, nullptr);
    if (!win)
    {
        std::cerr << "Window creation has failed." << std::endl;
        glfwTerminate();
        return -2;
    }
    
    //We need a OpenGL context
    glfwMakeContextCurrent(win);

    //We need experimental additions
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        std::cerr << "Glew failed." << std::endl;
        return 1;
    }
    glGetError();

    //Enable depth testing
    glEnable(GL_DEPTH_TEST);
    //And set our clear color to something blue
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    ShaderProgram shader;
    shader.link();
    shader.addShader(ShaderType::Vertex, vertexEDSRC, sizeof(vertexEDSRC));
    shader.addShader(ShaderType::Fragment, fragmentSRC, sizeof(fragmentSRC));

    shader.use();
    if (!shader) {
        std::cerr << "Shaders not happy :(" << std::endl;
        return -1;
    }
    
    cam.setShader(shader);
    GLBall ball;
    
    glfwSetWindowSizeCallback(win, resizeFunc);
    resizeFunc(win, 800, 600);
    //Setup some basic callbacks
    glfwSetKeyCallback(win, keyFunc);
    glfwSetCursorPosCallback(win, cursorFunc);
    
    ball.subdivide(3);
    ball.compile(true);
    */
      
    /*
    cam.move({1.0, 1.0, -10});
    int funcNum = sim.addFunction([&](Simulation<3>& sim) {
        //std::cout << "CALLBACK!" << std::endl;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        cam.move(movedir);

        cam.render();
        //std::cout << sim.getLastEvenTime()<< std::endl;
        //sim.synchronise();
        ball.renderInstanced(sim.getParticles());
        sim.queueFunction(0,sim.getLastEvenTime() + 10.0);
        
        glfwSwapBuffers(win);
        glfwPollEvents();
        
        if (glfwWindowShouldClose(win)) {
            glfwDestroyWindow(win);
            glfwTerminate();
            std::exit(0);
        }
    });
    */
}

/*
 * 
 */
int main(int argc, char** argv) {
    
    std::mt19937_64 gen(0);
    std::uniform_real_distribution<double> randDist(-1,1);
    
    Simulation<3> sim(100000.0, 10000.0);
    
    double spacing = 0.03;
    double radius = 0.5;
    int n = 0;
    int i = 0, j = 0, k = 0;
    double px, py, pz;
    int nmax = 4;
    double lx = 5.0;
    double ly = 5.0;
    double lz = 5.0;
    
    sim.addWall({{ 0, 0, 0},{ 1, 0, 0}});
    sim.addWall({{ 0, 0, 0},{ 0, 1, 0}});
    sim.addWall({{ 0, 0, 0},{ 0, 0, 1}});
    sim.addWall({{lx,ly,lz},{-1, 0, 0}});
    sim.addWall({{lx,ly,lz},{ 0,-1, 0}});
    sim.addWall({{lx,ly,lz},{ 0, 0,-1}});
    
    while (n < nmax) {
        px = radius+spacing+2*(radius+spacing)*i;
        py = radius+spacing+2*(radius+spacing)*j;
        pz = radius+spacing+2*(radius+spacing)*k;
        std::cout << px << " " << py << " " << pz << std::endl;
        i++;
        if (px+radius+spacing > lx) {
            i = 0;
            j++;
            continue;
        }
        if (py+radius+spacing > ly) {
            i = 0;
            j = 0;
            k++;
            continue;
        }
        if (pz+radius+spacing > lz) {
            std::cerr << "Particle don't fit inside walls" << std::endl;
            exit(1);
        }
        sim.addParticle({{px, py, 0.75}, {randDist(gen), randDist(gen), 0.0}, radius, 0});
        n++;
        //sim.addParticle({n, {2.5+randDist(gen), 2.5, 2.5+randDist(gen)}, {randDist(gen), 0.0, randDist(gen)}, 0.5, 0});
        //sim.addParticle({n, {2.5, 2.5, 0.75}, {0.0, 0.0, 1.0}, 0.5, 0});
    }
    
    std::cout << "Total number of particles=" << sim.getParticles().size() << std::endl;
    
    //sim.addParticle({{3.2, 2.5, 2.5}, {-0.4, 0.0, 0.0}, 0.5, 0});
    //sim.addParticle({{3.2, 3.8, 2.5}, { 0.0,-2.0, 0.0}, 0.5, 0});
    //sim.addParticle({{1.9, 2.5, 2.5}, {+0.5, 0.0, 0.0}, 0.5, 0});
    
    sim.createGrid({0.0,0.0,0.0},{lx,ly,lz},{radius*2.0,radius*2.0,radius*2.0});
    sim.run();

    return 0;
}

