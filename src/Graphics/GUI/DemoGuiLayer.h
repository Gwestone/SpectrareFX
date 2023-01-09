#pragma once
#include "GuiLayer.h"

class DemoGuiLayer : public GuiLayer {
    void init() override;
    void render() override;
public:
    DemoGuiLayer();
};
