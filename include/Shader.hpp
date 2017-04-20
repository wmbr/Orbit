#ifndef SHADER_HPP
#define SHADER_HPP

#include <SFML/Graphics.hpp>

class Shader
{
	public:
		virtual void apply(sf::RenderTarget& target, const sf::Texture& source) = 0;
		virtual ~Shader() {};
};

class NullShader : public Shader
{
	public:
		virtual void apply(sf::RenderTarget& target, const sf::Texture& source);
};

#endif // SHADER_HPP
