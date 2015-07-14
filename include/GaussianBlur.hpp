#ifndef GAUSSIANBLUR_HPP
#define GAUSSIANBLUR_HPP

#include <SFML/Graphics.hpp>

class GaussianBlur{
	private:
		sf::Shader shader;
		sf::RenderTexture tmp;
	public:
		GaussianBlur();
		void apply(sf::RenderTarget& target, const sf::Texture& source);
};

#endif // GAUSSIANBLUR_HPP
