//TA KLASA ODPOWIADA ZA OKNO SYSTEMOWE W KTORYM WYSWIETLA SIE OBRAZ

#include "Window.h"

Window::Window() {
	width = 800;
	height = 600;
    for (int i = 0; i < 1024; ++i) {
        keys[i] = 0;
    }
}

Window::Window(GLint windowWidth, GLint windowHeight) {
	width = windowWidth;
	height = windowHeight;
    for (int i = 0; i < 1024; ++i) {
        keys[i] = 0;
    }
}

int Window::Initialize(char input2) {
    //INITITALIZATION GLFW 
    if (!glfwInit()) {
        std::cout << "Initialization failed!" << std::endl;
        glfwTerminate();
        return 1;
    }

    //GLFW PROPERTIES
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //CREATE WINDOW
    mainWindow = glfwCreateWindow(width, height, "Kamera i Swiatlo, projekt na przedmiot Grafika Komputerowa", NULL, NULL);
    if (!mainWindow) {
        std::cout << "Window creation failed!" << std::endl;
        glfwTerminate();
        return 1;
    }

    //BUFFER SIZE INFORMATION
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);        //obraz najpierw idzie do bufora, nie od razu na ekran

    //SET CONTEXT FOR GLEW TO USE
    glfwMakeContextCurrent(mainWindow);

    //HANDLE KEYS
    createCallbacks();      //obsluga klawiszy w opengl odbywa sie przez tzw. callbacki czyli zwrotne wywolania funkcji po danym zdarzeniu

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW initialization failed" << std::endl;
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    if (input2 == 'y')
        glEnable(GL_DEPTH_TEST);        //automatyczny algorytm z biblioteki opengl do eliminacji obiektow zaslonietych - NIE PODLEGA OCENIE 

    //SETUP VIEWPORT SIZE - create viewport
    glViewport(0, 0, bufferWidth, bufferHeight);
    glfwSetWindowUserPointer(mainWindow, this);

    return 0;
}

void Window::createCallbacks() {
    glfwSetKeyCallback(mainWindow, handleKeys);         //funkcja zwrotna na obsluge klawiszy jest handleKeys()
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode) {
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));     //samo 'this' nie wystarczy do obslugi klawiatury
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            theWindow->keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            theWindow->keys[key] = false;
        }
    }
}

Window::~Window() {
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
}