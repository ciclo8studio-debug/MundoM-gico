#include "Vehicle/BlacklineWheelRear.h"

UBlacklineWheelRear::UBlacklineWheelRear()
{
	AxleType = EAxleType::Rear;
	WheelRadius = 38.f;
	WheelWidth = 28.f; // algo mas ancha: furgon cargado atras
	WheelMass = 27.f;  // VERIFY-5.8

	bAffectedBySteering = false;
	MaxSteerAngle = 0.f; // VERIFY-5.8: nombre

	bAffectedByEngine = true;
	bAffectedByBrake = true;
	bAffectedByHandbrake = true;
	MaxBrakeTorque = 3000.f; // Nm, reparto 58/42 a favor del eje delantero
	MaxHandBrakeTorque = 6000.f;

	FrictionForceMultiplier = 3.2f;
	CorneringStiffness = 1000.f; // VERIFY-5.8
	SlipThreshold = 400.f;
	SkidThreshold = 400.f;

	SuspensionAxis = FVector(0.f, 0.f, -1.f);
	SuspensionForceOffset = FVector::ZeroVector;
	SuspensionMaxRaise = 12.f;
	SuspensionMaxDrop = 18.f;
	SuspensionDampingRatio = 0.6f;
	SpringRate = 110.f; // mas rigido: carga
	SpringPreload = 60.f;
	SuspensionSmoothing = 8;
	WheelLoadRatio = 0.5f;
	RollbarScaling = 0.30f;
}
