#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		//Settings tab
		ImGui::Begin("Settings");

		//Render time counter
		ImGui::Text("Last Render: %.3fms", m_LastRenderTime);

		//Render on button click
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::End();

		//Remove padding from viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		//Viewport tab
		ImGui::Begin("Viewport");

		//Gets viewport width and height
		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		//If image is not null, renders the image
		auto image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render();
		
		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer m_Renderer;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}