#include "Shader.hpp"

void NullShader::apply(sf::RenderTarget& target, const sf::Texture& source)
{
	sf::Sprite sprite(source);
	target.draw(sprite);
}
