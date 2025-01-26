#include <bubbles/common.hpp>


void LoadLevel(String levelPath) {
	MarkedModel marked = load_marked("assets/level_testing/test_0.gltf").value();
	std::cout << marked.model.meshCount << "\n";
    for (int meshIndex = 0; i < marked.model.meshCount; i++) {
        if (isTrackMesh(name)) {

        }
    }
}
bool isTrackMesh(const std::string& meshName) {
    // Find the position of the first period
    size_t periodPos = meshName.find('.');

    // Extract the substring before the period (or the whole string if no period)
    std::string prefix = (periodPos != std::string::npos) ? meshName.substr(0, periodPos) : meshName;

    // Check if the prefix is "Track"
    return prefix == "Track";
}
float getAveragePosition(MarkedModel model, int meshIndex) {
    auto vertices = model.model.meshes[i].vertices[0];
    std::cout << "something";
}