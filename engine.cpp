#include <SFML/Graphics.hpp>
#include <thread>
#include <cstdlib>


void line(int x0, int x1, int y0, int y1, sf::RenderWindow* window, sf::Color color)
{

    bool steep = false;
    if (std::abs(x0 - x1) < s   td::abs(y0 - y1)) {
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
            v1 = { {static_cast<float>(x), static_cast<float>(y)}, color };
        }
        else
        {
            v1 = { {static_cast<float>(y), static_cast<float>(x)}, color };
        }

        // Рисуем точку
        window->draw(&v1, 1, sf::PrimitiveType::Points);
    }


}



int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "SFML works!");

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        line(100, 900, 100, 500, &window, sf::Color::Magenta);
        line(500, 200, 100, 400, &window, sf::Color::Cyan);

        window.display();
    }


}