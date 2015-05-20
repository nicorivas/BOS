/* 
 * File:   main.cpp
 * Author: dducks
 *
 * Created on April 20, 2015, 6:41 PM
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <renderer/Shader.h>
#include <renderer/Camera.h>
#include <renderer/GLBall.h>
#include <renderer/DisplayList.h>
#include <renderer/GLFileWriter.h>

#include <hg/walls/InfiniteWall.h>

#include <chrono>
#include <thread>

#include <random>
#include <map>

#include <data/shaders/shader_fragment.h>
#include <data/shaders/shader_fragmentW.h>
#include <data/shaders/shader_vertex.h>
#include <data/shaders/shader_vertexW.h>

using namespace std;

static Camera cam;
static bool capture = false;
static Vector<3> movedir;
static float moveSpeed = 0.1;
static bool running = false;
#ifdef ENABLE_VIDEO
static bool video = false;
static GLOffscreenBuffer* buffer = nullptr;
static GLVideoCodec* vcodec = nullptr;
#endif


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
            case GLFW_KEY_TAB:
                running = !running;
                break;
#ifdef ENABLE_VIDEO
            case GLFW_KEY_V:
                if (!video) {
                    buffer = new GLOffscreenBuffer((float) M_PI_2, 1000.f, 0.001f, 1920, 1080);
                    vcodec = new GLVideoCodec("test.h264", *buffer, 0);
                } else {
                    delete vcodec;
                    delete buffer;
                    vcodec = nullptr;
                    buffer = nullptr;
                }
                video = !video;
                break;
#endif
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

/*
 * 
 */
int main(int argc, char** argv) {
    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename"
                << "\nWhere filename is the prefix of a MercuryData file" << std::endl;
        return 1;
    }

    std::vector<DisplayList*> walls;
    
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
    win = glfwCreateWindow(800, 600, "hgGUI", nullptr, nullptr);
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
    glClearColor(0.0f, 0.247f, 0.568f, 0.0f);

    ShaderProgram shader;
    shader.addShader(ShaderType::Vertex, vertexSRC, sizeof(vertexSRC));
    shader.addShader(ShaderType::Fragment, fragmentSRC, sizeof(fragmentSRC));
    shader.link();
    shader.use();
    
    ShaderProgram shaderW;
    shaderW.addShader(ShaderType::Vertex, vertexWSRC, sizeof(vertexWSRC));
    shaderW.addShader(ShaderType::Fragment, fragmentWSRC, sizeof(fragmentWSRC));
    shaderW.link();
    shaderW.use();
    
    shader.use();
    
    if (!shader || !shaderW) {
        std::cerr << "Shaders not happy :(" << std::endl;
        return -1;
    }
    
    cam.setShader(shaderW);
    GLBall ball;
    
    glfwSetWindowSizeCallback(win, resizeFunc);
    resizeFunc(win, 800, 600);
    //Setup some basic callbacks
    glfwSetKeyCallback(win, keyFunc);
    glfwSetCursorPosCallback(win, cursorFunc);
    
    ball.subdivide(2);
    ball.compile(true,false);
    
    MercuryDataFile file(argv[1]);
    auto tsIt = file.begin<3>();
    auto tsEnd = file.end<3>();
    ++tsIt;
    if (!file.isMercuryDataFile<3>()) {
        std::cerr << argv[1] << " does not appear to be a mercury data file."
                << "\n please check the parameters and try again." << std::endl;
        return 2;
    }
    
    std::cout << "Particles: " << (*tsIt).getNumberOfParticles() << std::endl;
    

    
    
    InfiniteWall wall{{0, 0.33, 0}, {0, -1, 0}};
    wall.compile((*tsIt).getMin(),(*tsIt).getMax());
    
    Vector<3> origin = ((*tsIt).getMin() + (*tsIt).getMax()) / 2;
    cam.move(-(origin + Vector<3>{0,origin[1] * 5,0}));
    moveSpeed *= std::sqrt(((*tsIt).getMax() - (*tsIt).getMin()).lengthSq());

    cam.move(Vector<3>{0, 100, 0});
    
   // cam.rotate(M_PI_2, M_PI);
    
    /*IntersectionOfWalls wall2({0,0,0});
    wall2.add({5,5,5},{0,1,0});
    wall2.add({5,5,5},{-1,0,0}); */
    
    
    //wall2.compile((*tsIt).getMin(),(*tsIt).getMax());
    
    //glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    
    
    int frameSkipper = 0;
    
    while (!glfwWindowShouldClose(win)) {
        //std::cout << "CALLBACK!" << std::endl;
        if (running) {
            /*if (!(tsIt != tsEnd)) {
                tsIt = (*file.begin<3>());
            } */
          //  counter = 0;
#ifdef ENABLE_VIDEO
            if (video) {
                buffer->startRender(cam);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
                shaderW.use();
                cam.setShader(shaderW);
                cam.render();
                //wall2.render();

                shader.use();
                cam.setShader(shader);
                cam.render();

                //std::cout << sim.getLastEvenTime()<< std::endl;

                ball.renderInstanced((*tsIt).data());
                buffer->stopRender(cam);
                vcodec->writeFrame(*buffer);
            }
            
            if (frameSkipper == 1) {
#endif
              ++tsIt;
#ifdef ENABLE_VIDEO
              frameSkipper = 0;
            }
            frameSkipper++;
            
            if (tsIt == tsEnd) {
                running = false;
                if (video) {
                    vcodec->close();
                    delete vcodec;
                    delete buffer;
                    vcodec = nullptr;
                    buffer = nullptr;
                }
            }
            
            std::map<unsigned int, unsigned int> specCntr;
            for (const MercuryParticle<3>& p : *tsIt) {
                specCntr[p.speciesID]++;
            }
            
            std::cout << (*tsIt).getTime() << "\t";
            for (auto p : specCntr) {
                std::cout << "[" << p.first << "]=" << p.second << "\t";
            }
            std::cout << std::endl;
#endif            
            //std::cout << "Particles: " << (*tsIt).getNumberOfParticles() << std::endl;
            //std::cout << (*tsIt)[0].radius << std::endl;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        cam.move(movedir);

        shaderW.use();
        cam.setShader(shaderW);
        cam.render();
        //wall2.render();
        
        shader.use();
        cam.setShader(shader);
        cam.render();
        
        //std::cout << sim.getLastEvenTime()<< std::endl;
        
        ball.renderInstanced((*tsIt).data());
        
        
        glfwSwapBuffers(win);
        glfwPollEvents();
        
        if (glfwWindowShouldClose(win)) {
            break;
        }
    }

#ifdef ENABLE_VIDEO
    if (video) {
        vcodec->close();
        delete vcodec;
        delete buffer;
    }
#endif
    
    glfwDestroyWindow(win);
    glfwTerminate();
    
    return 0;
}

