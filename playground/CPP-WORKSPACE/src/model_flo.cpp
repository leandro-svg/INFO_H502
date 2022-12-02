#include <../include/glad/glad.h>
#include <../include/GLFW/glfw3.h>

#include <../include/learopengl/shader_m.h>
#include <../include/learopengl/camera.h>
#include <../include/learopengl/model.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#include <../include/flori/EBO.h>
#include <../include/flori/VBO.h>
#include <../include/flori/VAO.h>



// #define STB_IMAGE_IMPLEMENTATION
// #include "../include/stb/stb_image.h"


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
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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
    

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("./shaders/vertex/3D.vs", "./shaders/fragment/light.fs"); // you can name your shader files however you like

    Shader ourShaderSecond("./shaders/vertex/horizontalOffest.vs", "./shaders/fragment/3.3.shader.fs"); // you can name your shader files however you like

    Shader ourShaderThird("./shaders/vertex/model_loading.vs", "./shaders/fragment/model_loading.fs"); // you can name your shader files however you like
    // Shader ourShaderSecond("shaders/vertex/3.3.shader.vs", "./shaders/fragment/simple.fs"); // you can name your shader files however you like

    Shader ourShaderFourth("./shaders/vertex/cubemap.vs", "./shaders/fragment/cubemap.fs"); // you can name your shader files however you like

    // GOING 3D
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
        
        1, 2, 3,  // second triangle
        0, 1, 3, // first triangle
    };

    unsigned int indicesSecond[] = {  
        0, 1, 2, // first triangle
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


    // float texCoords[] = {
    // 0.5f, -0.5f,  // lower-left corner  
    // -0.5f, -0.5f,  // lower-right corner
    // 0.0f,  0.5f   // top-center corner
    // };

    unsigned int VBO[3], VAO[3],  lightVAO; // EBO[2],
    glGenVertexArrays(3, VAO);
    glGenBuffers(3, VBO);
    //glGenBuffers(2, EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    VAO VAO1;
	VAO1.Bind();
	// Vertex Buffer object creation + linking to the vertices
	VBO VBO1(vertices, sizeof(vertices));
    // Element Buffer object creation + linking to the indices
	EBO EBO1(indices, sizeof(indices));
    
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

    glBindVertexArray(VAO[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSecond), verticesSecond, GL_STATIC_DRAW);


    EBO EBO2(indicesSecond, sizeof(indicesSecond));
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesSecond), indicesSecond, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO[2]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);  

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);  

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    
 
    // glBindBuffer(GL_ARRAY_BUFFER, 0); 

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    


    unsigned int ourTexture_text, texture_text2;
    // texture 1
    // ---------
    glGenTextures(1, &ourTexture_text);
    glBindTexture(GL_TEXTURE_2D, ourTexture_text); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/image/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {   
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture_text2);
    glBindTexture(GL_TEXTURE_2D, texture_text2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // stbi_set_flip_vertically_on_load(true); 
    data = stbi_load("/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/image/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    vector<std::string> faces
    {
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/posx.jpg",
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/negx.jpg",
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/negy.jpg",
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/posy.jpg",
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/posz.jpg",
        "/home/leand/ULB_course/INFO-H502/playground/CPP-WORKSPACE/cubemap/negz.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);
    ourShader.use();
    glUniform1i(glGetUniformLocation(ourShader.ID, "ourTexture_text"), 0); // set it manually
    ourShader.setInt("ourTexture_text", 0);
    ourShader.setInt("texture_text2", 1); // or with shader class

    ourShaderThird.use();
    glUniform1i(glGetUniformLocation(ourShaderThird.ID, "texture_diffuse5"), 1); // set it manually


    ourShaderFourth.use();
    ourShaderFourth.setInt("skyboxY", 0);
    // ourShaderFourth.use();
    // glUniform1i(glGetUniformLocation(ourShaderFourth.ID, cubemapTexture), 0); // set it manually

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
       
       


        model = glm::rotate(model, 0.0f, glm::vec3(0.5f, 1.0f, 0.0f)); 
        // view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ourTexture_text);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_text2);


        
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

        glDepthMask(GL_FALSE);
        ourShaderFourth.use();
        glm::mat4 projectionY = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShaderFourth.setMat4("projectionY", projectionY);

        // camera/view transformation
        glm::mat4 viewY = camera.GetViewMatrix();
        viewY = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        ourShaderFourth.setMat4("viewY", viewY);
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position); 
        ourShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        ourShader.setFloatReal("ambient",  0.0f);//sin((float)glfwGetTime()) + 1);
        ourShader.setVec3("lightPos",  lightPos);
        glm::mat4 trans = glm::mat4(1.0f);
        //trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform_text");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projectionX = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projectionX", projectionX);

        // camera/view transformation
        glm::mat4 viewX = camera.GetViewMatrix();
        ourShader.setMat4("viewX", viewX);
        glBindVertexArray(VAO[0]);        
        // update shader uniform
        
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
        // // glDrawArrays(GL_TRIANGLES, 0, 12);
        // glBindVertexArray(VAO[1]);
        // ourShaderSecond.use();
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // int modelLoc4 = glGetUniformLocation(ourShader.ID, "model");
        // glUniformMatrix4fv(modelLoc4, 1, GL_FALSE, glm::value_ptr(model));

        // int viewLoc4 = glGetUniformLocation(ourShader.ID, "view");
        // glUniformMatrix4fv(viewLoc4, 1, GL_FALSE, glm::value_ptr(view));

        // int projectionLoc4 = glGetUniformLocation(ourShader.ID, "projection");
        // glUniformMatrix4fv(projectionLoc4, 1, GL_FALSE, glm::value_ptr(projection));

        //LIGHTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT                
        glBindVertexArray(VAO[2]);
        glm::mat4 trans3 = glm::mat4(1.0f);
        trans3 = glm::translate(trans3, lightPos);
        trans3 = glm::rotate(trans3, 0.0f, glm::vec3(1.0f, 0.0f, 1.0f));
        ourShader.setVec3("lightColor",  1000.0f, 1000.0f, 1000.0f);
        ourShader.setFloatReal("ambient",  1000.0f);
        

        unsigned int transformLoc3 = glGetUniformLocation(ourShader.ID, "transform_text");
        glUniformMatrix4fv(transformLoc3, 1, GL_FALSE, glm::value_ptr(trans3));
        glDrawArrays(GL_TRIANGLES, 0, 36);


        ourShader.setMat4("projectionX", projectionX);
        
        // camera/view transformation
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

        ourShaderThird.use();

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
        
        
        ourShaderSecond.use();

        // camera/view transformation
        glBindVertexArray(VAO[1]);
        double  timeValue = glfwGetTime();
        float greenValue = static_cast<float>(sin(timeValue)/1.0);
        // int vertexColorLocation = glGetUniformLocation(ourShader.ID, "offset");
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        ourShaderSecond.setFloat("offset", greenValue);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        glfwSwapInterval(1);
        
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    EBO1.Delete();
    EBO2.Delete();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    
    // if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    //     std::cout << "Escape for some reasons11" << std::endl;
    //     glfwSetWindowShouldClose(window, true);
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

 