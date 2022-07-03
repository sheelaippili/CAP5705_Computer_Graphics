#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// Include GLM and glm for matrix operations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Made the shader function dynamic to load multiple vertex shader at the same timeby creating a shader_m.h header file
#include "shader_m.h"

//including stb_image.h to handle several file formats for texture mapping
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace glm;

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//final project code
//cube mapping function calls
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(std::vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//camera position
// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.3f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

//rotation angle
float rotationAngleZ = 20.0f;
float rotationAngleY = 0.0f;

//scaling values for x, y and z axis
float x = 1.0;
float y = 1.0;
float z = 1.0;

//For mouse movement
bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//to load .obj file
bool loadObjectFile(
    const char * path,
    std::vector<glm::vec3> & ov,
    std::vector<glm::vec2> & ou,
    std::vector<glm::vec3> & on
)
{
    printf("Loading OBJ file %s\n", path);

    std::vector<unsigned int> vi, ui, ni;
    std::vector<glm::vec3> tv;
    std::vector<glm::vec2> tu;
    std::vector<glm::vec3> tn;

    FILE * file = fopen(path, "r");
    if( file == NULL )
    {
        printf("Check .obj file path\n");
        getchar();
        return false;
    }
    while( 1 ){
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader
        
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            tv.push_back(vertex);
        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            tu.push_back(uv);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            tn.push_back(normal);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("File cannot be read\n");
                return false;
            }
            vi.push_back(vertexIndex[0]);
            vi.push_back(vertexIndex[1]);
            vi.push_back(vertexIndex[2]);
            ui.push_back(uvIndex[0]);
            ui.push_back(uvIndex[1]);
            ui.push_back(uvIndex[2]);
            ni.push_back(normalIndex[0]);
            ni.push_back(normalIndex[1]);
            ni.push_back(normalIndex[2]);
        }else{
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }
    
    // For each vertex of each triangle
    for( unsigned int i=0; i<vi.size(); i++ ){

        // Get the indices of its attributes
        unsigned int vertexIndex = vi[i];
        unsigned int uvIndex = ui[i];
        unsigned int normalIndex = ni[i];
        
        // Get the attributes from the index
        glm::vec3 vertex = tv[ vertexIndex-1 ];
        glm::vec2 uv = tu[ uvIndex-1 ];
        glm::vec3 normal = tn[ normalIndex-1 ];
        
        // Put the attributes in buffers
        ov.push_back(vertex);
        ou.push_back(uv);
        on.push_back(normal);
    
    }
    fclose(file);
    return true;
}

int main()
{

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "The Swamp", NULL, NULL);
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // // glew: load all OpenGL function pointers
    glewInit();
    
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    
    //Enable depth test
    glEnable(GL_DEPTH_TEST);
    
    // build and compile our shader program
    // ------------------------------------
    //shaders for skybox, environmentmapping, texturemapping, blending and instancing.
    Shader skyboxShader("SkyboxVertexShader.vs", "SkyboxFragmentShader.fs");
    Shader objone_shader("EnvironmentVertexShader.vs", "EnvironmentFragmentShader.fs");
    Shader texture_shader("TextureVertexShader.vs", "TextureFragmentShader.fs");
    Shader grass_shader("InstancingVertexShader.vs", "InstancingFragmentShader.fs");
    Shader alligator_shader("BlendVertexShader.vs", "BlendFragmentShader.fs");
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    
    //not used in current project
    float colours[] = {
        //1st triangle
      0.4f, 0.0f,  0.0f,
      0.4f, 0.0f,  0.0f,
      0.4f, 0.0f,  0.0f,

    };
    
    float planeVertices[] = {
    // positions          // texture Coords
     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
     5.0f, -0.5f, -5.0f,  2.0f, 2.0f
        };

    
    // load skybox .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadObjectFile("Skybox.obj", vertices, uvs, normals);
    
    // load object one (small cube in our project)
    std::vector<glm::vec3> vertices_objone;
    std::vector<glm::vec2> uvs_objone;
    std::vector<glm::vec3> normals_objone;
    res = loadObjectFile("seperatetriangles.obj", vertices_objone, uvs_objone, normals_objone);
    
    //load one cube face for attaching grass to perform blending
    std::vector<glm::vec3> vertices_cubeface;
    std::vector<glm::vec2> uvs_cubeface;
    std::vector<glm::vec3> normals_cubeface;
    res = loadObjectFile("CubeFace.obj", vertices_cubeface, uvs_cubeface, normals_cubeface);
    
    //load rectangle plane for attaching alligator for blending
    std::vector<glm::vec3> vertices_rectangle;
    std::vector<glm::vec2> uvs_rectangle;
    std::vector<glm::vec3> normals_rectangle;
    res = loadObjectFile("alligatorplane.obj", vertices_rectangle, uvs_rectangle, normals_rectangle);
    
    
    //VBOs for vertices, colors and normals for skybox
    //----------------------------------------------------------
    unsigned int VBO_points;
    glGenBuffers(1, &VBO_points);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    unsigned int VBO_colours;
    glGenBuffers(1, &VBO_colours);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colours);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);
    
    //normals for lighting
    unsigned int VBO_normals;
    glGenBuffers(1, &VBO_normals);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    
    //vbo for texture
    unsigned int VBO_texture;
    glGenBuffers(1, &VBO_texture);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec3), &uvs[0], GL_STATIC_DRAW);
    
    //VBOs for vertices, colors and normals for object one (cube for skybox)
    //----------------------------------------------------------------
    unsigned int VBO_points_objone;
    glGenBuffers(1, &VBO_points_objone);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points_objone);
    glBufferData(GL_ARRAY_BUFFER, vertices_objone.size() * sizeof(glm::vec3), &vertices_objone[0], GL_STATIC_DRAW);
    
    unsigned int VBO_colours_objone;
    glGenBuffers(1, &VBO_colours_objone);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colours_objone);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);
    
    //normals for lighting
    unsigned int VBO_normals_objone;
    glGenBuffers(1, &VBO_normals_objone);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals_objone);
    glBufferData(GL_ARRAY_BUFFER, normals_objone.size() * sizeof(glm::vec3), &normals_objone[0], GL_STATIC_DRAW);
    
    //vbo for texture
    unsigned int VBO_texture_objone;
    glGenBuffers(1, &VBO_texture_objone);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_objone);
    glBufferData(GL_ARRAY_BUFFER, uvs_objone.size() * sizeof(glm::vec3), &uvs_objone[0], GL_STATIC_DRAW);
    

    //the VAO is for skybox
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colours);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    
    //the VAO is for object one
    unsigned int VAO_objone;
    glGenVertexArrays(1, &VAO_objone);
    glBindVertexArray(VAO_objone);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points_objone);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colours_objone);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals_objone);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_objone);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    
    //generating 500 random translation vectors for grass
    unsigned int amount = 500;
    glm::vec3 translations[amount];
    int index = 0;
    float width = 4.5;
    for (int i = 0; i < amount; i++)
    {
        glm::vec3 translation;
        float x, y, z;
        translation.x = (rand() % (int)(2 * width * 100)) / 100.0f - width;
        translation.y = 0.0;
        translation.z = (rand() % (int)(2 * width * 100)) / 100.0f - width;
        translations[index++] = translation;
    }
    
    // store instance data in an array buffer
        // --------------------------------------
        unsigned int instanceVBO;
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * amount, &translations[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    
    //VBOs for vertices, colors and normals for grass
    //----------------------------------------------------------
    unsigned int transparentVAO;
    glGenVertexArrays(1, &transparentVAO);
    
    unsigned int VBO_points_cubeface;
    glGenBuffers(1, &VBO_points_cubeface);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points_cubeface);
    glBufferData(GL_ARRAY_BUFFER, vertices_cubeface.size() * sizeof(glm::vec3), &vertices_cubeface[0], GL_STATIC_DRAW);
    
    //vbo for texture
    unsigned int VBO_texture_cubeface;
    glGenBuffers(1, &VBO_texture_cubeface);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_cubeface);
    glBufferData(GL_ARRAY_BUFFER, uvs_cubeface.size() * sizeof(glm::vec3), &uvs_cubeface[0], GL_STATIC_DRAW);
    
    //the VAO is for transparency of grass
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points_cubeface);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_cubeface);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(3);
    
    // also set instance data
   glEnableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
   glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
    
   
    
    //VBOs for vertices, colors and normals for alligator
    //----------------------------------------------------------
    unsigned int rectangleVAO;
    glGenVertexArrays(1, &rectangleVAO);
    
    unsigned int VBO_points_rectangleface;
    glGenBuffers(1, &VBO_points_rectangleface);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points_rectangleface);
    glBufferData(GL_ARRAY_BUFFER, vertices_rectangle.size() * sizeof(glm::vec3), &vertices_rectangle[0], GL_STATIC_DRAW);
    
    //vbo for texture
    unsigned int VBO_texture_rectangleface;
    glGenBuffers(1, &VBO_texture_rectangleface);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_rectangleface);
    glBufferData(GL_ARRAY_BUFFER, uvs_rectangle.size() * sizeof(glm::vec3), &uvs_rectangle[0], GL_STATIC_DRAW);
    
    //the VAO is for transparency of grass
    glBindVertexArray(rectangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points_rectangleface);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_rectangleface);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(3);

    
    //path for regular texture
    unsigned int Texture_objone = loadTexture("Gator.jpg");
    
    //path for loading texture of swamp
    unsigned int floorTexture = loadTexture("Ground_Soil.png");
    
    //path for loading grass
    unsigned int transparentTexture = loadTexture("Grass_new.png");
    
    //path for loading alligator
    unsigned int alligatorTexture = loadTexture("alligator transparent background.png");
    
    
    //path for cube map
    std::vector<std::string> faces;
    faces.push_back("skybox/right.jpg");
    faces.push_back("skybox/left.jpg");
    faces.push_back("skybox/top.jpg");
    faces.push_back("skybox/bottom.jpg");
    faces.push_back("skybox/front.jpg");
    faces.push_back("skybox/back.jpg");
    unsigned int cubemapTexture = loadCubemap(faces);
    
    /*
    // transparent vegetation locations
    // --------------------------------
    std::vector<glm::vec3> vegetation;
    vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    vegetation.push_back(glm::vec3( 1.5f, 0.0f, 0.51f));
    vegetation.push_back(glm::vec3( 0.0f, 0.0f, 0.7f));
    vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    vegetation.push_back(glm::vec3 (0.5f, 0.0f, -0.6f));
     */

    
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //use the shader here
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    
    objone_shader.use();
    objone_shader.setInt("skybox", 0);
    
    texture_shader.use();
    texture_shader.setInt("ourTexture", 0);
    
    grass_shader.use();
    grass_shader.setInt("ourTexture", 0);
    
    alligator_shader.use();
    alligator_shader.setInt("ourTexture", 0);
    
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // lighting position nd color
        glm::vec3 lightPos(4.0f, 3.0f, 3.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        
        //project, view and model (translate, rotate and scale) matrix
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float) SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0, 0.0, 6.5));
        model = glm::scale(model, glm::vec3(x, y, z));
        
        //draw cube for environment mapping
        objone_shader.use();
        objone_shader.setMat4("model", model);
        objone_shader.setMat4("view", view);
        objone_shader.setMat4("projection", projection);
        objone_shader.setVec3("cameraPos", cameraPos);
        glBindVertexArray(VAO_objone);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture_objone);
        glDrawArrays(GL_TRIANGLES, 0, vertices_objone.size());
        glDepthMask(GL_TRUE);
        glBindVertexArray(0);
        
        //draw plane
        texture_shader.use();
        model = glm::mat4(1.0f);
        texture_shader.setMat4("model", model);
        texture_shader.setMat4("view", view);
        texture_shader.setMat4("projection", projection);
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDepthMask(GL_TRUE);
        glBindVertexArray(0);

        //Draw grass
        grass_shader.use();
        glBindVertexArray(transparentVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        grass_shader.setMat4("view", view);
        grass_shader.setMat4("projection", projection);
        glDrawArraysInstanced(GL_TRIANGLES, 0, vertices_cubeface.size(),amount);

        //draw alligator
        alligator_shader.use();
        glBindVertexArray(rectangleVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, alligatorTexture);
        model = glm::mat4(1.0f);
        alligator_shader.setMat4("model", model);
        alligator_shader.setMat4("view", view);
        alligator_shader.setMat4("projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, vertices_rectangle.size());
        
        //draw skybox
        skyboxShader.use();
        model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0, 0.0, 6.5));
        model = glm::translate(model, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z));
        skyboxShader.setMat4("model", model);
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        glDepthMask(GL_TRUE);
        glBindVertexArray(0); // no need to unbind it every time
        
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO_points);
    glDeleteBuffers(1, &VBO_colours);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // making the scene dynamic
    float cameraSpeed = -0.05f;
    
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            if (cameraPos[1] <=0.0 && cameraFront[1] <=0)
            {
                cameraPos[1] += 0.1;
            }
            else
            {
            cameraPos -= cameraSpeed * cameraFront;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            if (cameraPos[1] <=0.0 && cameraFront[1] >= 0)
            {
                cameraPos[1] += 0.1;
            }
            else
            {
                cameraPos += cameraSpeed * cameraFront;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            
        }
    
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            cameraPos += cameraUp * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            cameraPos -= cameraUp * cameraSpeed;
    
        //scale the object by 0.1 along x,y, and z axis
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            x += 0.1;
            y += 0.1;
            z += 0.1;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            x -= 0.1;
            y -= 0.1;
            z -= 0.1;
        }

    
    //to rotate the object by an angle of 10 radians along z axis
        //if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            rotationAngleZ = rotationAngleZ + 10;
        //if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
            rotationAngleZ = rotationAngleZ - 10;
    
    //to rotate the object by an angle of 10 radians along x axis
        //if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
            rotationAngleY = rotationAngleY + 10;
        //if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            rotationAngleY = rotationAngleY - 10;
        
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: to zoom in and zoom out of the object
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

// to rotate the object using mouse left and right bitton clicks
// -------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        rotationAngleZ += 3;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        rotationAngleZ -= 3;
}

//function to load and create a texture
// -------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

//function to load and create a cube map texture
//order of textures
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
// -------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
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
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
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


