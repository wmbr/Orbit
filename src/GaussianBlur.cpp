#include "GaussianBlur.hpp"

GaussianBlur::GaussianBlur()
{
	if(!sf::Shader::isAvailable())
		throw std::runtime_error("No shader support available");
	if (!this->shader.loadFromFile("shaders/gaussianBlur_fs.glsl", sf::Shader::Fragment))
		throw std::runtime_error("Shader not found");
	this->shader.setParameter("source", sf::Shader::CurrentTexture);
}

void GaussianBlur::apply(sf::RenderTarget& target, const sf::Texture& source)
{
	if(this->tmp.getSize() != source.getSize())
		if(!this->tmp.create(source.getSize().x, source.getSize().y))
			throw std::runtime_error("Could not create RenderTexture");

	//1st pass: horizontal blur
	sf::Sprite sprite(source);
	this->shader.setParameter("direction", sf::Vector2f(1.0, 0));
	tmp.draw(sprite, &this->shader);
	tmp.display();

	//2nd pass: vertical blur
	sprite.setTexture(tmp.getTexture());
	this->shader.setParameter("direction", sf::Vector2f(0, 1.0));
	target.draw(sprite, &this->shader);
}
