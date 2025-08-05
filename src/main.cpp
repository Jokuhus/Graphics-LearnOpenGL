#include "Shader.hpp"
#include "utils.hpp"
#include "Object.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(Object& object);
void cameraMove(const float* model, unsigned int uMVPLoc);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int	SCR_WIDTH = 800;
const unsigned int	SCR_HEIGHT = 600;
bool                g_translate[7] = {false}, g_rotation[7] = {false}, g_textureMode = false;

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

    unsigned int    uMVPLoc = glGetUniformLocation(shader.ID, "uMVP");
    unsigned int    modelLoc = glGetUniformLocation(shader.ID, "model");
    unsigned int    lightPosLoc = glGetUniformLocation(shader.ID, "lightPos");
    unsigned int    lightColorLoc = glGetUniformLocation(shader.ID, "lightColor");
    unsigned int    objectColorLoc = glGetUniformLocation(shader.ID, "objectColor");
    unsigned int    textureRatioLoc = glGetUniformLocation(shader.ID, "textureRatio");
    unsigned int    bumpSamplerLoc = glGetUniformLocation(shader.ID, "BumpSampler");
    unsigned int    diffuseSamplerLoc = glGetUniformLocation(shader.ID, "DiffuseSampler");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // render
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();

        process_input(object);

        // Apply camera move & rotation
        // ----------------------------
        float   model[16];
        object.getModelMatrix(model);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);

        cameraMove(model, uMVPLoc);

        glUniform3f(lightPosLoc, 3.0f, 3.0f, 5.0f);
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
        glUniform3f(objectColorLoc, 0.7f, 0.7f, 0.7f);
        glUniform1f(textureRatioLoc, object.getTextureRatio());

        // Draw object
        // -----------
        object.updateTextureBlendRatio();
        object.drawObject(bumpSamplerLoc, diffuseSamplerLoc);

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

void    cameraMove(const float* model, unsigned int uMVPLoc)
{
    // MVP 행렬 계산 및 전달
    // -----------------
    float   view[16], proj[16], mvp[16];
    makeLookAt(view, 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    makePerspective(proj, 45.0f, (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

    multiplyMatrix(mvp, proj, view);
    multiplyMatrix(mvp, mvp, model);
    // uniform 전달
    // -----------
    glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, mvp);
}

void    process_input(Object& object)
{
    if (g_translate[0] && !g_translate[1])
        object.move(MOVE_RIGHT);
    if (g_translate[1] && !g_translate[0])
        object.move(MOVE_LEFT);
    if (g_translate[2] && !g_translate[3])
        object.move(MOVE_UP);
    if (g_translate[3] && !g_translate[2])
        object.move(MOVE_DOWN);
    if (g_translate[4] && !g_translate[5])
        object.move(MOVE_CLOSE);
    if (g_translate[5] && !g_translate[4])
        object.move(MOVE_FAR);
    if (g_translate[6])
        object.move(MOVE_RESET);
    if (g_rotation[0] && !g_rotation[1])
        object.rotate(ROTATE_CLOCK_Y);
    if (g_rotation[1] && !g_rotation[0])
        object.rotate(ROTATE_ANTICLOCK_Y);
    if (g_rotation[2] && !g_rotation[3])
        object.rotate(ROTATE_CLOCK_X);
    if (g_rotation[3] && !g_rotation[2])
        object.rotate(ROTATE_ANTICLOCK_X);
    if (g_rotation[4] && !g_rotation[5])
        object.rotate(ROTATE_CLOCK_Z);
    if (g_rotation[5] && !g_rotation[4])
        object.rotate(ROTATE_ANTICLOCK_Z);
    if (g_rotation[6])
        object.rotate(ROTATE_RESET);
    if (g_textureMode)
    {
        object.toggleTexureMode();
        g_textureMode = false;
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void    key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // to disable warning message
    (void)scancode;
    (void)mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        g_textureMode = true;
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        g_translate[0] = true;
    if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
        g_translate[0] = false;
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        g_translate[1] = true;
    if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
        g_translate[1] = false;
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        g_translate[2] = true;
    if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
        g_translate[2] = false;
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        g_translate[3] = true;
    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
        g_translate[3] = false;
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        g_translate[4] = true;
    if (key == GLFW_KEY_Z && action == GLFW_RELEASE)
        g_translate[4] = false;
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        g_translate[5] = true;
    if (key == GLFW_KEY_X && action == GLFW_RELEASE)
        g_translate[5] = false;
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
        g_translate[6] = true;
    if (key == GLFW_KEY_T && action == GLFW_RELEASE)
        g_translate[6] = false;
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        g_rotation[0] = true;
    if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        g_rotation[0] = false;
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        g_rotation[1] = true;
    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        g_rotation[1] = false;
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        g_rotation[2] = true;
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        g_rotation[2] = false;
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        g_rotation[3] = true;
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        g_rotation[3] = false;
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        g_rotation[4] = true;
    if (key == GLFW_KEY_E && action == GLFW_RELEASE)
        g_rotation[4] = false;
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        g_rotation[5] = true;
    if (key == GLFW_KEY_Q && action == GLFW_RELEASE)
        g_rotation[5] = false;
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        g_rotation[6] = true;
    if (key == GLFW_KEY_R && action == GLFW_RELEASE)
        g_rotation[6] = false;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void    framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window; // to disable warning message
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}