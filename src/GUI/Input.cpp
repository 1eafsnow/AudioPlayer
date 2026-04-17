#include "Input.h"
#include "GUI.h"

void Input::Press(uint32_t input)
{
    state |= input;
}
void Input::Release(uint32_t input)
{
    state &= ~input;
}
bool Input::Check(uint32_t input)
{
    return (state & input);
}

void Input::SetGUI(GUI* gui)
{
    this->gui = gui;
}

void Input::Tick(float deltaTime)
{
    if (glfwGetKey(gui->glWindow, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(gui->glWindow, 1);
    }
    if (glfwGetKey(gui->glWindow, GLFW_KEY_W))
    {
        gui->renderer->camera->Move(Vector3F(0.0, 0.0, -1.0));
		Vector3F cameraLocation = gui->renderer->camera->GetWorldLoation();
		gui->audioPlayer->SetListenerPosition(cameraLocation);
    }
    if (glfwGetKey(gui->glWindow, GLFW_KEY_S))
    {
        gui->renderer->camera->Move(Vector3F(0.0, 0.0, 1.0));
        Vector3F cameraLocation = gui->renderer->camera->GetWorldLoation();
        gui->audioPlayer->SetListenerPosition(cameraLocation);
    }
    if (glfwGetKey(gui->glWindow, GLFW_KEY_A))
    {
        gui->renderer->camera->Move(Vector3F(-1.0, 0.0, 0.0));
        Vector3F cameraLocation = gui->renderer->camera->GetWorldLoation();
        gui->audioPlayer->SetListenerPosition(cameraLocation);
    }
    if (glfwGetKey(gui->glWindow, GLFW_KEY_D))
    {
        gui->renderer->camera->Move(Vector3F(1.0, 0.0, 0.0));
        Vector3F cameraLocation = gui->renderer->camera->GetWorldLoation();
        gui->audioPlayer->SetListenerPosition(cameraLocation);
    }    

    if (glfwGetMouseButton(gui->glWindow, GLFW_MOUSE_BUTTON_RIGHT))
    {
        double x;
        double y;
        glfwGetCursorPos(gui->glWindow, &x, &y);
        if (Check(MOUSE_RIGHT))
        {
            gui->renderer->camera->AddLocalRotation(Rotator(mouseX - x, y - mouseY, 0.0));
            Rotator rotation = gui->renderer->camera->GetWorldRotation();
            gui->audioPlayer->SetListenerRotation(rotation);
        }
        mouseX = x;
        mouseY = y;
        Press(MOUSE_RIGHT);
    }
    else
    {
        Release(MOUSE_RIGHT);
    }
}