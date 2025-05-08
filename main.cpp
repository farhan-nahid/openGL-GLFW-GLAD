#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

// Camera settings
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;

glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
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

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Fan state (on/off) for each fan
std::vector<bool> fanStates(3, false);

// Mouse click callback for switch interaction
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        // Convert to normalized device coordinates (NDC) for rough interaction
        float x = (2.0f * xpos) / 800.0f - 1.0f;
        float y = 1.0f - (2.0f * ypos) / 600.0f;
        // Check switch positions (approximate based on window size)
        std::vector<glm::vec3> switchPositions = {
            glm::vec3(-4.0f, 1.5f, -4.0f), // Switch 1 near left wall
            glm::vec3(0.0f, 1.5f, -4.0f),  // Switch 2 near center
            glm::vec3(4.0f, 1.5f, -4.0f)   // Switch 3 near right wall
        };
        for (size_t i = 0; i < switchPositions.size(); ++i) {
            glm::vec4 clipSpace = glm::vec4(switchPositions[i], 1.0f);
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            glm::vec4 worldSpace = glm::inverse(projection * view) * clipSpace;
            worldSpace /= worldSpace.w;
            float screenX = (worldSpace.x + 1.0f) * 400.0f;
            float screenY = (1.0f - worldSpace.y) * 300.0f;
            if (std::abs(screenX - xpos) < 20 && std::abs(screenY - ypos) < 20) {
                fanStates[i] = !fanStates[i]; // Toggle fan state
            }
        }
    }
}

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 objectColor;
    void main() {
        FragColor = vec4(objectColor, 1.0);
    }
)";

class Shader {
public:
    unsigned int ID;
    Shader() {
        unsigned int vertex, fragment;
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexShaderSource, NULL);
        glCompileShader(vertex);

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragment);

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() { glUseProgram(ID); }
    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
};

class Object {
public:
    unsigned int VAO, VBO;
    std::vector<float> vertices;
    glm::vec3 position;
    glm::vec3 color;

    Object(const std::vector<float>& verts, glm::vec3 pos, glm::vec3 col) : position(pos), color(col) {
        vertices = verts;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw(Shader& shader) {
        shader.use();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        shader.setMat4("model", model);
        shader.setVec3("objectColor", color);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
        glBindVertexArray(0);
    }
};



// Function to create a chair
Object createChair(glm::vec3 position) {
    std::vector<float> chairVertices = {
        // Seat
        -0.3f, -0.5f, -0.3f,  0.3f, -0.5f, -0.3f,  0.3f, -0.4f, -0.3f,
         0.3f, -0.4f, -0.3f, -0.3f, -0.4f, -0.3f, -0.3f, -0.5f, -0.3f,
        -0.3f, -0.5f,  0.3f,  0.3f, -0.5f,  0.3f,  0.3f, -0.4f,  0.3f,
         0.3f, -0.4f,  0.3f, -0.3f, -0.4f,  0.3f, -0.3f, -0.5f,  0.3f,
        -0.3f, -0.4f,  0.3f, -0.3f, -0.4f, -0.3f, -0.3f, -0.5f, -0.3f,
        -0.3f, -0.5f, -0.3f, -0.3f, -0.5f,  0.3f, -0.3f, -0.4f,  0.3f,
         0.3f, -0.4f,  0.3f,  0.3f, -0.4f, -0.3f,  0.3f, -0.5f, -0.3f,
         0.3f, -0.5f, -0.3f,  0.3f, -0.5f,  0.3f,  0.3f, -0.4f,  0.3f,
        -0.3f, -0.5f, -0.3f,  0.3f, -0.5f, -0.3f,  0.3f, -0.5f,  0.3f,
         0.3f, -0.5f,  0.3f, -0.3f, -0.5f,  0.3f, -0.3f, -0.5f, -0.3f,
        -0.3f, -0.4f, -0.3f,  0.3f, -0.4f, -0.3f,  0.3f, -0.4f,  0.3f,
         0.3f, -0.4f,  0.3f, -0.3f, -0.4f,  0.3f, -0.3f, -0.4f, -0.3f,
        // Backrest
        -0.3f, -0.4f, -0.3f,  0.3f, -0.4f, -0.3f,  0.3f,  0.1f, -0.3f,
         0.3f,  0.1f, -0.3f, -0.3f,  0.1f, -0.3f, -0.3f, -0.4f, -0.3f
    };
    return Object(chairVertices, position, glm::vec3(0.6f, 0.3f, 0.0f));
}

// Function to create a table
Object createTable(glm::vec3 position) {
    std::vector<float> tableVertices = {
        // Tabletop
        -0.8f, -0.1f, -0.4f,  0.8f, -0.1f, -0.4f,  0.8f,  0.0f, -0.4f,
         0.8f,  0.0f, -0.4f, -0.8f,  0.0f, -0.4f, -0.8f, -0.1f, -0.4f,
        -0.8f, -0.1f,  0.4f,  0.8f, -0.1f,  0.4f,  0.8f,  0.0f,  0.4f,
         0.8f,  0.0f,  0.4f, -0.8f,  0.0f,  0.4f, -0.8f, -0.1f,  0.4f,
        -0.8f,  0.0f,  0.4f, -0.8f,  0.0f, -0.4f, -0.8f, -0.1f, -0.4f,
        -0.8f, -0.1f, -0.4f, -0.8f, -0.1f,  0.4f, -0.8f,  0.0f,  0.4f,
         0.8f,  0.0f,  0.4f,  0.8f,  0.0f, -0.4f,  0.8f, -0.1f, -0.4f,
         0.8f, -0.1f, -0.4f,  0.8f, -0.1f,  0.4f,  0.8f,  0.0f,  0.4f,
        -0.8f, -0.1f, -0.4f,  0.8f, -0.1f, -0.4f,  0.8f, -0.1f,  0.4f,
         0.8f, -0.1f,  0.4f, -0.8f, -0.1f,  0.4f, -0.8f, -0.1f, -0.4f,
        -0.8f,  0.0f, -0.4f,  0.8f,  0.0f, -0.4f,  0.8f,  0.0f,  0.4f,
         0.8f,  0.0f,  0.4f, -0.8f,  0.0f,  0.4f, -0.8f,  0.0f, -0.4f,
        // Legs
        -0.7f, -0.1f, -0.3f, -0.6f, -0.1f, -0.3f, -0.6f, -0.5f, -0.3f,
        -0.6f, -0.5f, -0.3f, -0.7f, -0.5f, -0.3f, -0.7f, -0.1f, -0.3f,
         0.6f, -0.1f, -0.3f,  0.7f, -0.1f, -0.3f,  0.7f, -0.5f, -0.3f,
         0.7f, -0.5f, -0.3f,  0.6f, -0.5f, -0.3f,  0.6f, -0.1f, -0.3f,
        -0.7f, -0.1f,  0.3f, -0.6f, -0.1f,  0.3f, -0.6f, -0.5f,  0.3f,
        -0.6f, -0.5f,  0.3f, -0.7f, -0.5f,  0.3f, -0.7f, -0.1f,  0.3f,
         0.6f, -0.1f,  0.3f,  0.7f, -0.1f,  0.3f,  0.7f, -0.5f,  0.3f,
         0.7f, -0.5f,  0.3f,  0.6f, -0.5f,  0.3f,  0.6f, -0.1f,  0.3f
    };
    return Object(tableVertices, position, glm::vec3(0.4f, 0.2f, 0.0f));
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Classroom", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glEnable(GL_DEPTH_TEST);
    Shader shader;

    // Cube vertices for fan and switch
    std::vector<float> cubeVertices = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    };

    // Switch vertices (small cube)
    std::vector<float> switchVertices = {
        -0.1f, -0.1f, -0.1f,  0.1f, -0.1f, -0.1f,  0.1f,  0.1f, -0.1f,
         0.1f,  0.1f, -0.1f, -0.1f,  0.1f, -0.1f, -0.1f, -0.1f, -0.1f,
        -0.1f, -0.1f,  0.1f,  0.1f, -0.1f,  0.1f,  0.1f,  0.1f,  0.1f,
         0.1f,  0.1f,  0.1f, -0.1f,  0.1f,  0.1f, -0.1f, -0.1f,  0.1f,
        -0.1f,  0.1f,  0.1f, -0.1f,  0.1f, -0.1f, -0.1f, -0.1f, -0.1f,
        -0.1f, -0.1f, -0.1f, -0.1f, -0.1f,  0.1f, -0.1f,  0.1f,  0.1f,
         0.1f,  0.1f,  0.1f,  0.1f,  0.1f, -0.1f,  0.1f, -0.1f, -0.1f,
         0.1f, -0.1f, -0.1f,  0.1f, -0.1f,  0.1f,  0.1f,  0.1f,  0.1f,
        -0.1f, -0.1f, -0.1f,  0.1f, -0.1f, -0.1f,  0.1f, -0.1f,  0.1f,
         0.1f, -0.1f,  0.1f, -0.1f, -0.1f,  0.1f, -0.1f, -0.1f, -0.1f,
        -0.1f,  0.1f, -0.1f,  0.1f,  0.1f, -0.1f,  0.1f,  0.1f,  0.1f,
         0.1f,  0.1f,  0.1f, -0.1f,  0.1f,  0.1f, -0.1f,  0.1f, -0.1f
    };

    // Wall vertices
    std::vector<float> wallVertices = {
        -5.0f, -2.0f,  0.0f,  5.0f, -2.0f,  0.0f,  5.0f,  2.0f,  0.0f,
         5.0f,  2.0f,  0.0f, -5.0f,  2.0f,  0.0f, -5.0f, -2.0f,  0.0f
    };

    // Whiteboard vertices
    std::vector<float> whiteboardVertices = {
        -2.0f, -0.5f,  0.0f,  2.0f, -0.5f,  0.0f,  2.0f,  1.0f,  0.0f,
         2.0f,  1.0f,  0.0f, -2.0f,  1.0f,  0.0f, -2.0f, -0.5f,  0.0f
    };

    // Teacher's desk vertices
    std::vector<float> teacherDeskVertices = {
        // Tabletop
        -1.2f, -0.1f, -0.6f,  1.2f, -0.1f, -0.6f,  1.2f,  0.0f, -0.6f,
         1.2f,  0.0f, -0.6f, -1.2f,  0.0f, -0.6f, -1.2f, -0.1f, -0.6f,
        -1.2f, -0.1f,  0.6f,  1.2f, -0.1f,  0.6f,  1.2f,  0.0f,  0.6f,
         1.2f,  0.0f,  0.6f, -1.2f,  0.0f,  0.6f, -1.2f, -0.1f,  0.6f,
        -1.2f,  0.0f,  0.6f, -1.2f,  0.0f, -0.6f, -1.2f, -0.1f, -0.6f,
        -1.2f, -0.1f, -0.6f, -1.2f, -0.1f,  0.6f, -1.2f,  0.0f,  0.6f,
         1.2f,  0.0f,  0.6f,  1.2f,  0.0f, -0.6f,  1.2f, -0.1f, -0.6f,
         1.2f, -0.1f, -0.6f,  1.2f, -0.1f,  0.6f,  1.2f,  0.0f,  0.6f,
        -1.2f, -0.1f, -0.6f,  1.2f, -0.1f, -0.6f,  1.2f, -0.1f,  0.6f,
         1.2f, -0.1f,  0.6f, -1.2f, -0.1f,  0.6f, -1.2f, -0.1f, -0.6f,
        -1.2f,  0.0f, -0.6f,  1.2f,  0.0f, -0.6f,  1.2f,  0.0f,  0.6f,
         1.2f,  0.0f,  0.6f, -1.2f,  0.0f,  0.6f, -1.2f,  0.0f, -0.6f,
        // Legs
        -1.1f, -0.1f, -0.5f, -1.0f, -0.1f, -0.5f, -1.0f, -0.5f, -0.5f,
        -1.0f, -0.5f, -0.5f, -1.1f, -0.5f, -0.5f, -1.1f, -0.1f, -0.5f,
         1.0f, -0.1f, -0.5f,  1.1f, -0.1f, -0.5f,  1.1f, -0.5f, -0.5f,
         1.1f, -0.5f, -0.5f,  1.0f, -0.5f, -0.5f,  1.0f, -0.1f, -0.5f,
        -1.1f, -0.1f,  0.5f, -1.0f, -0.1f,  0.5f, -1.0f, -0.5f,  0.5f,
        -1.0f, -0.5f,  0.5f, -1.1f, -0.5f,  0.5f, -1.1f, -0.1f,  0.5f,
         1.0f, -0.1f,  0.5f,  1.1f, -0.1f,  0.5f,  1.1f, -0.5f,  0.5f,
         1.1f, -0.5f,  0.5f,  1.0f, -0.5f,  0.5f,  1.0f, -0.1f,  0.5f
    };

    // Create objects
    std::vector<Object> objects;
    // Chairs (10 chairs in two rows)
    for (int i = 0; i < 5; ++i) {
        objects.push_back(createChair(glm::vec3(-4.0f + i * 1.5f, -0.5f, 2.0f))); // Row 1
        objects.push_back(createChair(glm::vec3(-4.0f + i * 1.5f, -0.5f, 1.0f))); // Row 2
    }
    // Tables (5 tables in two rows)
    for (int i = 0; i < 3; ++i) {
        objects.push_back(createTable(glm::vec3(-3.5f + i * 2.0f, -0.5f, 1.5f))); // Row 1
    }
    for (int i = 0; i < 2; ++i) {
        objects.push_back(createTable(glm::vec3(-2.5f + i * 2.0f, -0.5f, 0.5f))); // Row 2
    }
    // Teacher's desk
    objects.emplace_back(teacherDeskVertices, glm::vec3(0.0f, -0.5f, -3.5f), glm::vec3(0.4f, 0.2f, 0.0f));
    // Fans (3 fans on ceiling)
    objects.emplace_back(cubeVertices, glm::vec3(-4.0f, 2.0f, -4.0f), fanStates[0] ? glm::vec3(0.5f, 0.5f, 0.5f) : glm::vec3(0.0f, 0.0f, 0.0f));
    objects.emplace_back(cubeVertices, glm::vec3(0.0f, 2.0f, -4.0f), fanStates[1] ? glm::vec3(0.5f, 0.5f, 0.5f) : glm::vec3(0.0f, 0.0f, 0.0f));
    objects.emplace_back(cubeVertices, glm::vec3(4.0f, 2.0f, -4.0f), fanStates[2] ? glm::vec3(0.5f, 0.5f, 0.5f) : glm::vec3(0.0f, 0.0f, 0.0f));
    // Switches (3 switches on back wall)
    objects.emplace_back(switchVertices, glm::vec3(-4.0f, 1.5f, -4.0f), glm::vec3(0.8f, 0.8f, 0.0f));
    objects.emplace_back(switchVertices, glm::vec3(0.0f, 1.5f, -4.0f), glm::vec3(0.8f, 0.8f, 0.0f));
    objects.emplace_back(switchVertices, glm::vec3(4.0f, 1.5f, -4.0f), glm::vec3(0.8f, 0.8f, 0.0f));
    // Walls (brown)
    objects.emplace_back(wallVertices, glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.5f, 0.3f, 0.1f));
    std::vector<float> leftWallVertices = wallVertices;
    for (size_t i = 0; i < leftWallVertices.size(); i += 3) {
        float x = leftWallVertices[i];
        float z = leftWallVertices[i + 2];
        leftWallVertices[i] = z;
        leftWallVertices[i + 2] = -x;
    }
    objects.emplace_back(leftWallVertices, glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.3f, 0.1f));
    std::vector<float> rightWallVertices = wallVertices;
    for (size_t i = 0; i < rightWallVertices.size(); i += 3) {
        float x = rightWallVertices[i];
        float z = rightWallVertices[i + 2];
        rightWallVertices[i] = -z;
        rightWallVertices[i + 2] = x;
    }
    objects.emplace_back(rightWallVertices, glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.3f, 0.1f));
    // Whiteboard
    objects.emplace_back(whiteboardVertices, glm::vec3(0.0f, 0.0f, -4.9f), glm::vec3(1.0f, 1.0f, 1.0f));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    shader.use();
    shader.setMat4("projection", projection);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", view);

        for (size_t i = 0; i < objects.size(); ++i) {
            if (i >= 10 && i <= 12) { // Fans (indices 10, 11, 12)
                objects[i].color = fanStates[i - 10] ? glm::vec3(0.5f, 0.5f, 0.5f) : glm::vec3(0.0f, 0.0f, 0.0f);
            }
            objects[i].draw(shader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}