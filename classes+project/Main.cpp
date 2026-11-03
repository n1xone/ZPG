#include "Application.h"
#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
    try {
        Application application;
        application.run();
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }
    return 0;
}
