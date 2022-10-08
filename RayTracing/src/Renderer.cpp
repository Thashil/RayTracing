#include "Renderer.h"

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		//No resize needed
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render()
{
	float aspectRatio = (float)m_FinalImage->GetWidth() / m_FinalImage->GetHeight();
	for (uint32_t j = 0; j < m_FinalImage->GetHeight(); j++)
	{
		float y = (float)j / m_FinalImage->GetHeight();

		for (uint32_t i = 0; i < m_FinalImage->GetWidth(); i++)
		{
			glm::vec2 coord = { (float)i / m_FinalImage->GetWidth(), y };
			coord = coord * 2.0f - 1.0f; // -1 < coord < 1
			coord.x = coord.x * aspectRatio;
			m_ImageData[i + j * m_FinalImage->GetWidth()] = PerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

std::shared_ptr<Walnut::Image> Renderer::GetFinalImage()
{
	return m_FinalImage;
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t) (coord.x * 255.0f);
	uint8_t g = (uint8_t) (coord.y * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	rayDirection = glm::normalize(rayDirection);
	float radius = 0.5f;

	//(bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	//a = ray origin
	//b = ray direction
	//r = radius
	//t = hit distance

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin,rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	float discriminant = b * b - 4 * a * c;

	if (discriminant >= 0.0f)
	{
		return 0xffff0000;
	}

	return 0xff000000;

	//return 0xff000000 | (g << 8) | r;
}
