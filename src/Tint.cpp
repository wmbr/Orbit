#include "Tint.hpp"

#include <string>

extern char _binary_shaders_tint_fs_glsl_start;
extern char _binary_shaders_tint_fs_glsl_end;

Tint::Tint(const sf::Color& color)
{
	if(!sf::Shader::isAvailable())
		throw std::runtime_error("No shader support available");
	std::string src(&_binary_shaders_tint_fs_glsl_start, &_binary_shaders_tint_fs_glsl_end);
	if (!this->shader.loadFromMemory(src, sf::Shader::Fragment))
		throw std::runtime_error("Failed to load shader");
	this->shader.setParameter("source", sf::Shader::CurrentTexture);
	this->shader.setParameter("color", color);
}

void Tint::apply(sf::RenderTarget& target, const sf::Texture& source, bool intermediaryCopy)
{
	sf::Sprite sprite(source);
	if(intermediaryCopy)
	{
		if(this->tmp.getSize() != source.getSize())
			if(!this->tmp.create(source.getSize().x, source.getSize().y))
				throw std::runtime_error("Could not create RenderTexture");


		tmp.draw(sprite);
		tmp.display();
		sprite.setTexture(tmp.getTexture());
	}
	target.draw(sprite , &this->shader);
}
