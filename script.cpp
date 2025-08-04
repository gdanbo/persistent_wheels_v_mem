#include "script.h"
#include "steering.hpp"

void main()
{
	float persistentValue = 0.0f;

	while (true)
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();

		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, true)) {
			Vehicle currentVehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);

			if (AI::GET_IS_TASK_ACTIVE(playerPed, 160)) {
				SteeringControl->Patch();
				persistentValue = GET_VEHICLE_STEERING_VALUE(currentVehicle);

				//std::cout << "entering " << currentVehicle << std::endl;

				while (AI::GET_IS_TASK_ACTIVE(playerPed, 160)) {
					Wait(1);
				}
			}
			else {
				bool tryingToSteer = (CONTROLS::IS_CONTROL_PRESSED(0, 63) || CONTROLS::IS_CONTROL_PRESSED(0, 64) || CONTROLS::IS_CONTROL_PRESSED(0, 106));
				if (ENTITY::GET_ENTITY_SPEED(currentVehicle) < 0.8f) {
					if (tryingToSteer) {
						SteeringControl->Restore();
						persistentValue = GET_VEHICLE_STEERING_VALUE(currentVehicle);
					}
					else {
						SteeringControl->Patch();
					}

					if (persistentValue != 0) {
						SET_VEHICLE_STEERING_VALUE(currentVehicle, persistentValue);
					}
				}
				else {
					if (tryingToSteer) {
						SteeringControl->Restore();
					}
					persistentValue = 0.0f;
					Wait(10);
				}
			}
		}
		else {
			Wait(500); // debug
		}

		Wait(0);
	}
}

void ScriptInit()
{	
	srand(GetTickCount64());
	if (scanPatterns()) {
		main();
	}
}

void CollectGarbage() {
	SteeringParked->Restore();
	delete SteeringControl;
	delete SteeringParked;
}
