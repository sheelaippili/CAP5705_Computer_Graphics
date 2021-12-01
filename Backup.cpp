#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// Include GLM and glm for matrix operations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//including stb_image.h to handle several file formats for texture mapping

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace glm;

#include <iostream>
#include <vector>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//camera position
// camera
glm::vec3 cameraPos   = glm::vec3(4.0f, 3.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(-4.0f, -3.0f, -3.0f);
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
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/* copy a shader from a plain text file into a character array */
bool file_to_string( const char* fn, char* shader_string, int length ) {
  FILE* file = fopen( fn, "r" );
  if ( !file ) {
    //gl_log_err( "eror while opening file %s\n", fn );
    return false;
  }
  size_t count = fread( shader_string, 1, length - 1, file );
  if ( (int)count >= length - 1 ) {
      //gl_log_err( "warning: file too big\n", fn );
      
  }
  if ( ferror( file ) ) {
    //gl_log_err( "ERROR: reading shader file %s\n", fn );
    fclose( file );
    return false;
  }
    shader_string[count] = 0;
  fclose( file );
  return true;
}

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
//Assignment code

int main()
{
    //take user input to select the type of shader and the the .obj file to load
    int inputShader;
    int objType;
    
    std::cout << "Select one of the following shaders\n";
    std::cout << "Enter 1 for Phong Shader\n";
    std::cout << "Enter 2 for Gouraud Shader\n";
    std::cout << "Enter 3 for z-buffer (depth testing)\n";
    std::cout << "Enter 4 general shader\n";
    std::cout << "Enter 5 for texture shader\n";
    std:: cin >> inputShader;
    
    std::cout << "Select one of the following objects\n";
    std::cout << "Enter 7 for cube\n";
    std::cout << "Enter 8 for cylinder\n";
    std::cout << "Enter 9 for monkey\n";
    std:: cin >> objType;
    
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment2", NULL, NULL);
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
    
    //read vertex and fragment shaders
    char ver_shader[1024 * 256];
    char fra_shader[1024 * 256];
    const GLchar* s;
    
    switch(inputShader)
    {
        case 1:
        {
            file_to_string( "PhongVertexShader.vs", ver_shader, 1024 * 256 );
            file_to_string( "PhongFragmentShader.fs", fra_shader, 1024 * 256);
            break;
        }
        case 2:
        {
            file_to_string( "GouraudVertexShader.vs", ver_shader, 1024 * 256 );
            file_to_string( "GouraudFragmentShader.fs", fra_shader, 1024 * 256);
            break;
        }
        case 3:
        {
            file_to_string( "DTVertexShader.vs", ver_shader, 1024 * 256 );
            file_to_string( "DTFragmentShader.fs", fra_shader, 1024 * 256);
            break;
        }
        case 4:
        {
            file_to_string( "VertexShader.vs", ver_shader, 1024 * 256 );
            file_to_string( "FragmentShader.fs", fra_shader, 1024 * 256);
            break;
        }
        case 5:
        {
            file_to_string( "TextureVertexShader.vs", ver_shader, 1024 * 256 );
            file_to_string( "TextureFragmentShader.fs", fra_shader, 1024 * 256);
            break;
        }
        default:
        {
            std::cout << "Invalid input\n";
            file_to_string( "PhongVertexShader.vs", ver_shader, 1024 * 256 );
            file_to_string( "PhongFragmentShader.fs", fra_shader, 1024 * 256);
            break;
        }
    }

    // build and compile our shader program
    // ------------------------------------
    
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    s  = (const GLchar*)ver_shader;
    glShaderSource(vertexShader, 1, &s, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    s  = (const GLchar*)fra_shader;
    glShaderSource(fragmentShader, 1, &s, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glBindAttribLocation(shaderProgram, 0, "aPos");
    glBindAttribLocation(shaderProgram, 1, "vertex_colour");
    glBindAttribLocation(shaderProgram, 2, "aNormal");
    glBindAttribLocation(shaderProgram, 3, "aTexCoord");

    glLinkProgram(shaderProgram);
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    int lightPos_location = glGetUniformLocation(shaderProgram, "lightPos" );
    int lightColor_location =glGetUniformLocation(shaderProgram,"lightColor" );
    int model_location = glGetUniformLocation( shaderProgram, "model" );
    int view_location = glGetUniformLocation( shaderProgram, "view" );
    int projection_location = glGetUniformLocation( shaderProgram, "projection" );
    glUseProgram( shaderProgram );

    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    
    unsigned int indices[] = {  // note that we start from 0!
        5,1,4,
        5,4,8,
        3,7,8,
        3,8,4,
        2,6,3,
        6,7,3,
        1,5,2,
        5,6,2,
        5,8,6,
        8,7,6,
        1,2,3,
        1,3,4
    };
     
    
    float colours[] = {
        //1st triangle
      0.4f, 0.0f,  0.0f,
      0.4f, 0.0f,  0.0f,
      0.4f, 0.0f,  0.0f,

    };
    
    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals; // Won't be used at the moment.
    
    switch(objType)
    {
        case 7:
        {
            bool res = loadObjectFile("seperatetriangles.obj", vertices, uvs, normals);
            break;
        }
        case 8:
        {
            bool res = loadObjectFile("cylinder.obj", vertices, uvs, normals);
            break;
        }
        case 9:
        {
            bool res = loadObjectFile("monkey.obj", vertices, uvs, normals);
            break;
        }
        default:
        {
            std::cout << "Invalid input\n";
            bool res = loadObjectFile("seperatetriangles.obj", vertices, uvs, normals);
            break;
        }
    }
    
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    
    //VBOs for vertices, colors and normals
    unsigned int VBO_points;
    glGenBuffers(1, &VBO_points);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    unsigned int VBO_colours;
    glGenBuffers(1, &VBO_colours);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colours);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);
    
    //normals for lighting (phong and gouraud
    unsigned int VBO_normals;
    glGenBuffers(1, &VBO_normals);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    
    //vbo for texture
    unsigned int VBO_texture;
    glGenBuffers(1, &VBO_texture);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec3), &uvs[0], GL_STATIC_DRAW);
    

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colours);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // texture coord attribute
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    

    // load and create a texture
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = 
    stbi_load("/Users/sheelaippili/Desktop/Assignment3_deliverable/Assignment3/Gator.jpg", &width, &height, &nrChannels, 0);
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
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        
        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // draw our first triangle
        glUseProgram(shaderProgram);
        
        // lighting position nd color
        glm::vec3 lightPos(4.0f, 3.0f, 3.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

        
        // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float) SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
          
        // Or, for an ortho camera :
        //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
        
        glm::mat4 view = glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        //rotating the matrix along Z-Axis
        model = glm::rotate(model, glm::radians(rotationAngleZ), glm::vec3(0.0, 0.0, 1.0));
        model = glm::rotate(model, glm::radians(rotationAngleY), glm::vec3(0.0, 1.0, 0.0));
        //Scaling the cube
        model = glm::scale(model, glm::vec3(x, y, z));
        //glm::mat4 mvp = projection * view * model;
        
        //Assignment 3 - Model view transformation - code start
        glUniformMatrix4fv( projection_location, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv( view_location, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv( model_location, 1, GL_FALSE, &model[0][0]);
        glUniform3fv( lightPos_location, 1, &lightPos[0] );
        glUniform3fv( lightColor_location, 1, &lightColor[0]);
        //Assignment 3 - Model view transformation - code end
        
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        
        //if VBO
        //glDrawArrays(GL_TRIANGLES, 0, numVertices);
        //if .obj file
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        //if EBO
        //glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
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
    glDeleteProgram(shaderProgram);

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
    
    // to scale the cube up, down, move the object left, right up and down
    float cameraSpeed = -0.05f;
    
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraUp * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraUp * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    
        //scale the object by 0.1 along x,y, and z axis
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            x += 0.1;
            y += 0.1;
            z += 0.1;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            x -= 0.1;
            y -= 0.1;
            z -= 0.1;
        }

    
    //to rotate the object by an angle of 10 radians along z axis
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            rotationAngleZ = rotationAngleZ + 10;
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
            rotationAngleZ = rotationAngleZ - 10;
    
    //to rotate the object by an angle of 10 radians along x axis
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
            rotationAngleY = rotationAngleY + 10;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        rotationAngleZ += 3;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        rotationAngleZ -= 3;
}




