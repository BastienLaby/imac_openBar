#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

#include "glew/glew.h"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "GL/glfw.h"
#include "stb/stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imguiRenderGL3.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr


#ifndef DEBUG_PRINT
#define DEBUG_PRINT 1
#endif

#if DEBUG_PRINT == 0
#define debug_print(FORMAT, ...) ((void)0)
#else
#ifdef _MSC_VER
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#else
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __func__, __FILE__, __LINE__, __VA_ARGS__)
#endif
#endif

// Font buffers
extern const unsigned char DroidSans_ttf[];
extern const unsigned int DroidSans_ttf_len;    

struct ShaderGLSL
{
    enum ShaderType
    {
        VERTEX_SHADER = 1,
        FRAGMENT_SHADER = 2,
        GEOMETRY_SHADER = 4
    };
    GLuint program;
};

int compile_and_link_shader(ShaderGLSL & shader, int typeMask, const char * sourceBuffer, int bufferSize);
int destroy_shader(ShaderGLSL & shader);
int load_shader_from_file(ShaderGLSL & shader, const char * path, int typemask);
    
struct Camera
{
    float radius;
    float theta;
    float phi;
    glm::vec3 o;
    glm::vec3 eye;
    glm::vec3 up;
};

void camera_defaults(Camera & c);
void camera_zoom(Camera & c, float factor);
void camera_turn(Camera & c, float phi, float theta);
void camera_pan(Camera & c, float x, float y);

struct GUIStates
{
    bool panLock;
    bool turnLock;
    bool zoomLock;
    int lockPositionX;
    int lockPositionY;
    int camera;
    double time;
    bool playing;
    static const float MOUSE_PAN_SPEED;
    static const float MOUSE_ZOOM_SPEED;
    static const float MOUSE_TURN_SPEED;
};
const float GUIStates::MOUSE_PAN_SPEED = 0.001f;
const float GUIStates::MOUSE_ZOOM_SPEED = 0.05f;
const float GUIStates::MOUSE_TURN_SPEED = 0.005f;


void init_gui_states(GUIStates & guiStates)
{
    guiStates.panLock = false;
    guiStates.turnLock = false;
    guiStates.zoomLock = false;
    guiStates.lockPositionX = 0;
    guiStates.lockPositionY = 0;
    guiStates.camera = 0;
    guiStates.time = 0.0;
    guiStates.playing = false;
}


int main( int argc, char **argv )
{


    int width = 1200, height= 675;
    float widthf = (float) width, heightf = (float) height;
    double t;
    float fps = 0.f;

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit( EXIT_FAILURE );
    }

    // Force core profile on Mac OSX
#ifdef __APPLE__
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    // Open a window and create its OpenGL context
    if( !glfwOpenWindow( width, height, 0,0,0,0, 24, 0, GLFW_WINDOW ) )
    {
        fprintf( stderr, "Failed to open GLFW window\n" );

        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    glfwSetWindowTitle( "001_a" );

    // Core profile is flagged as experimental in glew
#ifdef __APPLE__
    glewExperimental = GL_TRUE;
#endif
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
          /* Problem: glewInit failed, something is seriously wrong. */
          fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
          exit( EXIT_FAILURE );
    }

    // Ensure we can capture the escape key being pressed below
    glfwEnable( GLFW_STICKY_KEYS );

    // Enable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );
    GLenum glerr = GL_NO_ERROR;
    glerr = glGetError();

    if (!imguiRenderGLInit(DroidSans_ttf, DroidSans_ttf_len))
    {
        fprintf(stderr, "Could not init GUI renderer.\n");
        exit(EXIT_FAILURE);
    }

    // Init viewer structures
    Camera camera;
    camera_defaults(camera);
    GUIStates guiStates;
    init_gui_states(guiStates);

    glerr = glGetError();
    if(glerr != GL_NO_ERROR)
        fprintf(stderr, "2nd OpenGL Error : %s\n", gluErrorString(glerr));

    // Try to load and compile shader
    ShaderGLSL shader;
    const char * shaderFile = "001/1.glsl";
    int status = load_shader_from_file(shader, shaderFile, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER | ShaderGLSL::GEOMETRY_SHADER);
    //int status = load_shader_from_file(shader, shaderFile, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading  %s\n", shaderFile);
        exit( EXIT_FAILURE );
    }

    // Apply shader
    GLuint program = shader.program;
    glUseProgram(program);

    // Matrix
    GLuint projectionLocation = glGetUniformLocation(program, "Projection");
    GLuint viewLocation = glGetUniformLocation(program, "View");
    GLuint objectLocation = glGetUniformLocation(program, "Object");
    
    // Animate
    GLuint timeLocation = glGetUniformLocation(program, "Time");
    GLuint instanceCountLocation = glGetUniformLocation(program, "InstanceCount");
    GLuint amplitudeLocation = glGetUniformLocation(program, "Amplitude");
    GLuint spaceBetweenCubesLocation = glGetUniformLocation(program, "SpaceBetweenCubes");
    
    // Light
    GLuint lightPositionLocation = glGetUniformLocation(program, "LightPosition");
    GLuint lightSpecularColorLocation = glGetUniformLocation(program, "LightSpecularColor");
    GLuint lightDiffusePowerLocation = glGetUniformLocation(program, "LightDiffusePower");

    GLuint cameraPositionLocation = glGetUniformLocation(program, "CameraPosition");
   
    // Textures
    GLuint diffuseTextureLocation = glGetUniformLocation(program, "DiffuseTexture");
    GLuint specularTextureLocation = glGetUniformLocation(program, "SpecularTexture");

    //
    // Load images and upload textures
    //

    GLuint textures[2];
    glGenTextures(2, textures);
    int x;
    int y;
    int comp;

    unsigned char * diffuse = stbi_load("textures/spnza_bricks_a_diff.tga", &x, &y, &comp, 3);
    fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    

    unsigned char * spec = stbi_load("textures/spnza_bricks_a_spec.tga", &x, &y, &comp, 1);
    fprintf(stderr, "Spec %dx%d:%d\n", x, y, comp);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, spec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(diffuseTextureLocation, 0);
    glUniform1i(specularTextureLocation, 2);


    // Load geometry
    int cube_triangleCount = 12;
    int cube_triangleList[] = {0, 1, 2,
                               2, 1, 3,
                               4, 5, 6,
                               6, 5, 7,
                               8, 9, 10,
                               10, 9, 11,
                               12, 13, 14,
                               14, 13, 15,
                               16, 17, 18,
                               19, 17, 20,
                               21, 22, 23,
                               24, 25, 26,};
    float cube_uvs[] = {0.f, 0.f,
                        0.f, 1.f,
                        1.f, 0.f,
                        1.f, 1.f,
                        0.f, 0.f,
                        0.f, 1.f,
                        1.f, 0.f,
                        1.f, 1.f,
                        0.f, 0.f,
                        0.f, 1.f,
                        1.f, 0.f,
                        1.f, 1.f,
                        0.f, 0.f,
                        0.f, 1.f,
                        1.f, 0.f,
                        1.f, 1.f,
                        0.f, 0.f,
                        0.f, 1.f,
                        1.f, 0.f, 
                        1.f, 0.f, 
                        1.f, 1.f, 
                        0.f, 1.f, 
                        1.f, 1.f, 
                        0.f, 0.f,
                        0.f, 0.f,
                        1.f, 1.f, 
                        1.f, 0.f};
    float cube_vertices[] = {-0.5, -0.5, 0.5,
                             0.5, -0.5, 0.5,
                             -0.5, 0.5, 0.5,
                             0.5, 0.5, 0.5,
                             -0.5, 0.5, 0.5,
                             0.5, 0.5, 0.5,
                             -0.5, 0.5, -0.5,
                             0.5, 0.5, -0.5,
                             -0.5, 0.5, -0.5,
                             0.5, 0.5, -0.5,
                             -0.5, -0.5, -0.5,
                             0.5, -0.5, -0.5, 
                             -0.5, -0.5, -0.5,
                             0.5, -0.5, -0.5,
                             -0.5, -0.5, 0.5,
                             0.5, -0.5, 0.5,
                             0.5, -0.5, 0.5,
                             0.5, -0.5, -0.5,
                             0.5, 0.5, 0.5,
                             0.5, 0.5, 0.5,
                             0.5, 0.5, -0.5,
                             -0.5, -0.5, -0.5,
                             -0.5, -0.5, 0.5,
                             -0.5, 0.5, -0.5,
                             -0.5, 0.5, -0.5,
                             -0.5, -0.5, 0.5,
                             -0.5, 0.5, 0.5 };
    float cube_normals[] = {0, 0, 1,
                            0, 0, 1,
                            0, 0, 1,
                            0, 0, 1,
                            0, 1, 0,
                            0, 1, 0,
                            0, 1, 0,
                            0, 1, 0,
                            0, 0, -1,
                            0, 0, -1,
                            0, 0, -1,
                            0, 0, -1,
                            0, -1, 0,
                            0, -1, 0,
                            0, -1, 0,
                            0, -1, 0,
                            1, 0, 0,
                            1, 0, 0,
                            1, 0, 0,
                            1, 0, 0,
                            1, 0, 0,
                            -1, 0, 0,
                            -1, 0, 0,
                            -1, 0, 0,
                            -1, 0, 0,
                            -1, 0, 0,
                            -1, 0, 0};
    

    // Create and bind VAO and VBO

    // Create one vbo for each mesh attribute
    GLuint vbos[8];
    glGenBuffers(8, vbos);

    GLuint vaos[2];
    glGenVertexArrays(2, vaos);

    // Bind vao for the cube mesh
    glBindVertexArray(vaos[0]);

    // bind vbo then send data to vbo currently bind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[0]); // For indexes attribute
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_triangleList), cube_triangleList, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uvs), cube_uvs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);

    // Unbind everything. Potentially illegal on some implementations
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Viewport 
    glViewport( 0, 0, width, height  );

    camera.eye.x = -4.6;
    camera.eye.y = 5.1;
    camera.eye.z = 7.3;

    // Animation Loop variables
    float timeStep = 0.1f;
    float timeSpend = 0.f;
    float nbCubeInstance = 9.f;
    float amplitude = 0.2f;
    float spaceBetweenCubes = 0.f;

    // Lights Loop variables
    glm::vec3 lightPosition(-8.67, 2.70, -6.63);
    glm::vec4 lightSpecularColor(1, 0.5, 0.25, 1);
    float lightDiffusePower = 1.f;

    bool checked = false;

    do
    {
        t = glfwGetTime();

        // Mouse states
        int leftButton = glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT );
        int rightButton = glfwGetMouseButton( GLFW_MOUSE_BUTTON_RIGHT );
        int middleButton = glfwGetMouseButton( GLFW_MOUSE_BUTTON_MIDDLE );

        if( leftButton == GLFW_PRESS )
            guiStates.turnLock = true;
        else
            guiStates.turnLock = false;

        if( rightButton == GLFW_PRESS )
            guiStates.zoomLock = true;
        else
            guiStates.zoomLock = false;

        if( middleButton == GLFW_PRESS )
            guiStates.panLock = true;
        else
            guiStates.panLock = false;

        // Camera movements
        int altPressed = glfwGetKey(GLFW_KEY_LSHIFT);
        if (!altPressed && (leftButton == GLFW_PRESS || rightButton == GLFW_PRESS || middleButton == GLFW_PRESS))
        {
            int x; int y;
            glfwGetMousePos(&x, &y);
            guiStates.lockPositionX = x;
            guiStates.lockPositionY = y;
        }
        if (altPressed == GLFW_PRESS)
        {
            int mousex; int mousey;
            glfwGetMousePos(&mousex, &mousey);
            int diffLockPositionX = mousex - guiStates.lockPositionX;
            int diffLockPositionY = mousey - guiStates.lockPositionY;
            if (guiStates.zoomLock)
            {
                float zoomDir = 0.0;
                if (diffLockPositionX > 0)
                    zoomDir = -1.f;
                else if (diffLockPositionX < 0 )
                    zoomDir = 1.f;
                camera_zoom(camera, zoomDir * GUIStates::MOUSE_ZOOM_SPEED);
            }
            else if (guiStates.turnLock)
            {
                camera_turn(camera, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
                            diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

            }
            else if (guiStates.panLock)
            {
                camera_pan(camera, diffLockPositionX * GUIStates::MOUSE_PAN_SPEED,
                            diffLockPositionY * GUIStates::MOUSE_PAN_SPEED);
            }
            guiStates.lockPositionX = mousex;
            guiStates.lockPositionY = mousey;
        }

        // Default states
        glEnable(GL_DEPTH_TEST);

        // Clear the front buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get camera matrices
        glm::mat4 projection = glm::perspective(45.0f, widthf / heightf, 0.1f, 100.f); 
        glm::mat4 worldToView = glm::lookAt(camera.eye, camera.o, camera.up);
        glm::mat4 objectToWorld;
        timeSpend += timeStep;
        if(timeSpend > 2*3.1415)
        {
            timeSpend = 0;
        }

        //
        // Activate texture units and bind textures to them
        //

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        // Select shader
        glUseProgram(program);

        //
        // Upload uniforms
        //

        // Matrix
        glUniformMatrix4fv(projectionLocation, 1, 0, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLocation, 1, 0, glm::value_ptr(worldToView));
        glUniformMatrix4fv(objectLocation, 1, 0, glm::value_ptr(objectToWorld));
        
        // Animation
        glUniform1f(timeLocation, timeSpend);
        glUniform1i(instanceCountLocation, (int)nbCubeInstance);
        glUniform1f(amplitudeLocation, amplitude);
        glUniform1f(spaceBetweenCubesLocation, spaceBetweenCubes);
        
        // Lights
        glUniform3fv(cameraPositionLocation, 1, glm::value_ptr(camera.eye));
        glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
        glUniform4fv(lightSpecularColorLocation, 1, glm::value_ptr(lightSpecularColor));
        glUniform1f(lightDiffusePowerLocation, lightDiffusePower);


        //
        // Render vaos
        //

        glBindVertexArray(vaos[0]);
        glDrawElementsInstanced(GL_TRIANGLES, cube_triangleCount*3, GL_UNSIGNED_INT, 0, (int)nbCubeInstance); // param count = taille du tableau d'indices

        // Draw light
        //glDrawElement(GL_TRIANGLES, cube_triangleCount*3, GL_UNSIGNED_INT, 0);

#if 1
        // Draw UI
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, width, height);

        unsigned char mbut = 0;
        int mscroll = 0;
        int mousex; int mousey;
        glfwGetMousePos(&mousex, &mousey);
        mousey = height - mousey;

        if( leftButton == GLFW_PRESS )
            mbut |= IMGUI_MBUT_LEFT;

        imguiBeginFrame(mousex, mousey, mbut, mscroll);
        int logScroll = 0;
        char lineBuffer[512];

        imguiBeginScrollArea("OpenBar", 0, 0, 200, height, &logScroll);

        sprintf(lineBuffer, "FPS %f (%.1f, %.1f, %.1f)", fps, camera.eye.x, camera.eye.y, camera.eye.z);
        imguiLabel(lineBuffer);

        imguiSeparatorLine();

        imguiSlider("Speed", &timeStep, 0.0, 0.8, 0.01);
        imguiSlider("Nb cubes", &nbCubeInstance, 1, 10000, 1);
        int but33 = imguiButton("3x3");
        int but55 = imguiButton("5x5");
        int but77 = imguiButton("7x7");
        int but99 = imguiButton("9x9");
        if(but33) { nbCubeInstance = 9; }
        if(but55) { nbCubeInstance = 25; }
        if(but77) { nbCubeInstance = 49; }
        if(but99) { nbCubeInstance = 81; }
        imguiSlider("Amplitude", &amplitude, 0, 3, 0.1);
        imguiSlider("Space Between Cubes", &spaceBetweenCubes, 0, 1, 0.01);

        imguiSeparatorLine();

        imguiLabel("Light Position");
        imguiIndent();
            imguiSlider("x", &lightPosition.x, -50, 50, 0.01);
            imguiSlider("y", &lightPosition.y, -50, 50, 0.01);
            imguiSlider("z", &lightPosition.z, -50, 50, 0.01);
        imguiUnindent();
        imguiSlider("Light Power", &lightDiffusePower, 0, 1, 0.01);
        imguiLabel("Light Specular Color");
        imguiIndent();
            imguiSlider("r", &lightSpecularColor.x, 0, 1, 0.01);
            imguiSlider("g", &lightSpecularColor.y, 0, 1, 0.01);
            imguiSlider("b", &lightSpecularColor.z, 0, 1, 0.01);
        imguiUnindent();
            
        imguiDrawRect(160, 157, 15, 15, imguiRGBA(255*lightSpecularColor.x, 255*lightSpecularColor.y, 255*lightSpecularColor.z, 255*lightSpecularColor.w));

        imguiEndScrollArea();
        imguiEndFrame();
        imguiRenderGLDraw(width, height);

        glDisable(GL_BLEND);
#endif
        // Check for errors
        GLenum err = glGetError();
        if(err != GL_NO_ERROR)
        {
            fprintf(stderr, "OpenGL Error : %s\n", gluErrorString(err));
        }

        // Swap buffers
        glfwSwapBuffers();

        double newTime = glfwGetTime();
        fps = 1.f/ (newTime - t);
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    exit( EXIT_SUCCESS );
}


int  compile_and_link_shader(ShaderGLSL & shader, int typeMask, const char * sourceBuffer, int bufferSize)
{
    // Create program object
    shader.program = glCreateProgram();
    
    //Handle Vertex Shader
    GLuint vertexShaderObject ;
    if (typeMask & ShaderGLSL::VERTEX_SHADER)
    {
        // Create shader object for vertex shader
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        // Add #define VERTEX to buffer
        const char * sc[3] = { "#version 150\n", "#define VERTEX\n", sourceBuffer};
        glShaderSource(vertexShaderObject, 
                       3, 
                       sc,
                       NULL);
        // Compile shader
        glCompileShader(vertexShaderObject);

        // Get error log size and print it eventually
        int logLength;
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1)
        {
            char * log = new char[logLength];
            glGetShaderInfoLog(vertexShaderObject, logLength, &logLength, log);
            fprintf(stderr, "Error in compiling vertex shader : %s", log);
            fprintf(stderr, "%s\n%s\n%s", sc[0], sc[1], sc[2]);
            delete[] log;
        }
        // If an error happend quit
        int status;
        glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
            return -1;          

        //Attach shader to program
        glAttachShader(shader.program, vertexShaderObject);
    }

    // Handle Geometry shader
    GLuint geometryShaderObject ;
    if (typeMask & ShaderGLSL::GEOMETRY_SHADER)
    {
        // Create shader object for Geometry shader
        geometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);
        // Add #define Geometry to buffer
        const char * sc[3] = { "#version 150\n", "#define GEOMETRY\n", sourceBuffer};
        glShaderSource(geometryShaderObject, 
                       3, 
                       sc,
                       NULL);
        // Compile shader
        glCompileShader(geometryShaderObject);

        // Get error log size and print it eventually
        int logLength;
        glGetShaderiv(geometryShaderObject, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1)
        {
            char * log = new char[logLength];
            glGetShaderInfoLog(geometryShaderObject, logLength, &logLength, log);
            fprintf(stderr, "Error in compiling Geometry shader : %s \n", log);
            fprintf(stderr, "%s\n%s\n%s", sc[0], sc[1], sc[2]);
            delete[] log;
        }
        // If an error happend quit
        int status;
        glGetShaderiv(geometryShaderObject, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
            return -1;          

        //Attach shader to program
        glAttachShader(shader.program, geometryShaderObject);
    }


    // Handle Fragment shader
    GLuint fragmentShaderObject ;
    if (typeMask && ShaderGLSL::FRAGMENT_SHADER)
    {
        // Create shader object for fragment shader
        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        // Add #define fragment to buffer
        const char * sc[3] = { "#version 150\n", "#define FRAGMENT\n", sourceBuffer};
        glShaderSource(fragmentShaderObject, 
                       3, 
                       sc,
                       NULL);
        // Compile shader
        glCompileShader(fragmentShaderObject);

        // Get error log size and print it eventually
        int logLength;
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1)
        {
            char * log = new char[logLength];
            glGetShaderInfoLog(fragmentShaderObject, logLength, &logLength, log);
            fprintf(stderr, "Error in compiling fragment shader : %s \n", log);
            fprintf(stderr, "%s\n%s\n%s", sc[0], sc[1], sc[2]);
            delete[] log;
        }
        // If an error happend quit
        int status;
        glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
            return -1;          

        //Attach shader to program
        glAttachShader(shader.program, fragmentShaderObject);
    }


    // Bind attribute location
    glBindAttribLocation(shader.program,  0,  "VertexPosition");
    glBindAttribLocation(shader.program,  1,  "VertexNormal");
    glBindAttribLocation(shader.program,  2,  "VertexTexCoord");
    glBindFragDataLocation(shader.program, 0, "Color");
    glBindFragDataLocation(shader.program, 1, "Normal");

    // Link attached shaders
    glLinkProgram(shader.program);

    // Clean
    if (typeMask & ShaderGLSL::VERTEX_SHADER)
    {
        glDeleteShader(vertexShaderObject);
    }
    if (typeMask && ShaderGLSL::GEOMETRY_SHADER)
    {
        glDeleteShader(fragmentShaderObject);
    }
    if (typeMask && ShaderGLSL::FRAGMENT_SHADER)
    {
        glDeleteShader(fragmentShaderObject);
    }

    // Get link error log size and print it eventually
    int logLength;
    glGetProgramiv(shader.program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        char * log = new char[logLength];
        glGetProgramInfoLog(shader.program, logLength, &logLength, log);
        fprintf(stderr, "Error in linking shaders : %s \n", log);
        delete[] log;
    }
    int status;
    glGetProgramiv(shader.program, GL_LINK_STATUS, &status);        
    if (status == GL_FALSE)
        return -1;


    return 0;
}

int  destroy_shader(ShaderGLSL & shader)
{
    glDeleteProgram(shader.program);
    shader.program = 0;
    return 0;
}

int load_shader_from_file(ShaderGLSL & shader, const char * path, int typemask)
{
    int status;
    FILE * shaderFileDesc = fopen( path, "rb" );
    if (!shaderFileDesc)
        return -1;
    fseek ( shaderFileDesc , 0 , SEEK_END );
    long fileSize = ftell ( shaderFileDesc );
    rewind ( shaderFileDesc );
    char * buffer = new char[fileSize + 1];
    fread( buffer, 1, fileSize, shaderFileDesc );
    buffer[fileSize] = '\0';
    status = compile_and_link_shader( shader, typemask, buffer, fileSize );
    delete[] buffer;
    return status;
}


void camera_compute(Camera & c)
{
    c.eye.x = cos(c.theta) * sin(c.phi) * c.radius + c.o.x;   
    c.eye.y = cos(c.phi) * c.radius + c.o.y ;
    c.eye.z = sin(c.theta) * sin(c.phi) * c.radius + c.o.z;   
    c.up = glm::vec3(0.f, c.phi < M_PI ?1.f:-1.f, 0.f);
}

void camera_defaults(Camera & c)
{
    c.phi = 3.14/2.f;
    c.theta = 3.14/2.f;
    c.radius = 10.f;
    camera_compute(c);
}

void camera_zoom(Camera & c, float factor)
{
    c.radius += factor * c.radius ;
    if (c.radius < 0.1)
    {
        c.radius = 10.f;
        c.o = c.eye + glm::normalize(c.o - c.eye) * c.radius;
    }
    camera_compute(c);
}

void camera_turn(Camera & c, float phi, float theta)
{
    c.theta += 1.f * theta;
    c.phi   -= 1.f * phi;
    if (c.phi >= (2 * M_PI) - 0.1 )
        c.phi = 0.00001;
    else if (c.phi <= 0 )
        c.phi = 2 * M_PI - 0.1;
    camera_compute(c);
}

void camera_pan(Camera & c, float x, float y)
{
    glm::vec3 up(0.f, c.phi < M_PI ?1.f:-1.f, 0.f);
    glm::vec3 fwd = glm::normalize(c.o - c.eye);
    glm::vec3 side = glm::normalize(glm::cross(fwd, up));
    c.up = glm::normalize(glm::cross(side, fwd));
    c.o[0] += up[0] * y * c.radius * 2;
    c.o[1] += up[1] * y * c.radius * 2;
    c.o[2] += up[2] * y * c.radius * 2;
    c.o[0] -= side[0] * x * c.radius * 2;
    c.o[1] -= side[1] * x * c.radius * 2;
    c.o[2] -= side[2] * x * c.radius * 2;       
    camera_compute(c);
}
