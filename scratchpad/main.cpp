#include <Bubbles/Physics.hpp>
#include <Bubbles/LoadMarked.hpp>
#include <Bubbles/MeshUtilities.hpp>
#include <Bubbles/GameObject.hpp>

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
	PhysicsCreationInfo info{.mass = 1.f, .isStatic = false};
	spdlog::debug("Before creating object");
	PhysicsGameObject myBall(.1f, info);
	spdlog::debug("After creating object");
			
	//Model model = LoadModel("assets/level_testing/test_0.glb");
	//MarkedModel marked = load_marked("assets/level_testing/test_0.gltf").value();
	//print_model_info(marked.model);
	//Physics physics;
	return 0;
}

