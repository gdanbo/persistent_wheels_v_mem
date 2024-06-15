#pragma once
#include <cstdint>
#include "sdk/main.h"
#include "sdk/types.h"
#include "patcher.hpp"

static int steering_angle_offset;
uint64_t(*getEntityAddy)(int);
MemPatcher* SteeringControl;
MemPatcher* SteeringParked;

int scanPatterns() {
	uintptr_t gta5 = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));

	uintptr_t steering_base = patterns::scan(gta5, "E8 ? ? ? ? 48 63 87 ? ? ? ? 48 8B 8F");
	getEntityAddy = (uint64_t(*)(int))patterns::scan(gta5, "83 f9 ff 74 31 4c 8b 0d ? ? ? ? 44 8b c1 49 8b 41 08");
	
	uintptr_t steering_control = patterns::scan(gta5, "F3 44 0F 11 87 ? ? ? ? F6 80 ? ? ? ? ? 74 4D"); // SA OVIM JE SIZE 9
	uintptr_t steering_parked = patterns::scan(gta5, "44 89 BB ? ? ? ? 8B 0D ? ? ? ? 0F 57 C0 81 F9 ? ? ? ? 74 ?"); // SA OVIM JE SIZE 7

	if (steering_base != NULL && getEntityAddy != NULL && steering_control != NULL && steering_parked != NULL) {
		steering_angle_offset = *(uint32_t*)(steering_base + 23);

		BYTE bytes[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		SteeringControl = new MemPatcher(steering_control, 9, bytes);
		SteeringParked = new MemPatcher(steering_parked, 7, bytes); // mozemo isti niz bajtova jer on uzima samo onoliko koliko je naveden m_size, a ne ceo prosledjeni niz
		
		SteeringParked->Patch(); // ne moramo non stop nego samo sad, i posle kad unloadujemo Restore() i tjt

		return 1;
	}

	return 0;
}

inline float GET_VEHICLE_STEERING_VALUE(Vehicle vehicle) {
	uint64_t vehiclePtr = getEntityAddy(vehicle);
	//std::cout << "STEERING ADDRESS: " << std::hex << (vehiclePtr + steering_angle_offset) << std::endl;
	return *(float*)((char*)vehiclePtr + steering_angle_offset);
}

inline void SET_VEHICLE_STEERING_VALUE(Vehicle vehicle, float value) {
	uint64_t vehiclePtr = getEntityAddy(vehicle);
	*(float*)((char*)vehiclePtr + steering_angle_offset) = value;
}