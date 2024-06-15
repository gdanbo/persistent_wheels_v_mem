#pragma once
#include "pattern_scan.hpp"

class MemPatcher {
private:
	uintptr_t m_address;
	size_t m_size;
	int m_state;
	BYTE* m_patchBytes;
	BYTE* m_originalBytes;

public:
	MemPatcher(uintptr_t address, size_t size, BYTE bytes[]) : m_address(address), m_size(size), m_state(0), m_patchBytes(new BYTE[size]), m_originalBytes(new BYTE[size]) {
		for (int i = 0; i < size; i++) {
			m_patchBytes[i] = bytes[i];
		}
	};

	~MemPatcher() {
		this->Restore();
		delete[] m_patchBytes;
		delete[] m_originalBytes;
	}

	inline void Patch() {
		if (m_state == 0) {
			//std::cout << "---------------\nPatching bytes at address " << std::hex << m_address << std::endl;
			memcpy(m_originalBytes, reinterpret_cast<void*>(m_address), m_size);
			memcpy(reinterpret_cast<void*>(m_address), m_patchBytes, m_size);
			m_state = 1;
			//TODO: VirtualProtect ----- nema potrebe
		}
	}

	inline void Restore() {
		if (m_state == 1) {
			//std::cout << "Restoring bytes at address " << std::hex << m_address << std::endl;
			memcpy(reinterpret_cast<void*>(m_address), m_originalBytes, m_size);
			m_state = 0;
			//TODO: VirtualProtect ----- nema potrebe
		}
	}
};