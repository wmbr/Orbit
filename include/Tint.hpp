#ifndef TINT_HPP
#define TINT_HPP

#include <SFML/Graphics.hpp>

#include "Shader.hpp"

class Tint : public Shader{
	private:
		sf::Shader shader;
		sf::RenderTexture tmp;
	public:
		Tint(const sf::Color& color);
		void apply(sf::RenderTarget& target, const sf::Texture& source);
};

#endif // TINT_HPP
