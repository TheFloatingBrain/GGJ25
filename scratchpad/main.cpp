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
	MarkedModel marked = LoadMarked("assets/level_testing/test_0.gltf").value();
	spdlog::set_level(spdlog::level::trace);
	spdlog::debug("Creating Physics World");
	Physics physicsWorld;
	spdlog::debug("Creating Objects");
	auto* meshobj = new PhysicsGameObject(
			LoadModel("assets/test/cube.glb"), 
			PhysicsCreationInfo{
				.startLocation = ::Vector3{0.f, 200.f, 0.f}, 
				//.orientation = ::Vector3{0.f, 3.14f / 2.f, 0.f}, 
				.mass = 20.f, .isStatic = false
			});
	auto* sphere0 = new PhysicsGameObject(
			10.f, 
			PhysicsCreationInfo{
				.startLocation = ::Vector3{40, 150.f, 40.f}, 
				.mass = 2000.f, .isStatic = false
			});
	auto* thirdBar = new PhysicsGameObject(
			::Vector3{4.f, 4.f, 4.f}, 
			PhysicsCreationInfo{
				.startLocation = ::Vector3{-20, 10.f, 0.f}, 
				.mass = 2000.f, .isStatic = false
			});
	auto* secondBar = new PhysicsGameObject(
			::Vector3{30.f, 4.f, 4.f}, 
			PhysicsCreationInfo{
				.startLocation = ::Vector3{-20, 150.f, 0.f}, 
				.mass = 2000.f, .isStatic = false
			});

	auto* ball = new PhysicsGameObject(
			::Vector3{20.f, 2.f, 20.f}, 
			PhysicsCreationInfo{
				.startLocation = ::Vector3{0.f, 25.f, 0.f}, 
				.mass = 2000.f, .isStatic = false
			});

	auto* ballyboi = new PhysicsGameObject (
			10,
			PhysicsCreationInfo{
				.startLocation = ::Vector3{24.f, 50.f, 0.f}, 
				.mass = 1000.f, .isStatic = false
			});

	auto* ground = new PhysicsGameObject (
			::Vector3{100.f, 1.f, 100.f}, 
			PhysicsCreationInfo{
				.startLocation = ::Vector3{0.f, 0.f, 0.f}, 
				.mass = 0.f, .isStatic = true
			});

	spdlog::debug("Adding Objects Physics World");
	physicsWorld.addGameObject(*meshobj);
	physicsWorld.addGameObject(*sphere0);
	physicsWorld.addGameObject(*thirdBar);
	physicsWorld.addGameObject(*secondBar);
	physicsWorld.addGameObject(*ball);
	physicsWorld.addGameObject(*ground);
	physicsWorld.addGameObject(*ballyboi);
	spdlog::debug("Creating Camera");
	Camera3D camera = { 0 };
	camera.position = Vector3{ 0.0f, 25.0f, 50.0f };
	camera.target = Vector3{ 0.0f, 10.0f, 0.0f };
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
            		ClearBackground(GRAY);
			BeginMode3D(camera);
				meshobj->drawColored(PURPLE);
				sphere0->drawColored(YELLOW);
				thirdBar->drawColored(SKYBLUE);
				secondBar->drawColored(RED);
				ball->drawColored(BLUE);
				ground->drawColored(GREEN);
                ballyboi->drawColored(RED);
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

