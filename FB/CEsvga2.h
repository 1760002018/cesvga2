/*

    CEsvga2 - ChrisEric1 Super Video Graphics Array 2
    Copyright (C) 2023-2024, Christopher Eric Lentocha

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef __CESVGA2_H__
#define __CESVGA2_H__

#include <IOKit/graphics/IOFramebuffer.h>
#include "SVGADevice.h"
#include "common_fb.h"

class CEsvga2 : public IOFramebuffer
{
	OSDeclareDefaultStructors(CEsvga2);

private:
	SVGADevice svga;				// (now * at 0x10C)
	IODeviceMemory* m_vram;			// offset 0x110
#if 1
	IOMemoryMap* m_vram_kernel_map;	// offset 0x114
	IOVirtualAddress m_vram_kernel_ptr;	// offset 0x118
	IOPhysicalAddress m_fb_offset;	// offset 0x11C
	uint32_t m_aperture_size;		// offset 0x120
#endif
	uint32_t m_num_active_modes;	// offset 0x124
#if 1
	uint32_t m_max_width;			// offset 0x128
	uint32_t m_max_height;			// offset 0x12C
#endif
	IODisplayModeID m_display_mode;	// offset 0x130
	IOIndex m_depth_mode;			// offset 0x134
	thread_call_t m_restore_call;	// offset 0x138
	IODisplayModeID m_modes[NUM_DISPLAY_MODES];	// offset 0x13C (16 entries)
	uint32_t m_custom_switch;		// offset 0x17C
	bool m_custom_mode_switched;	// offset 0x180
#if 1
	uint32_t m_custom_mode_width;	// offset 0x184
	uint32_t m_custom_mode_height;	// offset 0x188
#endif
	struct {
		OSObject* target;
		void* ref;
		IOFBInterruptProc proc;
	} m_intr;						// offset 0x18C
	IOLock* m_iolock;				// offset 0x198
	void* m_cursor_image;			// offset 0x19C
	int32_t m_hotspot_x;			// offset 0x1A0
	int32_t m_hotspot_y;			// offset 0x1A4

	/*
	 * Begin Added
	 */
	bool m_intr_enabled;
	bool m_accel_updates;
	thread_call_t m_refresh_call;
	uint32_t m_refresh_quantum_ms;
	DisplayModeEntry customMode;
	uint32_t m_edid_size;
	uint8_t* m_edid;
	/*
	 * End Added
	 */

	void Cleanup();
	static uint32_t FindDepthMode(IOIndex depth);
	DisplayModeEntry const* GetDisplayMode(IODisplayModeID displayMode);
	static void IOSelectToString(IOSelect io_select, char* output);
	static void ConvertAlphaCursor(uint32_t* cursor, uint32_t width, uint32_t height);
	void CustomSwitchStepWait(uint32_t value);
	void CustomSwitchStepSet(uint32_t value);
	void EmitConnectChangedEvent();
	void RestoreAllModes();
	static void _RestoreAllModes(thread_call_param_t param0, thread_call_param_t param1);

	/*
	 * Begin Added
	 */
	void scheduleRefreshTimer(uint32_t milliSeconds);
	void scheduleRefreshTimer();
	void cancelRefreshTimer();
	void refreshTimerAction();
	static void _RefreshTimerAction(thread_call_param_t param0, thread_call_param_t param1);
	void setupRefreshTimer();
	void deleteRefreshTimer();
	IODisplayModeID TryDetectCurrentDisplayMode(IODisplayModeID defaultMode) const;
	/*
	 * End Added
	 */

public:
	UInt64 getPixelFormatsForDisplayMode(IODisplayModeID displayMode, IOIndex depth);
	IOReturn setCursorState(SInt32 x, SInt32 y, bool visible);
	IOReturn setCursorImage(void* cursorImage);
	IOReturn setInterruptState(void* interruptRef, UInt32 state);
	IOReturn unregisterInterrupt(void* interruptRef);
	IOItemCount getConnectionCount();
	IOReturn getCurrentDisplayMode(IODisplayModeID* displayMode, IOIndex* depth);
	IOReturn getDisplayModes(IODisplayModeID* allDisplayModes);
	IOItemCount getDisplayModeCount();
	const char* getPixelFormats();
	IODeviceMemory* getVRAMRange();
	IODeviceMemory* getApertureRange(IOPixelAperture aperture);
	bool isConsoleDevice();
	bool start(IOService* provider);
	void stop(IOService* provider);
	IOReturn getAttribute(IOSelect attribute, uintptr_t* value);
	IOReturn getAttributeForConnection(IOIndex connectIndex, IOSelect attribute, uintptr_t* value);
	IOReturn setAttribute(IOSelect attribute, uintptr_t value);
	IOReturn setAttributeForConnection(IOIndex connectIndex, IOSelect attribute, uintptr_t value);
	IOReturn registerForInterruptType(IOSelect interruptType, IOFBInterruptProc proc, OSObject* target, void* ref, void** interruptRef);
	IOReturn CustomMode(CustomModeData const* inData, CustomModeData* outData, size_t inSize, size_t* outSize);
	IOReturn getInformationForDisplayMode(IODisplayModeID displayMode, IODisplayModeInformation* info);
	IOReturn getPixelInformation(IODisplayModeID displayMode, IOIndex depth, IOPixelAperture aperture, IOPixelInformation* pixelInfo);
	IOReturn setDisplayMode(IODisplayModeID displayMode, IOIndex depth);

	/*
	 * Begin Added
	 */
	IOReturn getDDCBlock(IOIndex connectIndex, UInt32 blockNumber, IOSelect blockType, IOOptionBits options, UInt8* data, IOByteCount* length);
	bool hasDDCConnect(IOIndex connectIndex);
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060
	bool passiveMatch(OSDictionary* matching, bool changesOK = false);
#endif
	/*
	 * End Added
	 */

	/*
	 * Accelerator Support
	 */
	SVGADevice* getDevice() { return &svga; }
	void lockDevice();
	void unlockDevice();
	bool supportsAccel();
	void useAccelUpdates(bool state);

#if 1
	IOReturn getStartupDisplayMode(IODisplayModeID* displayMode, IOIndex* depth);
	IOReturn getTimingInfoForDisplayMode(IODisplayModeID displayMode, IOTimingInformation* info);
	IOReturn setDetailedTimings(OSArray* array);
	IOReturn setGammaTable(UInt32 channelCount, UInt32 dataCount, UInt32 dataWidth, void* data);
	IOReturn setStartupDisplayMode(IODisplayModeID displayMode, IOIndex depth);
	IOReturn validateDetailedTiming(void* description, IOByteCount descripSize);
#endif
};

#endif /* __CESVGA2_H__ */
