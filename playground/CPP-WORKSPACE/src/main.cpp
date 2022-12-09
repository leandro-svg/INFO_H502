#include <../include/glad/glad.h>
#include <../include/GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <../include/stb/stb_image.h>
#include <../include/learopengl/Camera.h>
#include <../include/learopengl/model.h>
#include <../include/flori/EBO.h>
#include <../include/flori/VBO.h>
#include <../include/flori/VAO.h>
#include <../include/flori/Shader.h>
#include <../include/flori/texture.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
} 

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
   
    if( !glfwInit() ) {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);  
    
    Shader ourShader("../shaders/vertex/3D.vs", "../shaders/fragment/light.fs"); // you can name your shader files however you like
    Shader ourShaderSecond("../shaders/vertex/horizontalOffest.vs", "../shaders/fragment/3.3.shader.fs"); // you can name your shader files however you like
    Shader ourShaderThird("../shaders/vertex/model_loading.vs", "../shaders/fragment/model_loading.fs"); // you can name your shader files however you like
    Shader ourShaderFourth("../shaders/vertex/cubemap.vs", "../shaders/fragment/cubemap.fs"); // you can name your shader files however you like

    Model ourModel("/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/object/backpack.obj");
    Model SecondModel("/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/object/tynanausore.obj");
    Model mapModel("/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/object/map.obj");
    Model tryModel("/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/object/try.obj");

    glm::mat4 model = glm::mat4(1.0f);

    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,  0.0f, -1.0f,  // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,0.0f,  0.0f, -1.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f , 0.0f,  0.0f, -1.0f,  // top left 
    };

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    float cube[] = {
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,1.0f, 0.0f,0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,1.0f, 1.0f,0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,1.0f, 1.0f,0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,0.0f, 1.0f,0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,0.0f, 0.0f,0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,0.0f, 0.0f,0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f, 0.0f,0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f, 1.0f,0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f, 1.0f,0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,0.0f, 1.0f,0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,0.0f, 0.0f,0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f, 0.0f,-1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,1.0f, 1.0f,-1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,0.0f, 1.0f,-1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,0.0f, 1.0f,-1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,0.0f, 0.0f,-1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f, 0.0f,-1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f, 0.0f,1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,1.0f, 1.0f,1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,0.0f, 1.0f,1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,0.0f, 1.0f,1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,0.0f, 0.0f,1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f, 0.0f,1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,1.0f, 1.0f,0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f, 0.0f,0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f, 0.0f,0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,0.0f, 0.0f,0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,0.0f, 1.0f,0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,0.0f, 1.0f,0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,1.0f, 1.0f,0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f, 0.0f,0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f, 0.0f,0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,0.0f, 0.0f,0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,0.0f, 1.0f,0.0f,  1.0f,  0.0f
};

    float verticesSecond[] = {
        // positions         // colors
        0.6f, -0.6f, 0.0f,  1.0f, 0.0f, 1.0f,  // bottom right
        0.6f, -0.9f, 0.0f,  1.0f, 1.0f, 0.0f,  // bottom left
        0.775f,  -0.7f, 0.0f,  0.0f, 1.0f, 1.0f   // top 
    };
    unsigned int indices[] = {  
        1, 2, 3, 
        0, 1, 3, 
    };

    unsigned int indicesSecond[] = {  
        0, 1, 2, 
    };

    glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    VAO VAO1, VAO2, VAO3, skyboxVAO;
	VAO1.Bind();
	// Vertex Buffer object creation + linking to the vertices
	VBO VBO1(vertices, sizeof(vertices));
    // Element Buffer object creation + linking to the indices
	EBO EBO1(indices, sizeof(indices));

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

    VAO2.Bind();
	VBO VBO2(verticesSecond, sizeof(verticesSecond));
	EBO EBO2(indicesSecond, sizeof(indicesSecond));

    VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO2.LinkAttrib(VBO2, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    VAO2.Unbind();
	VBO2.Unbind();
	EBO2.Unbind();

    VAO3.Bind();
	VBO VBO3(cube, sizeof(cube));

    VAO3.LinkAttrib(VBO3, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
	VAO3.LinkAttrib(VBO3, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO3.LinkAttrib(VBO3, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO3.LinkAttrib(VBO3, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

    VAO3.Unbind();
	VBO3.Unbind();


    skyboxVAO.Bind();
	VBO skyboxVBO(skyboxVertices, sizeof(skyboxVertices));

    skyboxVAO.LinkAttrib(skyboxVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    skyboxVAO.Unbind();
	skyboxVBO.Unbind();
    Texture ourTexture_text("/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/image/container.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    Texture texture_text2("/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/image/awesomeface.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

    vector<std::string> faces
    {
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/posx.jpg",
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/negx.jpg",
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/negy.jpg",
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/posy.jpg",
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/posz.jpg",
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/negzsqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces); 

    ourShader.Activate();
    texture_text2.texUnit(ourShader, "ourTexture_text", 0);
    texture_text2.texUnit(ourShader, "texture_text2", 1);

    ourShaderThird.Activate();
    glUniform1i(glGetUniformLocation(ourShaderThird.ID, "texture_diffuse5"), 1); // set it manually

    ourShaderFourth.Activate();
    ourShaderFourth.setInt("skyboxY", 0);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        model = glm::rotate(model, 0.0f, glm::vec3(0.5f, 1.0f, 0.0f)); 
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        ourTexture_text.Bind();
        glActiveTexture(GL_TEXTURE1);
        texture_text2.Bind();

        
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

        glDepthMask(GL_FALSE);
        ourShaderFourth.Activate();
        glm::mat4 projectionY = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShaderFourth.setMat4("projectionY", projectionY);


        // camera/view transformation
        glm::mat4 viewY = camera.GetViewMatrix();
        viewY = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        ourShaderFourth.setMat4("viewY", viewY);
        skyboxVAO.Bind();
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        ourShader.Activate();
        ourShader.setVec3("viewPos", camera.Position); 
        ourShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        ourShader.setFloatReal("ambient",  0.0f);
        ourShader.setVec3("lightPos",  lightPos);
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform_text");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 projectionX = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projectionX", projectionX);

        glm::mat4 viewX = camera.GetViewMatrix();
        ourShader.setMat4("viewX", viewX);
        VAO1.Bind();        
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ourShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        ourShader.setVec3("lightPos",  lightPos);
        ourShader.setFloatReal("ambient",  0.0f);
        glm::mat4 trans2 = glm::mat4(1.0f);
        trans2 = glm::translate(trans2, glm::vec3(0.5f, -0.5f, -0.0f));
        trans2 = glm::rotate(trans2, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
        double  sctimeValue = glfwGetTime();
        float scaling = static_cast<float>(sin(sctimeValue)/1.0);
        trans2 = glm::scale(trans2, glm::vec3(scaling,scaling,scaling));  

        unsigned int transformLoc2 = glGetUniformLocation(ourShader.ID, "transform_text");
        glUniformMatrix4fv(transformLoc2, 1, GL_FALSE, glm::value_ptr(trans2));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        VAO3.Bind();
        glm::mat4 trans3 = glm::mat4(1.0f);
        trans3 = glm::translate(trans3, lightPos);
        trans3 = glm::rotate(trans3, 0.0f, glm::vec3(1.0f, 0.0f, 1.0f));
        ourShader.setVec3("lightColor",  1000.0f, 1000.0f, 1000.0f);
        ourShader.setFloatReal("ambient",  1000.0f);
        

        unsigned int transformLoc3 = glGetUniformLocation(ourShader.ID, "transform_text");
        glUniformMatrix4fv(transformLoc3, 1, GL_FALSE, glm::value_ptr(trans3));
        glDrawArrays(GL_TRIANGLES, 0, 36);


        ourShader.setMat4("projectionX", projectionX);
        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader.setFloatReal("ambient",  0.1f); //sin((float)glfwGetTime()) + 1);
        ourShader.setVec3("lightPos", lightPos);  
        ourShader.setMat4("viewX", viewX);
        for(unsigned int i = 0; i < 10; i++)
        {   
            glm::mat4 trans3 = glm::mat4(1.0f);
            trans3 = glm::translate(trans3, glm::vec3(-0.0f, -0.0f, -0.0f));
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i; 
            if (i%2) {
                ourShader.setVec3("lightColor",  0.0f, 1.0f, 0.0f);
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                trans3 = glm::rotate(trans3, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(1.0f, 1.0f, 1.0f));
            }
            else{
                ourShader.setVec3("lightColor",  1.0f, 0.0f, 0.0f);
                model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 0.3f, 0.5f));
                trans3 = glm::rotate(trans3, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            }
            ourShader.setMat4("model", model);

            unsigned int transformLoc3 = glGetUniformLocation(ourShader.ID, "transform_text");
            glUniformMatrix4fv(transformLoc3, 1, GL_FALSE, glm::value_ptr(trans3));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        ourShaderThird.Activate();

        glm::mat4 projection5 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view5 = camera.GetViewMatrix();
        ourShaderThird.setMat4("projection5", projection5);
        ourShaderThird.setMat4("view5", view5);

        glm::mat4 model5 = glm::mat4(1.0f);
        ourShaderThird.setVec3("lightColor5",  1.0f, 1.0f, 1.0f);
        ourShaderThird.setFloatReal("ambient5",  1.0f);
        glm::mat4 trans4 = glm::mat4(1.0f);
        trans4 = glm::translate(trans3, glm::vec3(-0.0f, -0.0f, -0.0f));
        trans4 = glm::rotate(trans4, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        
        model5 = glm::translate(model5, glm::vec3(2.2f, 1.0f, 2.0f)); // translate it down so it's at the center of the scene
        model5= glm::scale(model5, glm::vec3(0.005f, 0.005f, 0.005f));	// it's a bit too big for our scene, so scale it down
        ourShaderThird.setMat4("model5", model5);
        ourModel.Draw(ourShaderThird);
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(-2.0f, -2.0f, 0.0f)); // translate it down so it's at the center of the scene
        model2 = glm::scale(model2, glm::vec3(0.005f, 0.005f, 0.005f));	// it's a bit too big for our scene, so scale it down
        ourShaderThird.setMat4("model5", model2);
        SecondModel.Draw(ourShaderThird);

        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(-2.0f, -2.0f, -4.0f)); // translate it down so it's at the center of the scene
        model3 = glm::scale(model3, glm::vec3(0.005f, 0.005f, 0.005f));	// it's a bit too big for our scene, so scale it down
        ourShaderThird.setMat4("model5", model3);
        mapModel.Draw(ourShaderThird);

        glm::mat4 model4 = glm::mat4(1.0f);
        model4 = glm::translate(model4, glm::vec3(-2.0f, -2.0f, 4.0f)); // translate it down so it's at the center of the scene
        model4 = glm::scale(model4, glm::vec3(0.005f, 0.005f, 0.005f));	// it's a bit too big for our scene, so scale it down
        ourShaderThird.setMat4("model5", model4);
        tryModel.Draw(ourShaderThird);
        
        
        ourShaderSecond.Activate();

        // camera/view transformation
        VAO2.Bind();
        double  timeValue = glfwGetTime();
        float greenValue = static_cast<float>(sin(timeValue)/1.0);

        ourShaderSecond.setFloat("offset", greenValue);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
        glfwSwapInterval(1);
    }


    EBO1.Delete();
    EBO2.Delete();
    VAO1.Delete();
    VAO2.Delete();
    VAO3.Delete();
	VBO1.Delete();
    VBO2.Delete();
    VBO3.Delete();
    std::cout << "k13" << std::endl;
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window)
{
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    glfwSetWindowShouldClose(window, false);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

 