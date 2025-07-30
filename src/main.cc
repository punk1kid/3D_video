#include <SFML/Graphics.hpp>

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My window");

    // run the program as long as the window is open
    while (window.isOpen())
    {
        sf::CircleShape shape(50.f);
        sf::Vector2f initpos{ 25.f,25.f };
        shape.setPosition(initpos);
        // set the shape color to green
        shape.setFillColor(sf::Color(100, 250, 50));

        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            window.clear();
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
            shape.setPosition((sf::Vector2f)sf::Mouse::getPosition(window));
            window.draw(shape);

        }

        // clear the window with black color

        // end the current frame
        window.display();
    }
}
