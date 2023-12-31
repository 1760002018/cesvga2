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

#ifndef __CESVGA2GLDRIVER_H__
#define __CESVGA2GLDRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define GLD_DECLARE_GENERIC(name) GLDReturn name(void*, void*, void*, void*, void*, void*)

void gldInitializeLibrary(io_service_t const* pServices,
						  uint8_t const* pServiceFlags,
						  uint32_t GLDisplayMask,
						  PIODataFlush,
						  PIODataBindSurface);
void gldTerminateLibrary(void);

_Bool gldGetVersion(int*, int*, int*, int*);
GLDReturn gldGetRendererInfo(RendererInfo* struct_out, uint32_t GLDisplayMask);
GLDReturn gldChoosePixelFormat(PixelFormat** struct_out, int const* attributes);
GLDReturn gldDestroyPixelFormat(PixelFormat* pixel_format);
GLDReturn gldCreateShared(gld_shared_t** struct_out, uint32_t GLDisplayMask, long arg2);
GLDReturn gldDestroyShared(gld_shared_t* shared);
GLDReturn gldCreateContext(gld_context_t** struct_out,
						   PixelFormat* pixel_format,
						   gld_shared_t* shared,
						   void* arg3,
						   void* arg4,
						   void* arg5);
GLDReturn gldReclaimContext(gld_context_t* context);
GLDReturn gldDestroyContext(gld_context_t* context);
GLDReturn gldAttachDrawable(gld_context_t* context, int surface_type, uint32_t const* client_data, uint32_t options);
GLDReturn gldInitDispatch(gld_context_t* context, GLD_GENERIC_DISPATCH* dispatch_table, uint32_t* data_out);
GLDReturn gldUpdateDispatch(gld_context_t* context, GLD_GENERIC_DISPATCH* dispatch_table, uint32_t* data_out);
char const* gldGetString(uint32_t GLDisplayMask, int string_code);
GLDReturn gldGetError(gld_context_t* context);
GLDReturn gldSetInteger(gld_context_t* context, int arg1, void* arg2);
GLDReturn gldGetInteger(gld_context_t* context, int arg1, void* arg2);
void gldFlush(gld_context_t* context);
void gldFinish(gld_context_t* context);
GLDReturn gldTestObject(gld_context_t* context, int object_type, void const* object);
GLDReturn gldFlushObject(gld_context_t* context, int object_type, int arg2, void const* object);
GLDReturn gldFinishObject(gld_context_t* context, int object_type, void const* object);
GLDReturn gldWaitObject(gld_shared_t* shared, int object_type, void const* object, void const* arg3);
GLDReturn gldCreateTexture(gld_shared_t* shared, gld_texture_t** texture, void* client_texture);
int gldIsTextureResident(gld_shared_t* shared, gld_texture_t* texture);
GLDReturn gldModifyTexture(gld_shared_t* shared, gld_texture_t* texture, uint8_t arg2, int arg3, uint8_t arg4);
GLDReturn gldGenerateTexMipmaps(gld_shared_t* shared, gld_texture_t* texture, int arg2);
GLDReturn gldLoadTexture(gld_context_t* context, gld_texture_t* texture);
void gldUnbindTexture(gld_context_t* context, gld_texture_t* texture);
void gldReclaimTexture(gld_shared_t* shared, gld_texture_t* texture);
void gldDestroyTexture(gld_shared_t* shared, gld_texture_t* texture);
GLDReturn gldCopyTexSubImage(gld_context_t* context,
							 gld_texture_t* texture,
							 int arg2,
							 int arg3,
							 int arg4,
							 int arg5,
							 int arg6,
							 int arg7,
							 int arg8,
							 int arg9,
							 int arg10);
GLDReturn gldModifyTexSubImage(gld_context_t* context,
							   gld_texture_t* texture,
							   int arg2,
							   int arg3,
							   int arg4,
							   int arg5,
							   int arg6,
							   int arg7,
							   int arg8,
							   int arg9,
							   int arg10,
							   int arg11,
							   void* arg12,
							   int arg13,
							   void* arg14);
GLD_DECLARE_GENERIC(gldCreateTextureLevel);
GLDReturn gldGetTextureLevelInfo(void* arg0, void* arg1, void* arg2, int arg3, int arg4, void* arg5);
GLDReturn gldGetTextureLevelImage(void* arg0, void* arg1, int arg2, int arg3);
GLD_DECLARE_GENERIC(gldModifyTextureLevel);
GLD_DECLARE_GENERIC(gldDestroyTextureLevel);
GLDReturn gldCreateBuffer(gld_shared_t* shared, gld_buffer_t** struct_out, void* arg2, void* arg3);
GLDReturn gldBufferSubData(void);
GLDReturn gldLoadBuffer(void);
GLDReturn gldFlushBuffer(gld_shared_t* shared, gld_buffer_t* buffer, void const* base, uint32_t num_bytes);
void gldPageoffBuffer(gld_shared_t* shared, void*, gld_buffer_t* buffer);
void gldUnbindBuffer(gld_context_t* context, gld_buffer_t* buffer);
void gldReclaimBuffer(gld_shared_t* shared, gld_buffer_t* buffer);
GLDReturn gldDestroyBuffer(gld_shared_t* shared, gld_buffer_t* buffer);
void gldGetMemoryPlugin(void* arg0, void* arg1, void** struct_out);
void gldSetMemoryPlugin(void* arg0, void* arg1, void** struct_out);
int gldTestMemoryPlugin(void* arg0, void* arg1);
void gldFlushMemoryPlugin(void* arg0, void* arg1);
void gldDestroyMemoryPlugin(void* arg0, void* arg1);
GLDReturn gldCreateFramebuffer(gld_shared_t * shared, gld_framebuffer_t** struct_out, void* arg2, void* arg3);
void gldUnbindFramebuffer(gld_context_t* context, gld_framebuffer_t* framebuffer);
void gldReclaimFramebuffer(void);
void gldDiscardFramebuffer(void);
GLDReturn gldDestroyFramebuffer(gld_shared_t * shared, gld_framebuffer_t* framebuffer);
GLDReturn gldCreatePipelineProgram(gld_shared_t* shared, gld_pipeline_program_t** struct_out, void* arg2);
GLDReturn gldGetPipelineProgramInfo(gld_shared_t* shared, gld_pipeline_program_t* pp, int arg2, void* arg3);
GLDReturn gldModifyPipelineProgram(gld_shared_t* shared, gld_pipeline_program_t* pp, uint32_t arg2);
GLDReturn gldUnbindPipelineProgram(gld_context_t* context, gld_pipeline_program_t* pp);
GLDReturn gldDestroyPipelineProgram(gld_shared_t* shared, gld_pipeline_program_t* pp);
GLDReturn gldCreateProgram(gld_shared_t* shared, gld_program_t** struct_out, void* arg2, void* arg3);
GLDReturn gldDestroyProgram(gld_shared_t* shared, gld_program_t* program);
GLDReturn gldCreateVertexArray(gld_shared_t* shared, gld_vertex_array_t** struct_out, void* arg2, void* arg3);
GLDReturn gldModifyVertexArray(void);
GLDReturn gldFlushVertexArray(void);
void gldUnbindVertexArray(void);
void gldReclaimVertexArray(void);
GLDReturn gldDestroyVertexArray(gld_shared_t* shared, gld_vertex_array_t* vertex_array);
GLDReturn gldCreateFence(gld_context_t* context, gld_fence_t** struct_out);
GLDReturn gldDestroyFence(gld_context_t* context, gld_fence_t* fence);
GLDReturn gldCreateQuery(gld_context_t* context, uintptr_t* struct_out);
GLDReturn gldModifyQuery(void);
GLDReturn gldGetQueryInfo(void);
GLDReturn gldDestroyQuery(void);
GLDReturn gldObjectPurgeable(gld_context_t* context, int object_type, void* object, int arg3, int arg4);
GLDReturn gldObjectUnpurgeable(gld_context_t* context, int object_type, void* object, int arg3, void* arg4);
GLDReturn gldCreateComputeContext(void);
GLDReturn gldDestroyComputeContext(void);
GLDReturn gldLoadHostBuffer(void);
GLDReturn gldSyncBufferObject(void);
GLDReturn gldSyncTexture(void);
GLD_DECLARE_GENERIC(gldGetTextureLevel);
GLD_DECLARE_GENERIC(gldDeleteTextureLevel);
GLD_DECLARE_GENERIC(gldDeleteTexture);
GLD_DECLARE_GENERIC(gldAllocVertexBuffer);
GLD_DECLARE_GENERIC(gldCompleteVertexBuffer);
GLD_DECLARE_GENERIC(gldFreeVertexBuffer);
GLD_DECLARE_GENERIC(gldGetMemoryPluginData);
GLD_DECLARE_GENERIC(gldSetMemoryPluginData);
GLD_DECLARE_GENERIC(gldFinishMemoryPluginData);
GLD_DECLARE_GENERIC(gldTestMemoryPluginData);
GLD_DECLARE_GENERIC(gldDestroyMemoryPluginData);
	
#ifdef __cplusplus
}
#endif

#endif /* __CESVGA2GLDRIVER_H__ */
