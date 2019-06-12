#include <stdlib.h>
#include <time.h>

#include <CGFinalHeader/game/game.h>
#include <CGFinalHeader/resourceManager/resource_manager.h>
#include <CGFinalHeader/customObject/border.h>
#include <CGFinalHeader/customObject/plane.h>
#include <CGFinalHeader/modelObject/ModelObject.h>
#include <CGFinalHeader/camera/camera.h>
#include <CGFinalHeader/skybox/skybox.h>


// Custom Object
Plane *plane;
Border *border;

// Model Object
//ModelObject *nanosuit;
ModelObject *fiatCar;

Plane *plane_shadow;
Border *border_shadow;
ModelObject *fiatCar_shadow;
// Skybox
Skybox* skybox;

// lightPos
glm::vec3 lightPos(0.0f, 10.0f, 0.0f);

// position&shift of car
glm::vec3 carShift(0.0f, 0.0f, 0.0f);

const bool renderSkybox = false;

Game::Game(GLuint width, GLuint height, Camera *camera)
	: State(GameState::GAME_ACTIVE), isBlinn(false), Width(width), Height(height), camera(camera)
{

}

Game::~Game()
{
	delete plane;
	delete border;
	//delete nanosuit;
	delete fiatCar;
	delete skybox;
}


void Game::Init()
{
	// set rand seed 
	srand((unsigned int)time(NULL));
	
	// Load shaders
	ResourceManager::LoadShader("../Resources/shaders/shader.vs", "../Resources/shaders/shader.fs", nullptr, "BasicShader");
	ResourceManager::LoadShader("../Resources/shaders/model_loading.vs", "../Resources/shaders/model_loading.fs", nullptr, "BasicModelShader");
	ResourceManager::LoadShader("../Resources/shaders/skyShader.vs", "../Resources/shaders/skyShader.fs", nullptr, "skyShader");
	ResourceManager::LoadShader("../Resources/shaders/simpleDepthShader.vs", "../Resources/shaders/simpleDepthShader.fs", nullptr, "simpleDepthShader");
	ResourceManager::LoadShader("../Resources/shaders/shadow_mapping.vs", "../Resources/shaders/shadow_mapping.fs", nullptr, "shadow_mapping");
	ResourceManager::LoadShader("../Resources/shaders/textShader.vs", "../Resources/shaders/textShader.fs", nullptr, "textShader");
	ResourceManager::LoadShader("../Resources/shaders/debug_quad.vs", "../Resources/shaders/debug_quad.fs", nullptr, "debugDepthQuad");

	// Load textures
	ResourceManager::LoadTexture("../Resources/textures/block.png", GL_TRUE, "block");
	ResourceManager::LoadTexture("../Resources/textures/block_solid.png", GL_TRUE, "block_solid");
	ResourceManager::LoadTexture("../Resources/textures/container2.png", GL_TRUE, "container2");
	ResourceManager::LoadTexture("../Resources/textures/container2_specular.png", GL_TRUE, "container2_specular");
	ResourceManager::LoadTexture("../Resources/textures/brickwall.jpg", GL_TRUE, "brickwall");
	ResourceManager::LoadTexture("../Resources/textures/wood.png", GL_TRUE, "wood");
	ResourceManager::LoadTexture("../Resources/textures/awesomeface.png", GL_TRUE, "awesomeface");
	ResourceManager::LoadTexture("../Resources/textures/metal.png", GL_TRUE, "metal");
	ResourceManager::LoadTexture("../Resources/textures/window.png", GL_TRUE, "window");
	ResourceManager::LoadTexture("../Resources/textures/snake_skin.jpg", GL_TRUE, "snake_skin");
	ResourceManager::LoadTexture("../Resources/textures/animal_skin_0.jpg", GL_TRUE, "animal_skin_0");
	/* skybox */
	vector<std::string> faces{
		"../Resources/textures/skybox/right.jpg",
		"../Resources/textures/skybox/left.jpg",
		"../Resources/textures/skybox/top.jpg",
		"../Resources/textures/skybox/bottom.jpg",
		"../Resources/textures/skybox/front.jpg",
		"../Resources/textures/skybox/back.jpg"
	};
	unsigned int cubemapTexture = ResourceManager::LoadCubemap(faces);

	// Load models
	//ResourceManager::LoadModel("../Resources/objects/nanosuit/nanosuit.obj", "nanosuit");
	ResourceManager::LoadModel("../Resources/objects/fiat/Fiat_127_A_1971.obj", "fiatCar");

	// New Scene Object
	// plane
	//plane = new Plane(ResourceManager::GetShader("BasicShader"), ResourceManager::GetTexture("container2_specular"));
	plane = new Plane(ResourceManager::GetShader("shadow_mapping"), ResourceManager::GetTexture("container2_specular"));
	// border
	border = new Border(ResourceManager::GetShader("BasicShader"), ResourceManager::GetTexture("wood"));

	// skybox
	skybox = new Skybox(ResourceManager::GetShader("skyShader"), cubemapTexture);

	// nanosuit
	//nanosuit = new ModelObject(ResourceManager::GetShader("BasicModelShader"), ResourceManager::GetModel("nanosuit"));
	// fiatCar
	fiatCar = new ModelObject(ResourceManager::GetShader("BasicModelShader"), ResourceManager::GetModel("fiatCar"));
	
	plane_shadow = new Plane(ResourceManager::GetShader("simpleDepthShader"), ResourceManager::GetTexture("container2_specular"));
	border_shadow = new Border(ResourceManager::GetShader("simpleDepthShader"), ResourceManager::GetTexture("wood"));
	fiatCar_shadow = new ModelObject(ResourceManager::GetShader("simpleDepthShader"), ResourceManager::GetModel("fiatCar"));
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	ResourceManager::GetShader("shadow_mapping").Use().SetInteger("diffuseTexture", 0);
	ResourceManager::GetShader("shadow_mapping").Use().SetInteger("shadowMap", 1);
	
	mytext.init();
}

void Game::Update()
{
	carPos += carShift;
	carShift = glm::vec3(0.0f, 0.0f, 0.0f);
}


void Game::ProcessInput(GLFWwindow *window, Camera_Movement direction, glm::vec3 frontOfCar, glm::vec3 upOfCar, float deltaTime)
{
	float velocity = SPEED * deltaTime;
	glm::vec3 front;
	front.x = -cos(glm::radians(Yaw));
	front.y = 0;
	front.z = sin(glm::radians(Yaw));
	carfront = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	glm::vec3 rightOfCar = glm::normalize(glm::cross(carfront, upOfCar));
	if (direction == FORWARD) {
		carShift += carfront * velocity;
	}
	if (direction == LEFT_FORWARD) {
		Yaw += 1;
	}
	if (direction == RIGHT_FORWARD) {
		Yaw -= 1;
		float v = velocity * 2;
		carShift += carfront * v;
	}
	if (direction == LEFT_BACKWARD) {
		Yaw -= 1;
	}
	if (direction == RIGHT_BACKWARD) {
		Yaw += 1;
		float v = velocity * 2;
		carShift += -carfront * v;
	}
	if (direction == BACKWARD) {
		carShift += -carfront * velocity;
	}
}

void Game::Render()
{
	
	//光源空间的变换
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 7.5f;
	lightProjection = glm::ortho(0.0f, 10.0f, 0.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	ResourceManager::GetShader("simpleDepthShader").Use().SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
	//glViewport(0, 0, Width, Height);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	plane_shadow->Draw();
	border_shadow->Draw();
	fiatCar_shadow->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 使用深度贴图渲染场景
	//glViewport(0, 0, Width, Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::perspective(glm::radians((*camera).Zoom), (float)Width / (float)Height, 0.1f, 100.0f);
	glm::mat4 view = (*camera).GetViewMatrix();
	ResourceManager::GetShader("shadow_mapping").Use().SetMatrix4("projection", projection);
	ResourceManager::GetShader("shadow_mapping").Use().SetMatrix4("view", view);
	// set light uniforms
	ResourceManager::GetShader("shadow_mapping").Use().SetVector3f("viewPos", (*camera).Position);
	ResourceManager::GetShader("shadow_mapping").Use().SetVector3f("lightPos", lightPos);
	ResourceManager::GetShader("shadow_mapping").Use().SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	plane->Draw();

	ResourceManager::GetShader("debugDepthQuad").Use().SetInteger("near_plane", far_plane);
	ResourceManager::GetShader("debugDepthQuad").Use().SetInteger("near_plane", far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	// Configure view and projection
	view = this->camera->GetViewMatrix();
	//glm::mat4 projection = glm::perspective(this->camera->Zoom, (GLfloat)this->Width / this->Height, 0.1f, 100.0f);
	projection = glm::perspective(glm::radians(this->camera->Zoom), (GLfloat)this->Width / this->Height, 0.1f, 100.0f);

	// Configure shaders
	// BasicShader
	ResourceManager::GetShader("BasicShader").Use().SetMatrix4("view", view);
	ResourceManager::GetShader("BasicShader").Use().SetMatrix4("projection", projection);

	ResourceManager::GetShader("BasicShader").Use().SetInteger("texture", 0);
	ResourceManager::GetShader("BasicShader").Use().SetInteger("blinn", this->isBlinn);
	ResourceManager::GetShader("BasicShader").Use().SetVector3f("viewPos", this->camera->Position);
	ResourceManager::GetShader("BasicShader").Use().SetVector3f("lightPos", lightPos);
	// Custom object that use BasicShader
	
	border->Draw();

	// BasicModelShader
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	ResourceManager::GetShader("BasicModelShader").Use().SetMatrix4("model", model);
	ResourceManager::GetShader("BasicModelShader").Use().SetMatrix4("view", this->camera->GetViewMatrix());
	ResourceManager::GetShader("BasicModelShader").Use().SetMatrix4("projection", projection);

	ResourceManager::GetShader("BasicModelShader").Use().SetVector3f("viewPos", this->camera->Position);
	ResourceManager::GetShader("BasicModelShader").Use().SetVector3f("pointlight.position", lightPos);
	ResourceManager::GetShader("BasicModelShader").Use().SetVector3f("pointlight.ambient", 0.2f, 0.2f, 0.2f);
	ResourceManager::GetShader("BasicModelShader").Use().SetVector3f("pointlight.diffuse", 0.8f, 0.8f, 0.8f);
	ResourceManager::GetShader("BasicModelShader").Use().SetVector3f("pointlight.specular", 1.0f, 1.0f, 1.0f);
	ResourceManager::GetShader("BasicModelShader").Use().SetFloat("pointlight.constant", 1.0f);
	ResourceManager::GetShader("BasicModelShader").Use().SetFloat("pointlight.linear", 0.022f);
	ResourceManager::GetShader("BasicModelShader").Use().SetFloat("pointlight.quadratic", 0.0019f);
	// Model object that use BasicShader
	//nanosuit->Draw();
	model = glm::mat4(1.0f);
	model = glm::translate(model, carPos);
	//model = glm::translate(model, glm::vec3(1.5f, 0.0f, 3.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	ResourceManager::GetShader("BasicModelShader").Use().SetMatrix4("model", model);

	fiatCar->Draw();

	// Skybox Shader
	ResourceManager::GetShader("skyShader").Use().SetMatrix4("view",glm::mat4(glm::mat3(this->camera->GetViewMatrix())));
	ResourceManager::GetShader("skyShader").Use().SetMatrix4("projection", projection);
	if (!renderSkybox) {
		skybox->Draw();
	}
	// Compile and setup the shader
	projection = glm::ortho(0.0f, static_cast<GLfloat>(Width), 0.0f, static_cast<GLfloat>(Height));
	ResourceManager::GetShader("textShader").Use().SetMatrix4("projection", projection);
	mytext.RenderText(ResourceManager::GetShader("textShader"), str, 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

}

glm::vec3 Game::getFrontOfCar() {
	return carfront;
}
