#include <Bubbles/GameObject.hpp>
#include <functional>
#include <rcamera.h>

namespace Bubbles
{
	struct Controls {
		bool forward, backward, left, right, jump;
		float cameraZoomDelta;
	};


	struct Character : public PhysicsGameObject
	{
		::Camera camera;
		float cameraDistance;
		float radius;
		float torqueScalar;
		Character(
			Camera camera_, 
			::Vector3 position = { 0 }, 
			::Vector3 orientation = { 0 }, 
			float radius_ = 1.f, 
			float mass = 1.f, 
			float cameraDistance_ = 10.f, 
			float torqueScalar_ = 1000.f
		) : PhysicsGameObject(radius_, PhysicsCreationInfo {
			.startLocation = position, .orientation = orientation, .mass = mass
		}), camera(camera_), cameraDistance(cameraDistance_),  radius(radius_), torqueScalar(torqueScalar_) {
			body().setSpinningFriction(.1f);
			body().setRollingFriction(1.f);
		}

		void update(const Controls controls)
		{
			//UpdateCamera(&camera, CAMERA_THIRD_PERSON);
			cameraDistance += controls.cameraZoomDelta;
			cameraDistance = (cameraDistance < radius) ? radius : cameraDistance;
			auto torque = .1 * b2rv(body().getTotalTorque());
			auto towardsCharacterNormal = Vector3Normalize(camera.target - camera.position);
			if(controls.forward == true)
				this->body().applyTorque(r2bv(GetCameraRight(&camera) * (-torqueScalar + torque)));
				//this->body().applyTorque(r2bv(GetCameraRight(&camera) * torqueScalar));
			if(controls.backward == true)
				this->body().applyTorque(r2bv(GetCameraRight(&camera) * (torqueScalar + torque)));
				//this->body().applyTorque(r2bv(GetCameraRight(&camera) * -torqueScalar));
			if(controls.left == true)
				this->body().applyTorque(r2bv(GetCameraForward(&camera) * (-torqueScalar + torque)));
			if(controls.right == true)
				this->body().applyTorque(r2bv(GetCameraForward(&camera) * (torqueScalar + torque)));
		}

		virtual void drawColored(Color color) override 
		{
			PhysicsGameObject::drawColored(color);
			camera.target = this->getPosition();
			camera.position = camera.target + (
				cameraDistance * Vector3Normalize(camera.position - camera.target)
			);
		}
	};
}

