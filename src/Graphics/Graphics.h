#ifndef PARALLEL_GRAPHICS_H
#define PARALLEL_GRAPHICS_H

#include <queue>
#include <iostream>
#include <thread>

class Graphics {
public:
    static void run_thread();
    void init();
    void mainLoop();
    void clear();
};


#endif //PARALLEL_GRAPHICS_H
