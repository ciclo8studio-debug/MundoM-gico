#include "Vehicle/BlacklineWheelFront.h"

UBlacklineWheelFront::UBlacklineWheelFront()
{
	// Neumatico todoterreno 265/70R17 aprox: radio 38 cm, ancho 26.5 cm.
	AxleType = EAxleType::Front;
	WheelRadius = 38.f;
	WheelWidth = 26.5f;
	WheelMass = 25.f; // VERIFY-5.8: kg por rueda

	// Direccion
	bAffectedBySteering = true;
	MaxSteerAngle = 32.f; // VERIFY-5.8: nombre (MaxSteerAngle en 5.x)

	// Traccion / frenos
	bAffectedByEngine = true; // 4x4: ambos ejes motrices
	bAffectedByBrake = true;
	bAffectedByHandbrake = false;
	MaxBrakeTorque = 4200.f;     // Nm
	MaxHandBrakeTorque = 0.f;

	// Friccion / agarre
	FrictionForceMultiplier = 3.0f;
	CorneringStiffness = 1000.f; // VERIFY-5.8
	SlipThreshold = 400.f;
	SkidThreshold = 400.f;

	// Suspension (recorrido largo off-road)
	SuspensionAxis = FVector(0.f, 0.f, -1.f);
	SuspensionForceOffset = FVector::ZeroVector;
	SuspensionMaxRaise = 12.f;
	SuspensionMaxDrop = 16.f;
	SuspensionDampingRatio = 0.55f;
	SpringRate = 90.f;
	SpringPreload = 50.f;
	SuspensionSmoothing = 8;
	WheelLoadRatio = 0.5f;
	RollbarScaling = 0.35f;
}
