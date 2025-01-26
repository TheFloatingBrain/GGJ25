#include <Bubbles/GameObject.hpp>
#include <functional>
#include <rcamera.h>

namespace Bubbles
{
	struct Controls {
		bool forward, backward, left, right, jump;
		float cameraZoomDelta;
		::Vector2 cameraOrbitDelta;
		float orbitSensitivity = 0.003f;
	};

	struct Tiby
	{
		Model tiby;
		Image bodyImage, hairImage;
		Texture bodyTexture, hairTexture;
		Material bodyMaterial, hairMaterial;
	
		Tiby()
		{
			tiby = LoadModel("assets/characters/tiby/tiby.glb");
			bodyImage = LoadImage("assets/characters/tiby/Tiby_Texture.png");
			hairImage = LoadImage("assets/characters/tiby/Tiby_Texture_Hair.png");
			bodyTexture = LoadTextureFromImage(bodyImage);
			hairTexture = LoadTextureFromImage(hairImage);
			bodyMaterial = LoadMaterialDefault();
			hairMaterial = LoadMaterialDefault();
			SetMaterialTexture(&bodyMaterial, MATERIAL_MAP_DIFFUSE, bodyTexture);
			SetMaterialTexture(&hairMaterial, MATERIAL_MAP_DIFFUSE, hairTexture);
		}
	
		void draw(Vector3 position, float yAngle)
		{
			auto transform = MatrixRotateY(yAngle)
				* MatrixTranslate(position.x, position.y, position.z);
			DrawMesh(tiby.meshes[0], bodyMaterial, transform);
			DrawMesh(tiby.meshes[1], hairMaterial, transform);
		}
	};


	struct Character : public PhysicsGameObject
	{
		::Camera camera;
		float cameraDistance;
		float radius;
		float torqueScalar;
		Tiby tiby;
		Character(
			Camera camera_, 
			::Vector3 position = { 0 }, 
			::Vector3 orientation = { 0 }, 
			float radius_ = 5.f, 
			float mass = 10.f, 
			float cameraDistance_ = 30.f, 
			float torqueScalar_ = 100.f
		) : PhysicsGameObject(radius_, PhysicsCreationInfo {
			.startLocation = position, .orientation = orientation, .mass = mass
		}), camera(camera_), cameraDistance(cameraDistance_),  radius(radius_), torqueScalar(torqueScalar_) {
			body().setSpinningFriction(100.f);
			body().setRollingFriction(100.f);
			auto& shape = physicsData.shape.shapeRef();
			body().setAnisotropicFriction(
					shape.getAnisotropicRollingFrictionDirection(), 
					btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION
				);
		}

		::Vector3 objectUpVector() {
			return Vector3Normalize(GetCameraUp(&camera) - GetCameraForward(&camera));
		}

		::Vector3 objectForwardVector()
		{
			auto right = GetCameraRight(&camera);
			auto up = objectUpVector();
			auto result = Vector3CrossProduct(up, right);
			result.y = 0;
			result = Vector3Normalize(result);
			return result;
		}

		void update(const Controls controls, Physics& physicsWorld) {
			orbitalCameraControls(controls);

			angularVelocityControls(controls);
		}

		void orbitalCameraControls(const Controls controls)
		{
			auto cameraOffset = camera.position - camera.target;
			camera.target = this->getPosition();
			camera.position = camera.target + cameraOffset;
			cameraOffset = camera.position - camera.target;
			auto cameraToTarget = Vector3Normalize(cameraOffset);
			camera.position = camera.target + (
				cameraDistance * cameraToTarget
			);
			CameraYaw(
					&camera, 
					-controls.cameraOrbitDelta.x * controls.orbitSensitivity, 
					true
				);
			CameraPitch(
					&camera, 
					-controls.cameraOrbitDelta.y * controls.orbitSensitivity, 
					true, 
					true,  
					false
				);
			cameraDistance += controls.cameraZoomDelta;
			cameraDistance = (cameraDistance < radius) ? radius : cameraDistance;
		}

		void angularVelocityControls(const Controls controls)
		{
			auto forward = objectForwardVector();
			auto right = GetCameraRight(&camera);
			auto angularVelocity = this->body().getAngularVelocity();
			btVector3 delta(0, 0, 0);
			if(controls.forward == true) {
				delta = r2bv(right * -torqueScalar);
				++pressTime;
			}
			if(controls.backward == true) {
				delta = r2bv(right * torqueScalar);
				++pressTime;
			}
			if(controls.left == true) {
				delta = r2bv(forward * -torqueScalar);
				++pressTime;
			}
			if(controls.right == true) {
				delta = r2bv(forward * torqueScalar);
				++pressTime;
			}
			const bool directionControlPressed = (
				controls.right 
				|| controls.left 
				|| controls.forward
				|| controls.backward
			);
			if(directionControlPressed == false)
				pressTime = 1;
			float time = static_cast<float>(pressTime);
			this->body().setAngularVelocity(delta + (1 / time) * this->body().getAngularVelocity());
		}

		void drawObjectGizmo()
		{
			auto characterOffset = Vector3{0, radius + .1f, 0};
			DrawCylinderWiresEx(
				getPosition() + characterOffset, 
				getPosition() + characterOffset + objectUpVector() * 3.f, 
				radius / 2.f, 
				radius / 5.f, 
				10, 
				::Color { 0, 121, 241, 128 } 
			);
			DrawCylinderEx(
				getPosition() + characterOffset,  
				getPosition() + characterOffset + GetCameraRight(&camera) * 3.f, 
				radius / 2.f, 
				radius / 5.f, 
				10, 
				RED
			);
			DrawCylinderEx(
				getPosition() + characterOffset,  
				getPosition() + characterOffset + objectForwardVector() * 3.f, 
				radius / 2.f, 
				radius / 5.f, 
				10, 
				GREEN
			);
		}

		void exportMesh(const char* fileName) {
			spdlog::info("Exporting mesh to file {}", fileName);
			ExportMesh(mesh, fileName);
		}

		virtual void drawColored(Color color) override 
		{
			tiby.draw(
				getPosition() + ::Vector3{0, -radius + .1f, 0}, 
				directionToYAngle(objectForwardVector())
			);
			PhysicsGameObject::drawColored(color);
			//drawObjectGizmo();
		}

		protected: 
			size_t pressTime = 1;
	};
}

