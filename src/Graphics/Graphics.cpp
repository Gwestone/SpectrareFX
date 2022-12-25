#include "Graphics.h"

void Graphics::run_thread() {
//    using namespace std::chrono_literals;
//
//    while (true) {
//        std::cout << "Do rendering job..." << std::endl;
//        std::this_thread::sleep_for(1s);
//    }

    Graphics graphics;
    graphics.init();
    graphics.mainLoop();
    graphics.clear();

}

void Graphics::init() {

}

void Graphics::mainLoop() {

}

void Graphics::clear() {

}
