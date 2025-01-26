void renderWorld() {
	InitWindow(800, 450, "Bubbles");
	MarkedModel marked = LoadMarked("assets/level_testing/test_0.gltf").value();
	spdlog::set_level(spdlog::level::trace);
	spdlog::debug("Creating Physics World");
	Physics physicsWorld;
	spdlog::debug("Creating Objects");
	auto* ball = new PhysicsGameObject(
		::Vector3{ 4.f, 30.f, 4.f },
		PhysicsCreationInfo{
			.startLocation = ::Vector3{0.f, 150.f, 0.f},
			.mass = 1.f, .isStatic = false
		});

	auto* ground = new PhysicsGameObject(
		::Vector3{ 100.f, 10.f, 100.f },
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
}