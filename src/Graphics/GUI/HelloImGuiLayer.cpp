#include "HelloImGuiLayer.h"
#include "imgui.h"

void HelloImGuiLayer::init() {

}

void HelloImGuiLayer::render() {
    // render your GUI
    ImGui::Begin("Demo window");
    ImGui::Button("Hello!");
    ImGui::End();
}
