#include "DemoGuiLayer.h"
#include "imgui.h"

void DemoGuiLayer::init() {
}

void DemoGuiLayer::render() {
    ImGui::ShowDemoWindow();
}

DemoGuiLayer::DemoGuiLayer() = default;
