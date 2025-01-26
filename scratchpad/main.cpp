#include <Bubbles/Physics.hpp>
#include <Bubbles/LoadMarked.hpp>
#include <Bubbles/MeshUtilities.hpp>

using namespace Bubbles;


void CustomLog(int msgType, const char *text, va_list args)
{
    char timeStr[64] = { 0 };
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s] ", timeStr);

    switch (msgType)
    {
        case LOG_INFO: ("[INFO] : "); break;
        case LOG_ERROR: printf("[ERROR]: "); break;
        case LOG_WARNING: printf("[WARN] : "); break;
        case LOG_DEBUG: printf("[DEBUG]: "); break;
        default: break;
    }

    vprintf(text, args);
    printf("\n");
}

int main(int argc, char** args)
{
	InitWindow(800, 450, "Bubbles");
	spdlog::set_level(spdlog::level::trace);
	spdlog::debug("Creating Physics World");
	Physics physicsWorld;
	spdlog::debug("Creating Objects");
	auto* ball = new PhysicsGameObject(
			::Vector3{4.f, 30.f, 4.f}, 
			PhysicsCreationInfo{
				.startLocation = ::Vector3{0.f, 150.f, 0.f}, 
				.mass = 1.f, .isStatic = false
			});

	auto* ground = new PhysicsGameObject (
			::Vector3{100.f, 10.f, 100.f}, 
			PhysicsCreationInfo{
				.startLocation = ::Vector3{0.f, 0.f, 0.f}, 
				.mass = 0.f, .isStatic = true
			});
	spdlog::debug("Adding Objects Physics World");
	physicsWorld.addGameObject(*ball);
	physicsWorld.addGameObject(*ground);
	spdlog::debug("Creating Camera");
	Camera3D camera = { 0 };
	camera.position = Vector3{ 10.0f, 10.0f, 10.0f };
	camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
	camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	DisableCursor();
	
	SetTargetFPS(60);
	while (!WindowShouldClose())
	{
        	UpdateCamera(&camera, CAMERA_FREE);
		physicsWorld.step();
        	BeginDrawing();
            		ClearBackground(RAYWHITE);
			BeginMode3D(camera);
				ball->drawColored(BLUE);
				ground->drawColored(GREEN);
			EndMode3D();
		EndDrawing();
	}
	delete ball;
	delete ground;

	CloseWindow();
	//Model model = LoadModel("assets/level_testing/test_0.glb");
	//MarkedModel marked = load_marked("assets/level_testing/test_0.gltf").value();
	//print_model_info(marked.model);
	//Physics physics;
	return 0;
}

