#include "engine.h"

int main()
{
  cv::Mat image(1080, 1920, CV_8UC3, cv::Scalar(0, 0, 0));
  sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "SFML Triangle");

  while (window.isOpen())
  {
    while (const std::optional event = window.pollEvent())
    {
      if (event->is<sf::Event::Closed>())
        window.close();
    }

    window.clear();


    triangle(300, 200, 600, 300, 400, 600, &window, sf::Color::Green);

    window.display();
  }
  return 0;
}
