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

#ifndef __UCGLDCOMMONTYPES_H__
#define __UCGLDCOMMONTYPES_H__

#define TEX_TYPE_SURFACE   1U
#define TEX_TYPE_IOSURFACE 2U
#define TEX_TYPE_VB        3U
#define TEX_TYPE_AGP       4U
#define TEX_TYPE_AGPREF    6U
#define TEX_TYPE_STD       8U
#define TEX_TYPE_OOB       9U

#ifdef __cplusplus
extern "C" {
#endif

struct VendorNewTextureDataStruc
{
	uint32_t type;		// offset 0x00
	uint8_t num_faces;	// offset 0x04
	uint8_t num_mipmaps;// offset 0x05
	uint8_t min_mipmap; // offset 0x06
	uint8_t bytespp;	// offset 0x07
	uint32_t width;		// offset 0x08
	uint16_t height;	// offset 0x0C
	uint16_t depth;		// offset 0x0E
	uint32_t vram_pitch;// offset 0x10
	uint32_t pitch;		// offset 0x14
	uint32_t read_only;	// offset 0x18
	uint32_t f1[3];		// offset 0x1C
	uint32_t size[2];	// offset 0x28
	uint64_t pixels[3]; // offset 0x30
						// ends   0x48
};

struct sIONewTextureReturnData
{
	uint32_t pad;
	uint64_t tx_data;
	uint64_t sys_obj_addr;
} __attribute__((packed));

struct sIODevicePageoffTexture
{
	uint32_t texture_id;
	uint16_t mipmap;
	uint16_t face;
	uint32_t data[4];	// Note: other elements unused
};

struct sIODeviceChannelMemoryData
{
	mach_vm_address_t addr;
};

enum eIOGLContextModeBits {
	CGLCMB_Stereoscopic      = 0x00000010U,
	CGLCMB_Windowed          = 0x00000020U,
	CGLCMB_DepthMode0        = 0x00000040U,
	CGLCMB_StencilMode0      = 0x00000080U,
	CGLCMB_AuxBuffersMask    = 0x00000300U,
	CGLCMB_DoubleBuffer      = 0x00000400U,
	CGLCMB_SBNFS             = 0x00000800U, /* SingleBuffer, not FullScreen */
	CGLCMB_HaveSampleBuffers = 0x00001000U,
	CGLCMB_AuxDepthStencil   = 0x00002000U,
	CGLCMB_BeamSync          = 0x00008000U,
	CGLCMB_BackingStore      = 0x00800000U,
	CGLCMB_DepthMode16       = 0x01000000U,
	CGLCMB_DepthMode32       = 0x02000000U,
	CGLCMB_SampleBuffersMask = 0x0C000000U
};

struct sIOGLContextReadBufferData
{
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint32_t data_type;
	uint32_t pitch;
	mach_vm_address_t addr;
};

struct sIOGLContextSetSurfaceData {
	uint32_t surface_id;
	uint32_t context_mode_bits;
	uint32_t surface_mode;
	uint32_t dr_options_hi;		// high byte of options passed to gldAttachDrawable
	uint32_t dr_options_lo;		// low byte of options passed to gldAttachDrawable
	uint32_t volatile_state;
	uint32_t set_scale;
	uint32_t scale_options;
	uint32_t scale_width;		// lower 16 bits
	uint32_t scale_height;		// lower 16 bits
};

struct sIOGLContextGetConfigStatus {
	uint32_t config[3];
	uint32_t inner_width;
	uint32_t inner_height;
	uint32_t outer_width;
	uint32_t outer_height;
	uint32_t status;		// boolean 0 or 1
	uint32_t surface_mode_bits;
	uint32_t reserved;
};

struct sIOGLContextGetDataBuffer
{
	uint32_t len;
	mach_vm_address_t addr;
} __attribute__((packed));

struct sIOGLGetCommandBuffer {
	uint32_t len[2];
	mach_vm_address_t addr[2];
};

struct NvNotificationRec
{
	uint32_t data[4];
};

struct GLDSysObject {
	uint32_t object_id;		//  0
	uint32_t f0;			//  4
	int32_t volatile stamps[2];
							//  8
	int32_t volatile refcount;
							// 10
	uint8_t volatile in_use;
							// 14
	uint8_t vstate;			// 15 - looks like purgeable flag
	uint8_t type;			// 16
	uint32_t f2;			// 18
	uint16_t pageoff[6];	// 1C - 1 bit indicates the GLD copy is valid
	uint16_t pageon[6];		// 28 - 1 bit indicates the VRAM copy is valid
	uint32_t f3;			// 34
							// 38
};

struct GLDFence
{
	uint32_t u;
	uint32_t v;
};

/*
 * A GLD Texture of types TEX_TYPE_STD or TEX_TYPE_OOB begins
 *   with an array of 72 structures of the following type.
 *   This corresponds to 6 faces X 12 mipmaps per face
 *   [A total of 0x900 bytes of headers].  After that the
 *   pixel data is stored, as pointed to by pixels_in_client
 *   (equivalently offset_in_client).
 *
 * The f0 and f1 are used to calculate the addresses of
 *   multiple layers when the depth is > 1.  There are
 *   different volume texture layouts for GMA900 and GMA950.
 */
struct GLDTextureHeader {
	uint64_t pixels_in_client;	// 0 (64-bit virtual address of mipmap)
	uint32_t offset_in_client;	// 8 (offset from beginning of texture storage)
	uint32_t f0;				// 12
	uint16_t width_bytes;		// 16
	uint16_t height;			// 18 (in scan lines)
	uint32_t fixed;				// 20 (always 0x3CC0000U, used to build SRC_COPY_BLT commands)
	uint32_t f1;				// 24
	uint16_t pitch;				// 28 (in bytes)
	uint16_t depth;				// 30 (in planes)
};

#ifdef __cplusplus
}
#endif

#endif /* __UCGLDCOMMONTYPES_H__ */
