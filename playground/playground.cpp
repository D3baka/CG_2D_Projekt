#include "playground.h"

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cassert>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

//Include additional libraries
#include <vector>
#include <chrono>
#include <cmath>
#include <string>
using namespace std;

#include <common/shader.hpp>
#include "parse_stl.h"

//classes

class GameObject {
public:
    

    //modified
    GLuint vertexbuffer[2];

    string filename;

    GLuint VertexArrayID;
    GLuint vertexbuffer_size;
    glm::mat4 translation;
    float speed;
    float radius;
    bool isActive;

   
    
	virtual void update() = 0;
    void draw() {

        // Use our shader
        glUseProgram(programID);
		
        // Send our transformation to the currently bound shader, 
           // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(MatrixIDM, 1, GL_FALSE, &M[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
        glVertexAttribPointer(
            1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );





        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, vertexbuffer_size * 3); // 3 indices starting at 0 -> 1 triangle

    }
   

    bool initializeVertexbuffer()
    {
        std::cout << "test " << filename << std::endl;
        glGenVertexArrays(1, &VertexArrayID);
        std::cout << "test2 " << filename << std::endl;
        glBindVertexArray(VertexArrayID);
        std::cout << "test3 " << filename << std::endl;
        //create vertex and normal data 
        std::vector< glm::vec3 > vertices = std::vector< glm::vec3 >();
        std::vector< glm::vec3 > normals = std::vector< glm::vec3 >();
        std::cout << "parsing stl " << filename << std::endl;
		parseStl(vertices, normals, "../res/" + filename + ".stl");
        vertexbuffer_size = vertices.size() * sizeof(glm::vec3);

        // print normals to console

        glGenBuffers(2, vertexbuffer); //generate two buffers, one for the vertices, one for the normals

        //fill first buffer (vertices)
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        //fill second buffer (normals)
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        return true;

        
    }

    bool cleanupVertexbuffer()
    {
        // Cleanup VBO
        glDeleteVertexArrays(1, &VertexArrayID);
        return true;
    }

    bool cleanupColorbuffer() {
        // Cleanup VBO
        glDeleteBuffers(1, &colorbuffer);
        return true;
    }
};

class Turret : public GameObject {
public:
    Turret(string name){
        std::cout << "creating turret with name: " << name << std::endl;
        filename = name;
        std::cout << "initializing Vertexbuffer " << name << std::endl;
        initializeVertexbuffer();
    }
    
    void update() override{
        glm::mat4 transformation = glm::mat4(1.0f);
        transformation[0][0] = 1.0; transformation[1][0] = 0.0; transformation[2][0] = 0.0; transformation[3][0] = 0.0;
        transformation[0][1] = 0.0; transformation[1][1] = 1.0; transformation[2][1] = 0.0; transformation[3][1] = 0.0;
        transformation[0][2] = 0.0; transformation[1][2] = 0.0; transformation[2][2] = 1.0; transformation[3][2] = 0.0;
        transformation[0][3] = 0.0; transformation[1][3] = 0.0; transformation[2][3] = 0.0; transformation[3][3] = 1.0;
        initializeMVPTransformation(0.0, curr_angle_x, transformation);
        draw();
    }
   
};

class Barrels : public GameObject {
public:
    Barrels(string name) {
        std::cout << "creating barrels with name: " << name << std::endl;
        filename = name;
        std::cout << "initializing Vertexbuffer " << name << std::endl;
        initializeVertexbuffer();
    }
    void update() override{
        glm::mat4 transformation = glm::mat4(1.0f);
        transformation[0][0] = 1.0; transformation[1][0] = 0.0; transformation[2][0] = 0.0; transformation[3][0] = 0.0;
        transformation[0][1] = 0.0; transformation[1][1] = 1.0; transformation[2][1] = 0.0; transformation[3][1] = 0.415;
        transformation[0][2] = 0.0; transformation[1][2] = 0.0; transformation[2][2] = 1.0; transformation[3][2] = 0.0;
        transformation[0][3] = 0.0; transformation[1][3] = 0.0; transformation[2][3] = 0.0; transformation[3][3] = 1.0;

        //std::cout << "offset local: " << transformation[3][0] << " " << transformation[3][1] << " " << std::endl;
		initializeMVPTransformation(curr_angle_y, curr_angle_x, transformation);
        draw();
    }

};


//Global variables
std::vector<GameObject*> gameObjects;





int main(void)
{
	std::cout << "Hello World" << std::endl;

    std::cout << "initializing Window" << std::endl;
    //Initialize window
    bool windowInitialized = initializeWindow();
    if (!windowInitialized) return -1;
	
    std::cout << "Create Gameobjects" << std::endl;
    //create Gameojects
	Turret turret = Turret("KGVturret");
	Barrels barrels = Barrels("KGVsecbatGuns");
    std::cout << "Pushing Gameobjects" << std::endl;
	
    gameObjects.push_back(&turret);
	gameObjects.push_back(&barrels);




    

    /*//Initialize vertex buffer
    bool vertexbufferInitialized = initializeVertexbuffer();
    if (!vertexbufferInitialized) return -1;
    */

    glEnable(GL_DEPTH_TEST);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

    

    //start animation loop until escape key is pressed
	std::cout << "Starting animation loop" << std::endl;
    do {

        updateAnimationLoop();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);
    

    //Cleanup and close window
    for (int i = 0; i < gameObjects.size(); i++) {
        gameObjects.at(i)->cleanupVertexbuffer();
        gameObjects.at(i)->cleanupColorbuffer();
    }
	
    glDeleteProgram(programID);
    closeWindow();

    return 0;
}

void parseStl(std::vector< glm::vec3 >& vertices,
    std::vector< glm::vec3 >& normals,
    std::string stl_file_name)
{
    stl::stl_data info = stl::parse_stl(stl_file_name);
    std::vector<stl::triangle> triangles = info.triangles;
    for (int i = 0; i < info.triangles.size(); i++) {
        stl::triangle t = info.triangles.at(i);
        glm::vec3 triangleNormal = glm::vec3(t.normal.x,
            t.normal.y,
            t.normal.z);
        //add vertex and normal for point 1:
        vertices.push_back(glm::vec3(t.v1.x, t.v1.y, t.v1.z));
        normals.push_back(triangleNormal);
        //add vertex and normal for point 2:
        vertices.push_back(glm::vec3(t.v2.x, t.v2.y, t.v2.z));
        normals.push_back(triangleNormal);
        //add vertex and normal for point 3:
        vertices.push_back(glm::vec3(t.v3.x, t.v3.y, t.v3.z));
        normals.push_back(triangleNormal);
    }
}




void updateAnimationLoop()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    if (glfwGetKey(window, GLFW_KEY_W) && curr_angle_y > 0) curr_angle_y -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_S)  && (curr_angle_y < 1.0471976)) curr_angle_y += 0.01;
    if (glfwGetKey(window, GLFW_KEY_A)) curr_angle_x += 0.01;
    if (glfwGetKey(window, GLFW_KEY_D)) curr_angle_x -= 0.01;

	//needs loop where each gameobject is updated, according to the rotation information
	//call update for every gameobject in gameObjects
	for (int i = 0; i < gameObjects.size(); i++) {
		gameObjects.at(i)->update();
	}
	
   

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool initializeWindow()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "Test: Guns", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return false;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    return true;
}

bool initializeMVPTransformation(float angle_x, float angle_y, glm::mat4 offset)
{
    // Get a handle for our "MVP" uniform
    GLuint MatrixIDnew = glGetUniformLocation(programID, "MVP");
    MatrixID = MatrixIDnew;

    MatrixIDM = glGetUniformLocation(programID, "M");


    // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 500.0f);
    //glm::mat4 Projection = glm::frustum(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
    // Camera matrix
    glm::mat4 View = glm::lookAt(
        glm::vec3(2, 1, 0), // Camera is at (4,3,-3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
	
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    float fix_angle = -1.5708;
    Model = glm::rotate(Model, fix_angle , glm::vec3(1.0f, 0.0f, 0.0f));

    Model = glm::rotate(Model, angle_y, glm::vec3(0.0f, 0.0f, 1.0f));
    Model = glm::rotate(Model, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));

    
    glm::mat4 transformation = offset;
	
	//std::cout << "offset: " << transformation[3][0] << " " << transformation[3][1] << " "<< std::endl;
    
    

    

    // Our ModelViewProjection : multiplication of our 3 matrices
	
    MVP = Projection * View * transformation * Model ; // Remember, matrix multiplication is the other way around
    M = Model * transformation;

    return true;

}

bool closeWindow()
{
    glfwTerminate();
    return true;
}