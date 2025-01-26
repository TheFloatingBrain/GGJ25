#include <bubbles/Physics.hpp>
#include <bubbles/load_marked.hpp>
#include <bubbles/LevelLoader.hpp>
//#define R3D_ASSIMP_SUPPORT
//#define R3D_IMPLEMENTATION
//#include <r3d.h>


void CustomLog(int msgType, const char *text, va_list args)
{
    char timeStr[64] = { 0 };
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s] ", timeStr);

    switch (msgType)
    {
        case LOG_INFO: printf("[INFO] : "); break;
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
	//Model model = LoadModel("assets/level_testing/test_0.glb");
	MarkedModel marked = load_marked("assets/level_testing/test_0.gltf").value();
	std::cout << marked.model.meshCount << "\n";
	for(auto name : marked.mesh_tags)
		std::cout << name << "\n";
	// //try {
	// //	UnloadModel(marked.model);
	// //}
	// //catch(...) {
	// //}
	Physics physics;
	return 0;
}

