#include "Shader.hpp"
#include "utils.hpp"
#include "Object.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cameraMove(float* model, unsigned int program, GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int	SCR_WIDTH = 800;
const unsigned int	SCR_HEIGHT = 600;
bool                g_rotation = false;
int                 g_centreMode = 2;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << ".obj file required" << std::endl;
        return -1;
    }
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

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
    Shader  shader("./src/shaders/vertexShaderSource.glsl", "./src/shaders/fragmentShaderSource.glsl");

    // set up vertex data
    // ------------------
    Object  object(argv[1]);
    try
    {
        object.setObject();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    // render loop
    // -----------
    float   model[16];
    loadIdentity(model);
    while (!glfwWindowShouldClose(window))
    {
        // render
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();

        // Apply camera move & rotation
        // ----------------------------
        cameraMove(model, shader.ID, window);

        // Draw object
        // -----------
        object.drawObject(g_centreMode);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void    cameraMove(float* model, unsigned int program, GLFWwindow* window)
{
    // MVP 행렬 계산 및 전달
    // -----------------
    float   view[16], proj[16], vp[16], mvp[16], rotation[16];
    makeLookAt(view, 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    makePerspective(proj, 45.0f, (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

    if (g_rotation)
    {
        makeRotationY(rotation, 1.0f);
        multiplyMatrix(model, model, rotation);
    }

    multiplyMatrix(vp, proj, view);
    multiplyMatrix(mvp, vp, model);
    // uniform 전달
    // -----------
    int    loc = glGetUniformLocation(program, "uMVP");
    if (loc == -1)
    {
        std::cerr << "Warning: uMVP uniform not found in shader!" << std::endl;
        glfwSetWindowShouldClose(window, true);
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, mvp);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void    key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
        g_centreMode = 0;
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        g_centreMode = 1;
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        g_centreMode = 2;
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        g_rotation = !g_rotation;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void    framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}