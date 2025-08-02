#include "engine.h"

void line(int x0, int x1, int y0, int y1, sf::RenderWindow* window, sf::Color color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x = x0; x <= x1; x++) {
        float t = (x - x0) / (float)(x1 - x0);
        int y = static_cast<int>(y0 * (1. - t) + y1 * t);

        sf::Vertex v1;
        if (steep) {
            v1 = { {static_cast<float>(y), static_cast<float>(x)}, color };
        }
        else {
            v1 = { {static_cast<float>(x), static_cast<float>(y)}, color };
        }

        window->draw(&v1, 1, sf::PrimitiveType::Points);
    }
}


void triangle(int x0, int y0, int x1, int y1, int x2, int y2, sf::RenderWindow* window, sf::Color color)
{
    line(x0, x1, y0, y1, window, color);
    line(x1, x2, y1, y2, window, color);
    line(x2, x0, y2, y0, window, color);
}