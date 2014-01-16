#include <SFML/Audio.hpp>
#include <iostream>

int main()
{
    sf::SoundBuffer buffer;
    if (!buffer.LoadFromFile("sound.wav")) {
        std::cerr << "fail path" << std::endl;
        return -1;
    }
        

    return 0;
}