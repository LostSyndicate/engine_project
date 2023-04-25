#ifndef SM3_MAINPROGRAM_H
#define SM3_MAINPROGRAM_H

#include "Core/Application.h"

#include "Graphics/Font.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"
#include "Graphics/Model.h"
#include "Graphics/Camera.h"
#include "Graphics/Skybox.h"
#include "Graphics/Framebuffer.h"
#include "UI/UICanvas.h"
#include "UI/UIDropdown.h"
#include "UI/UIButton.h"
#include "UI/UIField.h"
#include "UI/UILabel.h"
#include "UI/UIWindow.h"

class Program : public Application
{
public:
	Program(const char* title, int width, int height) :
		Application(title, width, height) {}

	void OnInit() override;
	void OnDestroy() override;
	void OnFrame(float delta) override;
	void OnEvent(SDL_Event event) override;
	void OnBeginPoll() override;


	void DrawScene(Shader* shader, bool shadow_pass);
public:
	Shader* s_Depth = nullptr;
	Shader* s_Shaded = nullptr;
	Shader* s_Skybox = nullptr;
	Shader* s_GBuffer = nullptr;
	Shader* s_Screen = nullptr;
	Shader* s_WorldLines = nullptr;
	Shader* s_Deferred = nullptr;
	Shader* s_UI = nullptr;

	glm::vec3 light_pos_ = glm::vec3(6.f, 25.f, -3.f);
	Font* font_ = nullptr;
	Input* input_ = nullptr;
	UIWindow* ui_window_ = nullptr;
	UICanvas* canvas_ = nullptr;
	Framebuffer* shadowMap_ = nullptr;
	Model* model_ = nullptr;
	Skybox* skybox_ = nullptr;
	Camera  camera_;
};

#endif // SM3_MAINPROGRAM_H
