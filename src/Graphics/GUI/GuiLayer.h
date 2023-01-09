#pragma once

#include <iostream>

class GuiLayer {
public:
    virtual void init(){};
    virtual void render(){std::cout << "call to base funtion" << std::endl;};
    GuiLayer(){};
};