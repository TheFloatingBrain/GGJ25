#include <Bubbles/Physics.hpp>
#include <Bubbles/Character.hpp>
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
	spdlog::set_level(spdlog::level::debug);
	spdlog::debug("Initializing Window and Raylib Global Settings");

	InitWindow(800, 450, "Bubbles");
	DisableCursor();
	SetTargetFPS(60);

	spdlog::debug("Creating/Loading Tiby");
	Tiby tiby;

	spdlog::debug("Creating/Loading Materials");
	Image groundImage = GenImageCellular(512, 512, 16);
	Material groundMaterial = LoadMaterialDefault();
	Texture groundTexture = LoadTextureFromImage(groundImage);
	SetMaterialTexture(&groundMaterial, MATERIAL_MAP_DIFFUSE, groundTexture);


	spdlog::debug("Creating Camera");
	Camera3D camera = { 0 };
	camera.position = Vector3{ 10.0f, 10.0f, 10.0f };
	camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
	camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	spdlog::debug("Creating Physics World");
	Physics physicsWorld;

	spdlog::debug("Creating Objects");
	PhysicsGameObject ground(
		::Vector3{100.f, 0.f, 100.f}, 
		PhysicsCreationInfo{}, 
		&groundMaterial
	);
	Character character(camera, ::Vector3{0.f, 10.f, 0.f});
	
	spdlog::debug("Adding Objects Physics World");
	physicsWorld.addGameObject(character);
	physicsWorld.addGameObject(ground);


	spdlog::debug("Beggining game Loop");
	
	while (!WindowShouldClose())
	{
		physicsWorld.step();

		Controls controls{
			.forward = IsKeyDown(KEY_W), 
			.backward = IsKeyDown(KEY_S), 
			.left = IsKeyDown(KEY_A), 
			.right = IsKeyDown(KEY_D), 
			.jump = IsKeyDown(KEY_SPACE), 
			.cameraZoomDelta = -GetMouseWheelMove(), 
			.cameraOrbitDelta = GetMouseDelta()
		};
		character.update(controls, physicsWorld);
        	BeginDrawing();
            		ClearBackground(GRAY);
				BeginBlendMode(BLEND_ALPHA);
			BeginMode3D(character.camera);
				//DrawModel(tiby, {0, 0, 0}, 1.f, RAYWHITE);
				ground.draw();//Colored(GREEN);
				character.drawColored({ 135, 60, 190, 128 });
			EndMode3D();
			EndBlendMode();
		EndDrawing();
	}

	spdlog::debug("Cleaning Up");

	CloseWindow();

	spdlog::debug("Returning");

	return 0;
}

