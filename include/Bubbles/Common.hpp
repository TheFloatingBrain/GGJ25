#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include <fstream>

#include <spdlog/spdlog.h>

#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>
#include <LinearMath/btTransformUtil.h>
#include <LinearMath/btVector3.h>
#include <LinearMath/btAlignedObjectArray.h>

#include <raylib.h>
#include <raymath.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

