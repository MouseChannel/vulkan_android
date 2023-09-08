#include <android_native_app_glue.h>
#include <jni.h>
#include <iostream>
#include <string>
#include <android/log.h>
#include "src/main/cpp/vulkan_loader.cpp"
//#include
//#include "test/testf.cpp"
//#include "Example/main.cpp"
//#include "
//#include "Helper/"
//#include "GL/gl.h"

//#include "Helper/ImGui_Context.hpp"
#include "Rendering/Context.hpp"
#include "Wrapper/Device.hpp"
#include "imgui/imgui.h"
//#include "vulkan/v
extern "C" {
android_app *app;
std::shared_ptr<VulkanLoader> vulkan_loader;
bool isready = false;
class CommandBuffer;
class Window;
class ImGuiContext {
public:
    ImGuiContext();
    ~ImGuiContext();
    void Init(std::shared_ptr<Window> window);
    void Update(std::shared_ptr<CommandBuffer> cmd);

private:
    vk::DescriptorPool descriptor_pool;
    void create_descriptor_pool();
    ImGuiIO io;
};
}
void initVulkan(android_app *pApp);
void terminate();
void handle_cmd(android_app *pApp, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            LOGE("Hello micheng");
            initVulkan(pApp);

        case APP_CMD_TERM_WINDOW:
            terminate();


            break;

    }
}
void initVulkan(android_app *pApp) {
    MoCheng3D::Context::Get_Singleton()->Init_Vulkan(pApp);
    isready = true;
//    vulkan_loader->Init(pApp);
}
void terminate() {
    std::cout << "terminate" << std::endl;
}
void android_main(struct android_app *pApp) {
    vulkan_loader.reset(new VulkanLoader);
    app = pApp;
    pApp->onAppCmd = handle_cmd;

    int events;
    android_poll_source *pSource;
    do {
        if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
            if (pSource) {
                pSource->process(pApp, pSource);
            }
        }

        if (isready) {
            MoCheng3D::Context::Get_Singleton()->BeginFrame();
            MoCheng3D::Context::Get_Singleton()->EndFrame();
            MoCheng3D::Context::Get_Singleton()->Get_Device()->Get_handle().waitIdle();

        }
    } while (!pApp->destroyRequested);
}
}