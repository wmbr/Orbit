#ifndef TINT_HPP
#define TINT_HPP

#include <SFML/Graphics.hpp>


class Tint{
	private:
		sf::Shader shader;
		sf::RenderTexture tmp;
	public:
		Tint(const sf::Color& color);
		/**
		 * If intermediaryCopy is false, source and target may not refer to the same texture
		 */
		void apply(sf::RenderTarget& target, const sf::Texture& source, bool intermediaryCopy = true);
};

#endif // TINT_HPP
