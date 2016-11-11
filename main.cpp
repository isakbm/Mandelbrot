#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


double PI = 4.0*atan(1.0);

GLFWwindow* window;
double prevx = -1, prevy = -1;
double resx = 1600,resy = 900;

// "camera": specify x and y coordinate of lower left pixel
double dy = 2.0;                    // height
double dx = dy*resx/float(resy);    // scaled width
double minx = -2.5;                 // x from -2.0 to 0.5
double miny = -1.0;                 // y from -1.0 to 1.0

int clickedButtons = 0;
enum buttonMaps { FIRST_BUTTON=1, SECOND_BUTTON=2, THIRD_BUTTON=4, FOURTH_BUTTON=8, FIFTH_BUTTON=16, NO_BUTTON=0 };
enum modifierMaps { CTRL=2, SHIFT=1, ALT=4, META=8, NO_MODIFIER=0 };

int drawMandelbrot = 1;

GLuint programID;
GLuint VertexArrayID;
GLuint vertexbuffer;    

static const GLfloat vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
};


char *readFile(const char *filename) {
    FILE *f = fopen(filename, "rb");

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    printf("Reading %s, Filesize = %d\n", filename, int(fsize));

    char *string = (char*)malloc(fsize + 1);
    fread(string, fsize, 1, f);
    string[fsize] = '\0';
    fclose(f);

    return string;
}

void CompileShader(const char * file_path, GLuint ShaderID) {
    GLint Result = GL_FALSE;
    int InfoLogLength;

    char *ShaderCode   = readFile(file_path);

    // Compile Shader
    printf("Compiling shader : %s\n", file_path);
    glShaderSource(ShaderID, 1, (const char**)&ShaderCode , NULL);
    glCompileShader(ShaderID);

    // Check Shader
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( Result == GL_FALSE ){
        char ShaderErrorMessage[InfoLogLength];
        glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, ShaderErrorMessage);
        printf("%s", ShaderErrorMessage);
    }
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
    printf("Creating shaders\n");
    GLuint VertexShaderID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    CompileShader(vertex_file_path, VertexShaderID);
    CompileShader(fragment_file_path, FragmentShaderID);


    printf("Create and linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( InfoLogLength > 0 ){
        GLchar ProgramErrorMessage[InfoLogLength+1];
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    fflush(stdout);

    return ProgramID;
}


void initGL();
void Draw();
void RTT();

void windowsize_callback(GLFWwindow *win, int width, int height);
void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
void mousebutton_callback(GLFWwindow* win, int button, int action, int mods);
void mousepos_callback(GLFWwindow* win, double xpos, double ypos);
void mousewheel_callback(GLFWwindow* win, double xoffset, double yoffset);

int main() {
    initGL();

    // vertex buffers are bound to a vertex array object (VAO)
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "vertex_shader.vs", "fragment_shader.fs" );

    // vertex data are bound to vertex buffer objects (VBO)
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);  // copy to gpu

    RTT();

    while ( !glfwWindowShouldClose(window)) {   
        Draw();
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

    glfwTerminate();

    return 0;
}

void RTT(){

    // Generate texture

    // Bind texture

    // Render to texture

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programID);

    // transfer over uniform data (might have changed)
    GLint loc;
    loc = glGetUniformLocation(programID, "resx");
    if (loc != -1) 
        glUniform1f(loc, resx);
    loc = glGetUniformLocation(programID, "resy");
    if (loc != -1) 
        glUniform1f(loc, resy);
    loc = glGetUniformLocation(programID, "minx");
    if (loc != -1) 
        glUniform1f(loc, minx);
    loc = glGetUniformLocation(programID, "miny");
    if (loc != -1) 
        glUniform1f(loc, miny);
    loc = glGetUniformLocation(programID, "dx");
    if (loc != -1) 
        glUniform1f(loc, dx);
    loc = glGetUniformLocation(programID, "dy");
    if (loc != -1) 
        glUniform1f(loc, dy);
    loc = glGetUniformLocation(programID, "drawMandelbrot");
    if (loc != -1) 
        glUniform1i(loc, drawMandelbrot);

    glEnableVertexAttribArray(0);                            // matches shader layout specifier
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);             // vertex data bound to this buffer
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0); // specify layout (vec3, float)
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);                   // Draw the triangles using a triangle strip
    glDisableVertexAttribArray(0);


}

void Draw() {
    
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programID);

    // transfer over uniform data (might have changed)
    GLint loc;
    loc = glGetUniformLocation(programID, "resx");
    if (loc != -1) 
        glUniform1f(loc, resx);
    loc = glGetUniformLocation(programID, "resy");
    if (loc != -1) 
        glUniform1f(loc, resy);
    loc = glGetUniformLocation(programID, "minx");
    if (loc != -1) 
        glUniform1f(loc, minx);
    loc = glGetUniformLocation(programID, "miny");
    if (loc != -1) 
        glUniform1f(loc, miny);
    loc = glGetUniformLocation(programID, "dx");
    if (loc != -1) 
        glUniform1f(loc, dx);
    loc = glGetUniformLocation(programID, "dy");
    if (loc != -1) 
        glUniform1f(loc, dy);
    loc = glGetUniformLocation(programID, "drawMandelbrot");
    if (loc != -1) 
        glUniform1i(loc, drawMandelbrot);

    glEnableVertexAttribArray(0);                            // matches shader layout specifier
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);             // vertex data bound to this buffer
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0); // specify layout (vec3, float)
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);                   // Draw the triangles using a triangle strip
    glDisableVertexAttribArray(0);

    // Swap buffers
    glfwSwapBuffers(window);
}
void initGL() {

    printf("Initializing OpenGL/GLFW\n"); 
    if (!glfwInit()) {
        printf("Could not initialize\n");
        exit(-1);
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(resx, resy, "Mandelbrot", 0, 0);
    if (!window) {
        printf("Could not open glfw window\n");
        glfwTerminate();
        exit(-2);
    }
    glfwMakeContextCurrent(window); 

    // Initialize GLEW
    printf("Initializing GLEW\n"); 
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(-3);
    }
    

    glfwSetWindowSizeCallback(window,windowsize_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mousebutton_callback);
    glfwSetScrollCallback(window, mousewheel_callback);
    glfwSetCursorPosCallback(window, mousepos_callback);

    glfwSwapInterval(1);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
}

void windowsize_callback(GLFWwindow *win, int width, int height) {
    double distance_per_pixel = dy/resy; // assuming cam_height/resy == cam_width/resx

    resx = width;
    resy = height;
    dx = distance_per_pixel*resx;
    dy = distance_per_pixel*resy;

    glViewport(0, 0, resx, resy);
}

void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(win, GL_TRUE);
    }

    if (key == GLFW_KEY_SPACE && action) {
        drawMandelbrot = 1 - drawMandelbrot;
    }
}

void mousebutton_callback(GLFWwindow* win, int button, int action, int mods) {
    glfwGetCursorPos(win,&prevx,&prevy);

    if (action == 1)
        clickedButtons |= (1 << button);
    else
        clickedButtons &= ~(1 << button);

    if (clickedButtons&FIRST_BUTTON) {
        
    } else if (clickedButtons&SECOND_BUTTON) {

    } else if (clickedButtons&THIRD_BUTTON) {

    } else if (clickedButtons&FOURTH_BUTTON) {

    } else if (clickedButtons&FIFTH_BUTTON) {

    }
}

void mousepos_callback(GLFWwindow* win, double xpos, double ypos) {
    if (clickedButtons&FIRST_BUTTON) {
        minx -= (xpos-prevx)*dx/resx;
        miny += (ypos-prevy)*dy/resy;

        prevx = xpos;
        prevy = ypos;
    } else if (clickedButtons&SECOND_BUTTON) {

    } else if (clickedButtons&THIRD_BUTTON) {

    } else if (clickedButtons&FOURTH_BUTTON) {

    } else if (clickedButtons&FIFTH_BUTTON) {

    }
}

void mousewheel_callback(GLFWwindow* win, double xoffset, double yoffset) {
    double zoomFactor = pow(0.95,yoffset);

    minx = minx + dx*(1.0 - zoomFactor)/2.0;
    miny = miny + dy*(1.0 - zoomFactor)/2.0;
    dx *= zoomFactor;
    dy *= zoomFactor;
}
