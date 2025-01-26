#include <bubbles/common.hpp>


void LoadLevel(std::string& levelPath) {
	MarkedModel marked = load_marked("assets/level_testing/test_0.gltf").value();
    PhysicsGameObject meshes[marked.model.meshCount];
	std::cout << marked.model.meshCount << "\n";
    for (int meshIndex = 0; meshIndex < marked.model.meshCount; meshIndex++) {
        if (isTrackMesh(marked.meshTags[meshIndex])) {
            PhysicsCreationInfo info = PhysicsCreationInfo{
                .startLocation = getAveragePosition(marked, meshIndex),
                .isStatic = true
            };
            meshes[meshIndex] = PhysicsGameObject(false, marked.

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

Vector3 getAveragePosition(MarkedModel model, int meshIndex) {
    Vector3 averagePosition = Vector3();
    float *averagePositionA = getAveragePositionA(model, meshIndex);
    averagePosition.x = averagePositionA[0];
    averagePosition.y = averagePositionA[1];
    averagePosition.z = averagePositionA[2];
    return averagePosition;
}

float *getAveragePositionA(MarkedModel model, int meshIndex) {
    float averagePosition[3] = {};
    for (int dimIndex = 0; dimIndex < 3; dimIndex++){
        float sum = 0;
        for (int verticeIndex = 0; verticeIndex < model.model.meshes[meshIndex].vertexCount; verticeIndex += 3) {
            sum += model.model.meshes[meshIndex].vertices[verticeIndex + dimIndex];
        }
        averagePosition[dimIndex] = sum / model.model.meshes[meshIndex].vertexCount;
    }
    return averagePosition;
}