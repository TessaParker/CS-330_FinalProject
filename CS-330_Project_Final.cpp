// Tessa Parker
// 04/16/2023
//--------------------
// *** CS-330: FINAL PROJECT ***

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

#define STB_IMAGE_IMPLEMENTATION

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers for sphere creation
#include "ShapeGenerator.h"
#include "ShapeData.h"

// Header inclusions for camera and images
#include "camera.h"        // Camera class (taken from learnopengl)
#include "stb_image.h"     // Image loading Utility functions

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Final Project: Tessa Parker"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint containerVao;         // Handles for the vertex array objects
        GLuint planeVao;
        GLuint lampVao;
        GLuint bookVao;

        GLuint containerVbo;         // Handles for the vertex buffer objects
        GLuint planeVbo;
        GLuint lampVbo;
        GLuint bookVbo;

        GLuint nContainerVertices;    // Number of indices of the meshes
        GLuint nPlaneVertices;
        GLuint nLampVertices;
        GLuint nBookVertices;

    };


    // Main GLFW window
    GLFWwindow* gWindow = nullptr;

    // Triangle mesh data
    GLMesh gMesh;

    // Texture id
    GLuint gTextureContainer;
    GLuint gTexturePlane;
    GLuint gTextureLamp;
    GLuint gTextureSphere;
    GLuint gTextureBook;

    glm::vec2 gUVScale(1.0f, 1.0f);
    GLint gTexWrapMode = GL_CLAMP_TO_BORDER;

    // Shader program
    GLuint gContainerProgramId;
    GLuint gPlaneProgramId;
    GLuint gLampProgramId;
    GLuint gBookProgramId;

    // camera
    Camera gCamera(glm::vec3(-1.5f, 2.0f, 8.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // offset variables for sphere
    const uint NUM_VERTICES_PER_TRI = 3;
    const uint NUM_FLOATS_PER_VERTICE = 9;
    const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);


    // Object and light color
    glm::vec3 gObjectColor(1.f, 0.2f, 0.0f);
    glm::vec3 gLightColor(1.0f, 1.0f, 1.0f);

    // Light position and scale
    glm::vec3 gLightPosition(1.5f, 5.5f, -3.0f);
    glm::vec3 gLightScale(1.0f);

    // Container position and scale
    glm::vec3 gContainerPosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gContainerScale(1.0f);

    // Plane (tabletop) position and scale
    glm::vec3 gPlanePosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gPlaneScale(1.0f);

    // Lamp (plane of light) position and scale
    glm::vec3 gLampPosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gLampScale(1.0f);

    // Sphere position and scale
    glm::vec3 gSpherePosition(0.0f, 0.5f, -5.0f);
    glm::vec3 gSphereScale(1.0f);

    // Book position and scale
    glm::vec3 gBookPosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gBookScale(1.0f);

}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
//Input Processing
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
//Meshes
void containerMesh(GLMesh& mesh);
void planeMesh(GLMesh& mesh);
void lampMesh(GLMesh& mesh);
void bookMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
//Texture Handling
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
//Rendering Functions
void URenderContainer();
void URenderPlane();
void URenderLamp();
void URenderSphere();
void URenderBook();
//Shader Program Handling
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
//Orthographic function: Default FALSE
bool orthoView = false;


//----------------------------------------------
/* Vertex Shader Source Code for CONTAINER */
//-----------------------------------------------
const GLchar* containerVertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position;
    layout(location = 1) in vec3 normal; // VAP position 1 for normals
    layout(location = 2) in vec2 textureCoordinate;

    out vec3 vertexNormal; // For outgoing normals to fragment shader
    out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
    out vec2 vertexTextureCoordinate;


//Global variables for the transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties

    vertexTextureCoordinate = textureCoordinate;
}
);

/* Fragment Shader Source Code for CONTAINER*/
//----------------------------------------------
const GLchar* containerFragmentShaderSource = GLSL(440,
    in vec3 vertexNormal; // For incoming normals
    in vec3 vertexFragmentPos; // For incoming fragment position
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    // Uniform / Global variables for object color, light color, light position, and camera/view position
    uniform vec3 objectColor;
    uniform vec3 lightColor;
    uniform vec3 lightPos;
    uniform vec3 viewPosition;
    uniform sampler2D uTexture; // Useful when working with multiple textures
    uniform vec2 uvScale;



void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.75f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 1.0f; // Set specular light strength
    float highlightSize = 32.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

//-------------------------------------------
/* Vertex Shader Source Code for PLANE */
//-------------------------------------------
const GLchar* planeVertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position;
    layout(location = 1) in vec3 normal; // VAP position 1 for normals
    layout(location = 2) in vec2 textureCoordinate;

    out vec3 vertexNormal; // For outgoing normals to fragment shader
    out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
    out vec2 vertexTextureCoordinate;


    //Global variables for the transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties

    vertexTextureCoordinate = textureCoordinate;
}
);

/* Fragment Shader Source Code for PLANE*/
//----------------------------------------------
const GLchar* planeFragmentShaderSource = GLSL(440,
    in vec3 vertexNormal; // For incoming normals
    in vec3 vertexFragmentPos; // For incoming fragment position
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.75f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 1.0f; // Set specular light strength
    float highlightSize = 32.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);


//-------------------------------------------
/* Vertex Shader Source Code for LAMP */
//-------------------------------------------
const GLchar* lampVertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position;


//Global variables for the transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates

}
);

/* Fragment Shader Source Code for LAMP*/
//----------------------------------------------
const GLchar* lampFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor;

void main()
{

    fragmentColor = vec4(1.0); // Send lighting results to GPU
}
);


//-------------------------------------------
/* Vertex Shader Source Code for BOOK */
//-------------------------------------------
const GLchar* bookVertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;


//Global variables for the transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties

    vertexTextureCoordinate = textureCoordinate;
}
);


/* Fragment Shader Source Code for BOOK*/
//----------------------------------------------
const GLchar* bookFragmentShaderSource = GLSL(440,
    in vec3 vertexNormal; // For incoming normals
    in vec3 vertexFragmentPos; // For incoming fragment position
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;



void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.75f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 1.0f; // Set specular light strength
    float highlightSize = 32.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the meshes
    containerMesh(gMesh); // Calls the function to create the Vertex Buffer Object
    planeMesh(gMesh);
    lampMesh(gMesh);
    bookMesh(gMesh);

    // Create the shader programs
    if (!UCreateShaderProgram(containerVertexShaderSource, containerFragmentShaderSource, gContainerProgramId))
    {
        return EXIT_FAILURE;
    }

    if (!UCreateShaderProgram(planeVertexShaderSource, planeFragmentShaderSource, gPlaneProgramId))
    {
        return EXIT_FAILURE;
    }

    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId))
    {
        return EXIT_FAILURE;
    }

    if (!UCreateShaderProgram(bookVertexShaderSource, bookFragmentShaderSource, gBookProgramId))
    {
        return EXIT_FAILURE;
    }


    // Load texture
    const char* texContainer = "Debug/resources/ContainerTexture.jpg";
    const char* texPlane = "Debug/resources/TableTexture.jpg";
    const char* texBook = "Debug/resources/PlannerTexture.jpg";
    const char* texSphere = "Debug/resources/TableTexture.jpg";
    

    if (!UCreateTexture(texContainer, gTextureContainer))
    {
        cout << "Failed to load texture " << texContainer << endl;
        return EXIT_FAILURE;
    }

    if (!UCreateTexture(texPlane, gTexturePlane))
    {
        cout << "Failed to load texture " << texPlane << endl;
        return EXIT_FAILURE;
    }

    if (!UCreateTexture(texSphere, gTextureSphere))
    {
        cout << "Failed to load texture " << texBook << endl;
        return EXIT_FAILURE;
    }

    if (!UCreateTexture(texBook, gTextureBook))
    {
        cout << "Failed to load texture " << texBook << endl;
        return EXIT_FAILURE;
    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gContainerProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gContainerProgramId, "uTexture"), 0);
    
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gPlaneProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gPlaneProgramId, "uTexture"), 0);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gLampProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gLampProgramId, "uTexture"), 0);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gBookProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gBookProgramId, "uTexture"), 0);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // Clear the frame and z buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URenderContainer();
        URenderPlane();
        URenderLamp();
        URenderSphere();
        URenderBook();

        glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);

    // Release texture
    UDestroyTexture(gTextureContainer);
    UDestroyTexture(gTexturePlane);
    UDestroyTexture(gTextureLamp);
    UDestroyTexture(gTextureSphere);
    UDestroyTexture(gTextureBook);

    // Release shader program
    UDestroyShaderProgram(gContainerProgramId);
    UDestroyShaderProgram(gPlaneProgramId);
    UDestroyShaderProgram(gLampProgramId);
    //UDestroyShaderProgram(gSphereProgramId);
    UDestroyShaderProgram(gBookProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    //To get Ortho view
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        gCamera.WorldUp = glm::vec3(0.0f, -1.0f, 0.0f);
        orthoView = true;
    }
    else
    {
        gCamera.WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        orthoView = false;
    }
    //EXIT
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //WASD commands
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);

    //Move Camera Up
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    //Move Camera Down
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);

    //Texture Scaling 
    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
    {
        gUVScale += 0.1f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
    {
        gUVScale -= 0.1f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


// Functioned called to render a frame
void URenderContainer()
{

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // 1. Scales the object 
    glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
    // 2. Rotates shape by 'n' degrees in the x axis
    glm::mat4 rotation = glm::rotate(45.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Place object
    glm::mat4 translation = glm::translate(glm::vec3(3.0f, 2.0f, 0.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;


    glm::mat4 projection;
    glm::mat4 view;

    //Orthographic View option
    if (orthoView) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    // camera/view transformation
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Set the shader to be used
    glUseProgram(gContainerProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gContainerProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gContainerProgramId, "view");
    GLint projLoc = glGetUniformLocation(gContainerProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.containerVao);

    // Reference matrix uniforms from the Pyramid Shader program for the shape color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(gContainerProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gContainerProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gContainerProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gContainerProgramId, "viewPosition");

    // Pass color, light, and camera data to the Pyramid Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(gContainerProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureContainer);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nContainerVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

void URenderPlane()
{

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // 1. Scales the object 
    glm::mat4 scale = glm::scale(glm::vec3(5.0f, 5.0f, 5.0f));
    // 2. Rotates shape 
    glm::mat4 rotation = glm::rotate(45.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Place object 
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 7.5f, 0.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    glm::mat4 projection;
    glm::mat4 view;

    //Orthographic View option
    if (orthoView) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    // camera/view transformation
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Set the shader to be used
    glUseProgram(gPlaneProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gPlaneProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gPlaneProgramId, "view");
    GLint projLoc = glGetUniformLocation(gPlaneProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.planeVao);

    // Reference matrix uniforms from the Pyramid Shader program for the shape color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(gPlaneProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gPlaneProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gPlaneProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gPlaneProgramId, "viewPosition");

    // Pass color, light, and camera data to the Pyramid Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(gPlaneProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexturePlane);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nPlaneVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

void URenderLamp()
{

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // 1. Scales the object
    glm::mat4 scale = glm::scale(glm::vec3(1.25f, 1.25f, 1.25f));
    // 2. Rotates shape 
    glm::mat4 rotation = glm::rotate(45.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Place object 
    glm::mat4 translation = glm::translate(glm::vec3(-1.0f, -7.0f, 5.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    glm::mat4 projection;
    glm::mat4 view;

    //Orthographic View option
    if (orthoView) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    // camera/view transformation
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Set the shader to be used
    glUseProgram(gLampProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gLampProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gLampProgramId, "view");
    GLint projLoc = glGetUniformLocation(gLampProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.lampVao);

    // Reference matrix uniforms from the Pyramid Shader program for the shape color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(gLampProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gLampProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gLampProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gLampProgramId, "viewPosition");

    // Pass color, light, and camera data to the Pyramid Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(gLampProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nLampVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

void URenderSphere()
{    
    // creates sphere object
    ShapeData sphere = ShapeGenerator::makeSphere();

    unsigned int sphereVBO{}, sphereVAO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.vertexBufferSize() + sphere.indexBufferSize(), 0, GL_STATIC_DRAW);
    int currentOffset = 0;
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.vertexBufferSize(), sphere.vertices);
    currentOffset += sphere.vertexBufferSize();
    int sphereIndexByteOffset = currentOffset;
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.indexBufferSize(), sphere.indices);
    int sphereNumIndices = sphere.numIndices;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereVBO);
    
    // setup to draw sphere
    glBindTexture(GL_TEXTURE_2D, gTextureSphere);
    glBindVertexArray(sphereVAO);
    glm::mat4(1.0f);
    glm::translate(glm::vec3(0.0f, 7.1f, -2.0f));
    glm::scale(glm::vec3(5.5f));

    // draw sphere
    glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)sphereIndexByteOffset);

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    //// 1. Scales the object 
    //glm::mat4 scale = glm::scale(glm::vec3(5.0f, 0.0f, 5.0f));
    //// 2. Rotates shape by 'n' degrees in the x axis
    //glm::mat4 rotation = glm::rotate(45.0f, glm::vec3(1.0, 1.0f, 1.0f));
    //// 3. Place object at the origin
    //glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
    //// Model matrix: transformations are applied right-to-left order
    //glm::mat4 model = translation * rotation * scale;

    glm::mat4 projection;
    glm::mat4 view;

    //Orthographic View option
    if (orthoView) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    // camera/view transformation
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

void URenderBook()
{

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // 1. Scales the object 
    glm::mat4 scale = glm::scale(glm::vec3(7.0f, 5.0f, 5.0f));
    // 2. Rotates shape by 'n' degrees in the x axis
    glm::mat4 rotation = glm::rotate(45.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(12.0f, -6.0f, 9.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;


    glm::mat4 projection;
    glm::mat4 view;

    //Orthographic View option
    if (orthoView) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    // camera/view transformation
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Set the shader to be used
    glUseProgram(gBookProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gBookProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gBookProgramId, "view");
    GLint projLoc = glGetUniformLocation(gBookProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.bookVao);

    // Reference matrix uniforms from the Pyramid Shader program for the shape color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(gBookProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gBookProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gBookProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gBookProgramId, "viewPosition");

    // Pass color, light, and camera data to the Pyramid Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(gBookProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureBook);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nBookVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}


// Implements the UCreateMesh function
void containerMesh(GLMesh& mesh)
{
    // Vertex data
    GLfloat containerVerts[] = {

        //Positions           //Normals              //Texture Coordinates
        //********** PYRAMID [LID] VERTS **********
        // ----------------------------------------
        //Side Triangle: 1    //Negative Z
       -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,    0.0f, 0.0f, //BL
        0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,    1.0f, 1.0f, //FL
        0.0f,  0.15f, 0.0f,   0.0f,  0.0f, -1.0f,    0.0f, 1.0f, //Top
        //Side Triangle: 2    //Positive Z
       -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,    0.0f, 0.0f, //FL
        0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,    1.0f, 1.0f, //FR
        0.0f,  0.15f, 0.0f,   0.0f,  0.0f,  1.0f,    0.0f, 1.0f, //Top
        //Side Triangle: 3    //Negative X
       -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,    0.0f, 0.0f, //BL
       -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,    1.0f, 1.0f, //FL
        0.0f,  0.15f, 0.0f,  -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //Top
        //Side Triangle: 4    //Positive x
        0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,    0.0f, 0.0f, //BR
        0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,    1.0f, 1.0f, //FR
        0.0f,  0.15f, 0.0f,   1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //Top
        //Base Triangle: 1    //Negative Y
        0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,    0.0f, 0.0f, //BR
        0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,    1.0f, 1.0f, //FR
        0.0f, -0.5f,  0.0f,   0.0f, -1.0f,  0.0f,    0.0f, 1.0f, //Bottom Center
        //Base Triangle: 2    //Positive Y
       -0.5f, -0.5f,  0.5f,   0.0f,  1.0f,  0.0f,    0.0f, 0.0f, //FL
       -0.5f, -0.5f, -0.5f,   0.0f,  1.0f,  0.0f,    1.0f, 1.0f, //BL
        0.0f, -0.5f,  0.0f,   0.0f,  1.0f,  0.0f,    0.0f, 1.0f, //Bottom Center


        //************ CUBE [BODY] VERTS ************
        // ------------------------------------------
        //Side: 1 - BACK     //Negative Z
       -0.5f, -1.5f, -0.5f,  0.0f,  0.0f, -1.0f,    0.0f, 0.0f, //BBL
        0.5f, -1.5f, -0.5f,  0.0f,  0.0f, -1.0f,    1.0f, 0.0f, //BBR
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,    1.0f, 1.0f, //BTR
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,    1.0f, 1.0f, //BTR
       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,    0.0f, 1.0f, //BTL
       -0.5f, -1.5f, -0.5f,  0.0f,  0.0f, -1.0f,    0.0f, 0.0f, //BBL
       //Side: 2 - FRONT     //Positive Z
       -0.5f, -1.5f,  0.5f,  0.0f,  0.0f,  1.0f,    0.0f, 0.0f, //FBL
        0.5f, -1.5f,  0.5f,  0.0f,  0.0f,  1.0f,    1.0f, 0.0f, //FBR
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    1.0f, 1.0f, //FTR
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    1.0f, 1.0f, //FTR
       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    0.0f, 1.0f, //FTL
       -0.5f, -1.5f,  0.5f,  0.0f,  0.0f,  1.0f,    0.0f, 0.0f, //FBL
       //Side: 3 - LEFT      //Negative X
       -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //FTL
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,    1.0f, 1.0f, //BTL
       -0.5f, -1.5f, -0.5f, -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //BBL
       -0.5f, -1.5f, -0.5f, -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //BBL
       -0.5f, -1.5f,  0.5f, -1.0f,  0.0f,  0.0f,    0.0f, 0.0f, //FBL
       -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //FTL
        //Side: 4 - RIGHT    //Positive x
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //FTR
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,    1.0f, 1.0f, //BTR
        0.5f, -1.5f, -0.5f,  1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //BBR
        0.5f, -1.5f, -0.5f,  1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //BBR
        0.5f, -1.5f,  0.5f,  1.0f,  0.0f,  0.0f,    0.0f, 0.0f, //FBR
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //FTR
        //Side: 5 - BOTTOM   //Negative Y
       -0.5f, -1.5f, -0.5f,  0.0f, -1.0f,  0.0f,    0.0f, 1.0f, //BBL
        0.5f, -1.5f, -0.5f,  0.0f, -1.0f,  0.0f,    1.0f, 1.0f, //BBR
        0.5f, -1.5f,  0.5f,  0.0f, -1.0f,  0.0f,    1.0f, 0.0f, //FBR
        0.5f, -1.5f,  0.5f,  0.0f, -1.0f,  0.0f,    1.0f, 0.0f, //FBR
       -0.5f, -1.5f,  0.5f,  0.0f, -1.0f,  0.0f,    0.0f, 0.0f, //FBL
       -0.5f, -1.5f, -0.5f,  0.0f, -1.0f,  0.0f,    0.0f, 1.0f, //BBL
        //Side: 6 - TOP      //Positive Y
       -0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,    0.0f, 1.0f,

    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    //Container vertices
    mesh.nContainerVertices = sizeof(containerVerts) / (sizeof(containerVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.containerVao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.containerVao);

    // Create VBO
    glGenBuffers(1, &mesh.containerVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.containerVbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(containerVerts), containerVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

void planeMesh(GLMesh& mesh)
{
    // Vertex data
    GLfloat planeVerts[] = {

     //************ PLANE VERTS ************
     // ------------------------------------
     //Positions          //Normals              //Texture Coordinates
     //-------------------    TOP   ----------------------------------
    -4.0f, -1.5f, -2.0f,  0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
     4.0f, -1.5f, -2.0f,  0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
     4.0f, -1.5f,  2.0f,  0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
     //-------------------   BOTTOM   ----------------------------------
     4.0f, -1.5f,  2.0f,  0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
    -4.0f, -1.5f,  2.0f,  0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
    -4.0f, -1.5f, -2.0f,  0.0f, -1.0f,  0.0f,    0.0f, 1.0f,

    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    //Container vertices
    mesh.nPlaneVertices = sizeof(planeVerts) / (sizeof(planeVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.planeVao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.planeVao);

    // Create VBO
    glGenBuffers(1, &mesh.planeVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.planeVbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), planeVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

void lampMesh(GLMesh& mesh)
{
    // Vertex data
    GLfloat lampVerts[] = {

        //************ Lamp VERTS ************
        // ------------------------------------
        //Positions          //Normals              //Texture Coordinates
       //Side: 1 - BACK     //Negative Z
       -1.5f,  5.5f, -3.5f,  0.0f,  0.0f, -1.0f,    0.0f, 0.0f, //BBL
       -0.5f,  5.5f, -3.5f,  0.0f,  0.0f, -1.0f,    1.0f, 0.0f, //BBR
       -0.5f,  4.5f, -3.5f,  0.0f,  0.0f, -1.0f,    1.0f, 1.0f, //BTR
       -0.5f,  4.5f, -3.5f,  0.0f,  0.0f, -1.0f,    1.0f, 1.0f, //BTR
       -1.5f,  4.5f, -3.5f,  0.0f,  0.0f, -1.0f,    0.0f, 1.0f, //BTL
       -1.5f,  5.5f, -3.5f,  0.0f,  0.0f, -1.0f,    0.0f, 0.0f, //BBL
       //Side: 2 - FRONT     //Positive Z
       -1.5f,  5.5f, -2.5f,  0.0f,  0.0f,  1.0f,    0.0f, 0.0f, //FBL
       -0.5f,  5.5f, -2.5f,  0.0f,  0.0f,  1.0f,    1.0f, 0.0f, //FBR
       -0.5f,  4.5f, -2.5f,  0.0f,  0.0f,  1.0f,    1.0f, 1.0f, //FTR
       -0.5f,  4.5f, -2.5f,  0.0f,  0.0f,  1.0f,    1.0f, 1.0f, //FTR
       -1.5f,  5.5f, -2.5f,  0.0f,  0.0f,  1.0f,    0.0f, 1.0f, //FTL
       -1.5f,  4.5f, -2.5f,  0.0f,  0.0f,  1.0f,    0.0f, 0.0f, //FBL
       //Side: 3 - LEFT      //Negative X
       -1.5f,  4.5f, -2.5f, -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //FTL
       -1.5f,  4.5f, -3.5f, -1.0f,  0.0f,  0.0f,    1.0f, 1.0f, //BTL
       -1.5f,  5.5f, -3.5f, -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //BBL
       -1.5f,  5.5f, -3.5f, -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //BBL
       -1.5f,  5.5f, -2.5f, -1.0f,  0.0f,  0.0f,    0.0f, 0.0f, //FBL
       -1.5f,  4.5f, -2.5f, -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //FTL
       //Side: 4 - RIGHT    //Positive x
      -0.5f,   4.5f, -2.5f,  1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //FTR
      -0.5f,   4.5f, -3.5f,  1.0f,  0.0f,  0.0f,    1.0f, 1.0f, //BTR
      -0.5f,   5.5f, -3.5f,  1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //BBR
      -0.5f,   5.5f, -3.5f,  1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //BBR
      -0.5f,   5.5f, -2.5f,  1.0f,  0.0f,  0.0f,    0.0f, 0.0f, //FBR
      -0.5f,   4.5f, -2.5f,  1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //FTR
       //Side: 5 - BOTTOM   //Negative Y
      -1.5f,   5.5f, -3.5f,  0.0f, -1.0f,  0.0f,    0.0f, 1.0f, //BBL
      -0.5f,   5.5f, -3.5f,  0.0f, -1.0f,  0.0f,    1.0f, 1.0f, //BBR
      -0.5f,   5.5f, -2.5f,  0.0f, -1.0f,  0.0f,    1.0f, 0.0f, //FBR
      -0.5f,   5.5f, -2.5f,  0.0f, -1.0f,  0.0f,    1.0f, 0.0f, //FBR
      -1.5f,   5.5f, -2.5f,  0.0f, -1.0f,  0.0f,    0.0f, 0.0f, //FBL
      -1.5f,   5.5f, -3.5f,  0.0f, -1.0f,  0.0f,    0.0f, 1.0f, //BBL
      //Side: 6 - TOP      //Positive Y
     -1.5f,    4.5f, -3.5f,  0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
     -0.5f,    4.5f, -3.5f,  0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
     -0.5f,    4.5f, -2.5f,  0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
     -0.5f,    4.5f, -2.5f,  0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
     -1.5f,    4.5f, -2.5f,  0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
     -1.5f,    4.5f, -3.5f,  0.0f,  1.0f,  0.0f,    0.0f, 1.0f,

    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    // vertices
    mesh.nLampVertices = sizeof(lampVerts) / (sizeof(lampVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.lampVao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.lampVao);

    // Create VBO
    glGenBuffers(1, &mesh.lampVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.lampVbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(lampVerts), lampVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


void bookMesh(GLMesh& mesh)
{
    // Vertex data
    GLfloat bookVerts[] = {

        //************ Lamp VERTS ************
        // ------------------------------------
        //Positions          //Normals              //Texture Coordinates
       //Side: 1 - BACK     //Negative Z
       -1.5f,  0.15f, -3.5f,  0.0f,  0.0f, -1.0f,    0.0f, 0.0f, //BBL
       -0.5f,  0.15f, -3.5f,  0.0f,  0.0f, -1.0f,    1.0f, 0.0f, //BBR
       -0.5f,  0.00f, -3.5f,  0.0f,  0.0f, -1.0f,    1.0f, 1.0f, //BTR
       -0.5f,  0.00f, -3.5f,  0.0f,  0.0f, -1.0f,    1.0f, 1.0f, //BTR
       -1.5f,  0.00f, -3.5f,  0.0f,  0.0f, -1.0f,    0.0f, 1.0f, //BTL
       -1.5f,  0.15f, -3.5f,  0.0f,  0.0f, -1.0f,    0.0f, 0.0f, //BBL
       //Side: 2 - FRONT     //Positive Z
       -1.5f,  0.15f, -2.5f,  0.0f,  0.0f,  1.0f,    0.0f, 0.0f, //FBL
       -0.5f,  0.15f, -2.5f,  0.0f,  0.0f,  1.0f,    1.0f, 0.0f, //FBR
       -0.5f,  0.00f, -2.5f,  0.0f,  0.0f,  1.0f,    1.0f, 1.0f, //FTR
       -0.5f,  0.00f, -2.5f,  0.0f,  0.0f,  1.0f,    1.0f, 1.0f, //FTR
       -1.5f,  0.15f, -2.5f,  0.0f,  0.0f,  1.0f,    0.0f, 1.0f, //FTL
       -1.5f,  0.00f, -2.5f,  0.0f,  0.0f,  1.0f,    0.0f, 0.0f, //FBL
       //Side: 3 - LEFT      //Negative X
       -1.5f,  0.00f, -2.5f, -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //FTL
       -1.5f,  0.00f, -3.5f, -1.0f,  0.0f,  0.0f,    1.0f, 1.0f, //BTL
       -1.5f,  0.15f, -3.5f, -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //BBL
       -1.5f,  0.15f, -3.5f, -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, //BBL
       -1.5f,  0.15f, -2.5f, -1.0f,  0.0f,  0.0f,    0.0f, 0.0f, //FBL
       -1.5f,  0.00f, -2.5f, -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //FTL
       //Side: 4 - RIGHT    //Positive x
       -0.5f,   0.00f, -2.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f, //FTR
       -0.5f,   0.00f, -3.5f,  1.0f,  0.0f,  0.0f,   1.0f, 1.0f, //BTR
       -0.5f,   0.15f, -3.5f,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f, //BBR
       -0.5f,   0.15f, -3.5f,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f, //BBR
       -0.5f,   0.15f, -2.5f,  1.0f,  0.0f,  0.0f,   0.0f, 0.0f, //FBR
       -0.5f,   0.00f, -2.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f, //FTR
       //Side: 5 - BOTTOM   //Negative Y
       -1.5f,   0.15f, -3.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f, //BBL
       -0.5f,   0.15f, -3.5f,  0.0f, -1.0f,  0.0f,   1.0f, 1.0f, //BBR
       -0.5f,   0.15f, -2.5f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f, //FBR
       -0.5f,   0.15f, -2.5f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f, //FBR
       -1.5f,   0.15f, -2.5f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f, //FBL
       -1.5f,   0.15f, -3.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f, //BBL
       //Side: 6 - TOP      //Positive Y
       -1.5f,   0.00f, -3.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
       -0.5f,   0.00f, -3.5f,  0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
       -0.5f,   0.00f, -2.5f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
       -0.5f,   0.00f, -2.5f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
       -1.5f,   0.00f, -2.5f,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
       -1.5f,   0.00f, -3.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,

    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    // vertices
    mesh.nBookVertices = sizeof(bookVerts) / (sizeof(bookVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.bookVao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.bookVao);

    // Create VBO
    glGenBuffers(1, &mesh.bookVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.bookVbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(bookVerts), bookVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.containerVao);
    glDeleteBuffers(1, &mesh.containerVbo);

    glDeleteVertexArrays(1, &mesh.planeVao);
    glDeleteBuffers(1, &mesh.planeVbo);

    glDeleteVertexArrays(1, &mesh.lampVao);
    glDeleteBuffers(1, &mesh.lampVbo);

    glDeleteVertexArrays(1, &mesh.bookVao);
    glDeleteBuffers(1, &mesh.bookVbo);
}



/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}


void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}

