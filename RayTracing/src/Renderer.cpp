#include "Renderer.h"

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);
		
		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
}

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

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	Ray ray;
	ray.Origin = camera.GetPosition();

	for (uint32_t j = 0; j < m_FinalImage->GetHeight(); j++)
	{
		for (uint32_t i = 0; i < m_FinalImage->GetWidth(); i++)
		{
			ray.Direction = camera.GetRayDirections()[i + j * m_FinalImage->GetWidth()];

			glm::vec4 pixelColor = TraceRay(scene, ray);
			pixelColor = glm::clamp(pixelColor, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[i + j * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(pixelColor);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

std::shared_ptr<Walnut::Image> Renderer::GetFinalImage()
{
	return m_FinalImage;
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	//(bx^2 + by^2)t^2 + (2(axbx - bxox + ayby - byoy))t + (ax^2 - 2axox + ox^2 + ay^2 - 2ayoy + oy^2 - r^2) = 0
	//Ray equation = a + bt
	//a = ray origin
	//b = ray direction
	//t = hit distance
	//o = sphere origin
	//r = sphere radius

	if(scene.Spheres.size() == 0)
		return glm::vec4(0, 0, 0, 1);

	const Sphere* closestSphere = nullptr;
	float hitDistance = std::numeric_limits<float>::max();
	for (const Sphere& sphere : scene.Spheres)
	{
		glm::vec3 origin = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

		float discriminant = b * b - 4 * a * c;

		if (discriminant < 0.0f)
			continue;

		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		if (closestT < hitDistance)
		{
			closestSphere = &sphere;
			hitDistance = closestT;
		}
	}

	if(closestSphere == nullptr)
		return glm::vec4(0, 0, 0, 1);

	glm::vec3 origin = ray.Origin - closestSphere->Position;
	glm::vec3 hitPoint = origin + ray.Direction * hitDistance;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 normalizedLight = glm::normalize(m_LightDirection);
	float intensity = glm::max(glm::dot(normal, -normalizedLight), 0.0f);

	return glm::vec4(closestSphere->Albedo * intensity, 1);
}
