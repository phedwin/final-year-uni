#include <stdint.h>

uint32_t calculate_physical_address(uint16_t segment, uint16_t offset) {
	/* To get the PA -> we shift the segment by 4 and add the offset*/
	uint32_t physical_address = ((uint32_t)segment * 16) + offset;

	return physical_address;
}

#include <assert.h>
void main() {
	/*TODO: im assuming locations like 0x7c00 are either 0000:0x7c00 or
	 * 0x7c00:0000*/
	assert(calculate_physical_address(0x1234, 0x5678) == 0x179b8);
}