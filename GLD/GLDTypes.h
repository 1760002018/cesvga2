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

#include "UCGLDCommonTypes.h"

#ifndef __GLDTYPES_H__
#define __GLDTYPES_H__

#ifdef __LP64_
#define MKOFS(t, ofs32, ofs64, p) ((t*)(((uint8_t*)(p))+ofs64))
#else
#define MKOFS(t, ofs32, ofs64, p) ((t*)(((uint8_t*)(p))+ofs32))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef int GLDReturn;

typedef GLDReturn (*GLD_GENERIC_FUNC)(void*, void*, void*, void*, void*, void*);
typedef void (*GLD_GENERIC_DISPATCH)();

typedef int (*PIODataFlush)();
typedef int (*PIODataBindSurface)(uint32_t arg0,
								  uint32_t arg1,
								  uint32_t const* surface_id,
								  uint32_t context_mode_bits,
								  io_service_t svc);

typedef struct _display_info_t {
	io_service_t service;
	uint32_t mask;
	uint32_t config[5];
	char engine1[64];
	char engine2[64];
} display_info_t;

typedef struct _glr_io_data_t {
	uint32_t displayMask;
	uint32_t lastDisplay;
	io_service_t const* pServices;
	uint8_t const* pServiceFlags;
	PIODataFlush IODataFlush;
	PIODataBindSurface IODataBindSurface;
	io_connect_t surfaces[32];
	uint8_t surface_refcount[32];
	uint32_t num_displays;
	display_info_t* dinfo;
} glr_io_data_t;

typedef struct _RendererInfo
{
	void* pad;
	uint32_t	rendererID;		// offset (4, 8) (Note: upper byte is display number (starting 1))

	// DWORD of bit flags at offset (8, 0xC)
	unsigned bWindow:1;			// can run in windowed mode
	unsigned bFullScreen:1;		// can run fullscreen
	unsigned bOffScreen:1;		// can render to offscreen client memory
	unsigned bBackingStore:1;	// back buffer contents valid after swap
	unsigned bUndefined1:1;		// bit 4
	unsigned bUndefined2:1;		// bit 5
	unsigned bRobust:1;			// does not need failure recovery
	unsigned bUnknown1:1;		// bit 7
	unsigned bAccelerated:1;	// hardware accelerated
	unsigned bMultiScreen:1;	// single window can span multiple screens
	unsigned bCompliant:1;		// OpenGL compliant renderer
	unsigned bAuxDepthStencil:1;	// each aux buffer has its own depth stencil
	unsigned bQuartzExtreme:1;	// supports QuartzExtreme
	unsigned bPBuffer:1;		// can render to PBuffer
	unsigned pVertexProc:1;		// Vertex Proc capable
	unsigned pFragProc:1;		// Fragment Proc capable
	unsigned pAcceleratedCompute:1;	// supports compute acceleration

	uint32_t bufferModes;		// offset (0x0C, 0x10), see Buffer modes in CGLTypes.h
	uint32_t colorModes;		// offset( 0x10, 0x14), see Color and accumulation buffer formats in CGLTypes.h
	uint32_t accumModes;		// offset (0x14, 0x18), see Color and accumulation buffer formats in CGLTypes.h
	uint32_t depthModes;		// offset (0x18, 0x1C), see Depth and stencil buffer depths in CGLTypes.h
	uint32_t stencilModes;		// offset (0x1C, 0x20), see Depth and stencil buffer depths in CGLTypes.h
	uint32_t displayMask;		// offset (0x20, 0x24)
	uint16_t maxAuxBuffers;		// offset (0x24, 0x28), maximum number of auxilliary buffers
	uint16_t maxSampleBuffers;	// offset (0x26, 0x2A), maximum number of sample buffers
	uint16_t maxSamples;		// offset (0x28, 0x2C), maximum number of samples
	uint8_t  sampleAlpha;		// offset (0x2A, 0x2E), support for alpha sampling
	uint32_t sampleModes;		// offset (0x2C, 0x30), see Sampling modes in CGLTypes.h
	uint32_t unknown0[6];		// offset (0x30, 0x34)
	uint32_t vramSize;			// offset (0x48, 0x4C) (in megabytes)
	uint32_t unknown1;			// offset (0x4C, 0x50)
	uint32_t textureMemory;		// offset (0x50, 0x54) (in megabytes)
	uint32_t unknown2[12];		// offset (0x54, 0x58)
								// ends   (0x84, 0x88)
} RendererInfo;

typedef struct _PixelFormat
{
	void* pad;
	uint32_t rendererID;		// offset (4, 8) (Note: upper byte is something else, probably a display mask)

	// DWORD of bit flags at offset (8, 0xC)
	unsigned bWindow:1;			// can run in windowed mode
	unsigned bFullScreen:1;		// can run fullscreen
	unsigned bOffScreen:1;		// can render to offscreen client memory
	unsigned bBackingStore:1;	// back buffer contents valid after swap
	unsigned bUndefined1:1;		// bit 4
	unsigned bUndefined2:1;		// bit 5
	unsigned bRobust:1;			// does not need failure recovery
	unsigned bUnknown1:1;		// bit 7
	unsigned bAccelerated:1;	// hardware accelerated
	unsigned bMultiScreen:1;	// single window can span multiple screens
	unsigned bCompliant:1;		// OpenGL compliant renderer
	unsigned bAuxDepthStencil:1;	// each aux buffer has its own depth stencil
	unsigned bQuartzExtreme:1;	// supports QuartzExtreme
	unsigned bPBuffer:1;		// can render to PBuffer
	unsigned pVertexProc:1;		// Vertex Proc capable
	unsigned pFragProc:1;		// Fragment Proc capable
	unsigned pAcceleratedCompute:1;	// supports compute acceleration

	uint32_t bufferModes;		// offset 0x10, see Buffer modes in CGLTypes.h
	uint32_t colorModes;		// offset 0x14, see Color and accumulation buffer formats in CGLTypes.h
	uint32_t accumModes;		// offset 0x18, see Color and accumulation buffer formats in CGLTypes.h
	uint32_t depthModes;		// offset 0x1C, see Depth and stencil buffer depths in CGLTypes.h
	uint32_t stencilModes;		// offset 0x20, see Depth and stencil buffer depths in CGLTypes.h
	uint16_t unknown1;			// offset 0x24
	int16_t auxBuffers;			// offset 0x26, number of aux buffers
	int16_t sampleBuffers;		// offset 0x28, number of multi sample buffers
	int16_t samples;			// offset 0x2A, number of samples per multi sample buffer
	uint32_t sampleModes;		// offset 0x2C, see Sampling modes in CGLTypes.h
	uint8_t sampleAlpha;		// offset 0x30, request alpha filtering
	uint32_t displayMask;		// offset 0x34
	// end point 0x38
} PixelFormat;

typedef struct _gld_context_t {
	uint32_t display_num;			// (  0,   0)
	io_connect_t context_obj;		// (  4,   4)
	uint8_t	flags1[4];				// (  8,   8)
	struct _gld_shared_t* shared;	// (  C,  10)
	void* arg4;						// ( 10,  18)
	void* arg3;						// ( 14,  20)
	void* arg5;						// ( 18,  28)
	uint8_t flags2[2];				// ( 1C,  30)
	uint32_t config0;				// ( 20,  34)
	uint32_t vramSize;				// ( 24,  38)
	void* f0[2];					// ( 28,  40)
	uint32_t f1;					// ( 30,  50)
	uint32_t context_mode_bits;		// ( 34,  54)
	uint32_t const* client_data;	// ( 38,  58)
	uint32_t f3[18];				// ( 3C,  60)
	uint8_t flags3[8];				// ( 84,  A8)
	void* f5;						// ( 8C,  B0)
	uint32_t f6;					// ( 90,  B8)
	void* f7[2];					// ( 94,  C0)
	uint32_t f8[2];					// ( 9C,  D0)
	uint64_t f9[3];					// ( A4,  D8)
	uint32_t f10;					// ( BC,  F0)
	GLDReturn last_error;			// ( C0,  F4)
	uint64_t f12[2];				// ( C4,  F8)
	struct _gld_texture_t* stages[16];
									// ( D4, 108) - 16 Sampler Stages
	void* f13[4];					// (114, 188)
	uint32_t f14[8];				// (124, 1A8)
	uint32_t* cb_iter[3];			// (144, 1C8) - 3 pointers for iterating over command buffer
	uint32_t* command_buffer_ptr;	// (150, 1E0)
	size_t command_buffer_size;		// (154, 1E8)
	uint32_t* mem1_addr;			// (158, 1F0)
	size_t mem1_size;				// (15c, 1F8)
	void* pad_addr;					// (160, 200)
	uint32_t pad_size;				// (164, 208)
	struct GLDFence* kfence_addr;	// (168, 210)
	size_t kfence_size_bytes;		// (16C, 218)
	uint32_t* fences_bitmap;		// (170, 220)
	void* f20;						// (174, 228)
	uint32_t f21;					// (178, 230)
	void* f22;						// (17C, 238)
	uint32_t f23;					// (180, 240)
	void* f24;						// (184, 248)
	void* f25;						// (188, 250)
	void* f26[3];					// (18C, 258)
	void* gpdd;						// (198, 270)
	void* f27;						// (19C, 278)
	uint32_t block1[285];			// (1A0, 280)
	uint32_t f28;					// (614, 6F4)
	uint32_t block2[369];			// (618, 6F8)
	uint32_t f29;					// (BDC, CBC)
	void* reserved1[20];			// (BE0, CC0)
	uint32_t reserved2[464];		// (C30, D60)
									// (1370, 14A0)
} gld_context_t;

typedef struct _gld_shared_t {
	io_connect_t obj;        // (0,    0)
	pthread_mutex_t mutex;   // (4,    8)
	long arg2;               // (30,  48)
	display_info_t* dinfo;   // (34,  50)
	uint8_t needs_locking;   // (38,  58)
	uint32_t num_contexts;   // (3C,  5C)
	uint32_t config0;        // (40,  60)
	uint32_t f2;             // (44,  64)
	void* f3;                // (48,  68)
	void* f4;                // (4C,  70)
	void* f5;                // (50,  78)
	void* f6;                // (54,  80)
	uint32_t f7[8];          // (58,  88)
	struct _gld_pipeline_program_t* pp_list;
                             // (78,  A8)
	struct _gld_texture_t* t0[7];
                             // (7C,  B0)
	struct _gld_texture_t* t1[7];
                             // (98,  E8)
	void* channel_memory;    // (B4, 120)
                             // (B8, 128)
} gld_shared_t;

typedef struct _gld_texture_t {
	void* raw_data; // (0, 0)
	struct VendorNewTextureDataStruc tds;
	                // (4, 8)
	void* f6;       // (4C, 50)
	void* f7;       // (50, 58)
	void* f8;       // (54, 60)
	uint16_t f9[6]; // (58, 68)
	void* client_texture;
	                // (64, 78)
	void* f11;      // (68, 80)
	struct GLDSysObject* obj;
					// (6C, 88)
	uint8_t format_index;
					// (70, 90) - index for struct describing pixel format [sIntelTextureInfo]
	uint8_t f14;    // (71, 91)
	uint32_t f15[2];// (74, 94)
                    // (7C, A0)
} gld_texture_t;

typedef struct _gld_vertex_array_t {
	void* f0;
	void* f1;
} gld_vertex_array_t;

typedef struct _vend_ctx_pipe_prog {
	void* f0;	// ( 0,  0)
	struct _vend_ctx_pipe_prog* next;
				// ( 4,  8)
	struct _vend_ctx_pipe_prog* prev;
				// ( 8, 10)
	uint32_t f1[4];
				// ( C, 18)
	void* f2;	// (1C, 28)
				// (20, 30)
} vend_ctx_pipe_prog;

typedef struct _gld_pipeline_program_t {
	void* arg2;        // ( 0,  0)
	struct {
		void* p;       // ( 4,  8)
		uint8_t f;     // ( 8, 10)
	} f0;
	uint32_t f1;       // ( C, 18)
	void* f2;          // (10, 20)
	struct _vend_ctx_pipe_prog* ctx_next;
                       // (14, 28)
	struct _vend_ctx_pipe_prog* ctx_prev;
                       // (18, 30)
	int ctx_count;     // (1C, 38)
	struct _gld_pipeline_program_t* next;
                       // (20, 40)
	struct _gld_pipeline_program_t* prev;
                       // (24, 48)
	uint32_t f3[4];    // (28, 50)
                       // (38, 60)
} gld_pipeline_program_t;

typedef struct _gld_program_t {
	void* f0;
	void* f1;
} gld_program_t;

typedef struct _gld_fence_t {
	uint32_t f0;
	uint8_t f1;
} gld_fence_t;

typedef struct GLDSysObject gld_sys_object_t;

typedef struct _gld_framebuffer_t {
	void* f0;
	void* f1;
	uint32_t f2[2];
} gld_framebuffer_t;

typedef struct _gld_buffer_t {
	void* f0;
	void* f1;
	gld_sys_object_t** f2;
	uint32_t reserved[18];
} gld_buffer_t;

typedef struct _libglimage_t {
	void* handle;
	void* glg_processor_default_data;
	int (*glgConvertType)();
	int (*glgPixelCenters)();
	int (*glgProcessPixelsWithProcessor)();
	void (*glgTerminateProcessor)(void*);
} libglimage_t;

typedef struct _libglprogrammability_t {
	void* handle;
	void (*glpFreePPShaderLinearize)(void*);
	void (*glpFreePPShaderToProgram)(void*);
	int (*glpPPShaderLinearize)();
	int (*glpPPShaderToProgram)();
} libglprogrammability_t;

#ifdef __cplusplus
}
#endif

#endif /* __GLDTYPES_H__ */
