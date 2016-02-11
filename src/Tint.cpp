#include "Tint.hpp"

Tint::Tint(const sf::Color& color)
{
	if(!sf::Shader::isAvailable())
		throw std::runtime_error("No shader support available");
	if (!this->shader.loadFromFile("shaders/tint_fs.glsl", sf::Shader::Fragment))
		throw std::runtime_error("Shader not found");
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
