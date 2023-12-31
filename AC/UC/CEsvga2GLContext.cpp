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

#include <IOKit/IOBufferMemoryDescriptor.h>
#include <libkern/version.h>
#define GL_INCL_SHARED
#define GL_INCL_PUBLIC
#define GL_INCL_PRIVATE
#include "GLCommon.h"
#include "UCGLDCommonTypes.h"
#include "UCMethods.h"
#include "VLog.h"
#include "CEsvga2Accel.h"
#include "CEsvga2Device.h"
#include "CEsvga2GLContext.h"
#include "CEsvga2IPP.h"
#include "CEsvga2Shared.h"
#include "CEsvga2Surface.h"

#define CLASS CEsvga2GLContext
#define super IOUserClient
OSDefineMetaClassAndStructors(CEsvga2GLContext, IOUserClient);

#if LOGGING_LEVEL >= 1
#define GLLog(log_level, ...) do { if (log_level <= m_log_level) VLog("IOGL: ", ##__VA_ARGS__); } while (false)
#else
#define GLLog(log_level, ...)
#endif

#define HIDDEN __attribute__((visibility("hidden")))

static
IOExternalMethod const iofbFuncsCache[kIOCEGLNumMethods] =
{
// Note: methods from IONVGLContext
{0, reinterpret_cast<IOMethod>(&CLASS::set_surface), kIOUCScalarIScalarO, 4, 0},
{0, reinterpret_cast<IOMethod>(&CLASS::set_swap_rect), kIOUCScalarIScalarO, 4, 0},
{0, reinterpret_cast<IOMethod>(&CLASS::set_swap_interval), kIOUCScalarIScalarO, 2, 0},
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060
{0, reinterpret_cast<IOMethod>(&CLASS::get_config), kIOUCScalarIScalarO, 0, 3},
#endif
{0, reinterpret_cast<IOMethod>(&CLASS::get_surface_size), kIOUCScalarIScalarO, 0, 4},
{0, reinterpret_cast<IOMethod>(&CLASS::get_surface_info), kIOUCScalarIScalarO, 1, 3},
{0, reinterpret_cast<IOMethod>(&CLASS::read_buffer), kIOUCScalarIStructI, 0, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::finish), kIOUCScalarIScalarO, 0, 0},
{0, reinterpret_cast<IOMethod>(&CLASS::wait_for_stamp), kIOUCScalarIScalarO, 1, 0},
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060
{0, reinterpret_cast<IOMethod>(&CLASS::new_texture), kIOUCStructIStructO, kIOUCVariableStructureSize, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::delete_texture), kIOUCScalarIScalarO, 1, 0},
#endif
{0, reinterpret_cast<IOMethod>(&CLASS::become_global_shared), kIOUCScalarIScalarO, 1, 0},
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060
{0, reinterpret_cast<IOMethod>(&CLASS::page_off_texture), kIOUCScalarIStructI, 0, kIOUCVariableStructureSize},
#endif
{0, reinterpret_cast<IOMethod>(&CLASS::purge_texture), kIOUCScalarIScalarO, 1, 0},
{0, reinterpret_cast<IOMethod>(&CLASS::set_surface_volatile_state), kIOUCScalarIScalarO, 1, 0},
{0, reinterpret_cast<IOMethod>(&CLASS::set_surface_get_config_status), kIOUCStructIStructO, kIOUCVariableStructureSize, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::reclaim_resources), kIOUCScalarIScalarO, 0, 0},
{0, reinterpret_cast<IOMethod>(&CLASS::get_data_buffer), kIOUCScalarIStructO, 0, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::set_stereo), kIOUCScalarIScalarO, 2, 0},
{0, reinterpret_cast<IOMethod>(&CLASS::purge_accelerator), kIOUCScalarIScalarO, 1, 0},
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060
{0, reinterpret_cast<IOMethod>(&CLASS::get_channel_memory), kIOUCScalarIStructO, 0, kIOUCVariableStructureSize},
#else
{0, reinterpret_cast<IOMethod>(&CLASS::submit_command_buffer), kIOUCScalarIStructO, 1, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::filter_control), kIOUCStructIStructO, kIOUCVariableStructureSize, kIOUCVariableStructureSize},
#endif
// Note: Methods from NVGLContext
{0, reinterpret_cast<IOMethod>(&CLASS::get_query_buffer), kIOUCScalarIStructO, 1, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::get_notifiers), kIOUCScalarIScalarO, 0, 2},
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060
{0, reinterpret_cast<IOMethod>(&CLASS::new_heap_object), kIOUCStructIStructO, kIOUCVariableStructureSize, kIOUCVariableStructureSize},
#endif
{0, reinterpret_cast<IOMethod>(&CLASS::kernel_printf), kIOUCScalarIStructI, 0, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::nv_rm_config_get), kIOUCStructIStructO, kIOUCVariableStructureSize, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::nv_rm_config_get_ex), kIOUCStructIStructO, kIOUCVariableStructureSize, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::nv_client_request), kIOUCStructIStructO, kIOUCVariableStructureSize, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::pageoff_surface_texture), kIOUCScalarIStructI, 0, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::get_data_buffer_with_offset), kIOUCScalarIStructO, 0, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::nv_rm_control), kIOUCStructIStructO, kIOUCVariableStructureSize, kIOUCVariableStructureSize},
{0, reinterpret_cast<IOMethod>(&CLASS::get_power_state), kIOUCScalarIScalarO, 0, 2},
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
{0, reinterpret_cast<IOMethod>(&CLASS::set_watchdog_timer), kIOUCScalarIScalarO, 1, 0},
{0, reinterpret_cast<IOMethod>(&CLASS::GetHandleIndex), kIOUCScalarIScalarO, 0, 2},
{0, reinterpret_cast<IOMethod>(&CLASS::ForceTextureLargePages), kIOUCScalarIScalarO, 1, 0},
#endif
// Note: CE Methods
};

#pragma mark -
#pragma mark Global Functions
#pragma mark -

static inline
void lockAccel(CEsvga2Accel* accel)
{
}

static inline
void unlockAccel(CEsvga2Accel* accel)
{
}

#if 1
static inline
void memset32(void* dest, uint32_t value, size_t size)
{
	__asm__ volatile ("cld; rep stosl" : "+c" (size), "+D" (dest) : "a" (value) : "memory");
}

void readHostVramSimple(CEsvga2Accel* m_provider, uint32_t sid, size_t offset_in_vram, uint32_t w, uint32_t h, uint32_t pitch, uint32_t* fence);
void blitGarbageToScreen(CEsvga2Accel* m_provider, size_t offset_in_vram, uint32_t w, uint32_t h, uint32_t pitch);
#endif

#pragma mark -
#pragma mark Private Methods
#pragma mark -

HIDDEN
void CLASS::Init()
{
	m_command_buffer.xfer.init();
	m_context_buffer0.xfer.init();
	m_context_buffer1.xfer.init();
}

HIDDEN
void CLASS::Cleanup()
{
	if (m_ipp) {
		m_ipp->stop();
		m_ipp->release();
		m_ipp = 0;
	}
	CleanupApp();
	if (m_shared) {
		m_shared->release();
		m_shared = 0;
	}
	if (m_surface_client) {
		m_surface_client->detachGL();
		m_surface_client->release();
		m_surface_client = 0;
	}
	if (m_gc) {
		m_gc->flushCollection();
		m_gc->release();
		m_gc = 0;
	}
	m_command_buffer.xfer.complete(m_provider);
	m_command_buffer.xfer.discard();
	m_context_buffer0.xfer.discard();
	m_context_buffer1.xfer.discard();
	if (m_fences) {
		m_fences->release();
		m_fences = 0;
		m_fences_len = 0U;
		m_fences_ptr = 0;
	}
}

HIDDEN
bool CLASS::allocCommandBuffer(CEsvga2CommandBuffer* buffer, size_t size)
{
	IOBufferMemoryDescriptor* bmd;

	/*
	 * Intel915 ors an optional flag @ IOAccelerator+0x924
	 */
	bmd = IOBufferMemoryDescriptor::inTaskWithPhysicalMask(kernel_task,
														   kIOMemoryKernelUserShared |
														   kIOMemoryPageable |
														   kIODirectionInOut,
														   size,
														   ((1ULL << (32 + PAGE_SHIFT)) - 1U) & -PAGE_SIZE);
	buffer->xfer.md = bmd;
	if (!bmd)
		return false;
	buffer->size = size;
	buffer->kernel_ptr = static_cast<VendorCommandBufferHeader*>(bmd->getBytesNoCopy());
	initCommandBufferHeader(buffer->kernel_ptr, size);
	return true;
}

HIDDEN
void CLASS::initCommandBufferHeader(VendorCommandBufferHeader* buffer_ptr, size_t size)
{
	bzero(buffer_ptr, sizeof *buffer_ptr);
	buffer_ptr->size_dwords = static_cast<uint32_t>((size - sizeof *buffer_ptr) / sizeof(uint32_t));
	buffer_ptr->stamp = 0;	// Intel915 puts (submitStamp - 1) here
}

HIDDEN
bool CLASS::allocAllContextBuffers()
{
	IOBufferMemoryDescriptor* bmd;
	VendorCommandBufferHeader* p;

	bmd = IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task,
													  kIOMemoryKernelUserShared |
													  kIOMemoryPageable |
													  kIODirectionInOut,
													  PAGE_SIZE,
													  page_size);
	m_context_buffer0.xfer.md = bmd;
	if (!bmd)
		return false;
	m_context_buffer0.kernel_ptr = static_cast<VendorCommandBufferHeader*>(bmd->getBytesNoCopy());
	p = m_context_buffer0.kernel_ptr;
	bzero(p, sizeof *p);
	p->flags = 1;
	p->data[3] = 1007;

#if 1
	// This buffer is never used, so skip it
	/*
	 * Intel915 ors an optional flag @ IOAccelerator+0x924
	 */
	bmd = IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task,
													  kIOMemoryKernelUserShared |
													  kIOMemoryPageable |
													  kIODirectionInOut,
													  PAGE_SIZE,
													  page_size);
	if (!bmd)
		return false; // TBD frees previous ones
	m_context_buffer1.xfer.md = bmd;
	m_context_buffer1.kernel_ptr = static_cast<VendorCommandBufferHeader*>(bmd->getBytesNoCopy());
	p = m_context_buffer1.kernel_ptr;
	bzero(p, sizeof *p);
	p->flags = 1;
	p->data[3] = 1007;
	// TBD: allocates another one like this
#endif
	return true;
}

HIDDEN
IOReturn CLASS::get_status(uint32_t* status)
{
	/*
	 * crazy function that sets the status to 0 or 1
	 */
	*status = 1;
	return kIOReturnSuccess;
}

#pragma mark -
#pragma mark IOUserClient Methods
#pragma mark -

IOExternalMethod* CLASS::getTargetAndMethodForIndex(IOService** targetP, UInt32 index)
{
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
	if (index >= kIOCEGLFilterControl && (version_major != 10 || version_minor != 8))
		++index;
#endif
	if (index >= kIOCEGLNumMethods)
		GLLog(2, "%s(target_out, %u)\n", __FUNCTION__, static_cast<unsigned>(index));
	if (!targetP || index >= kIOCEGLNumMethods)
		return 0;
#if 1
	if (index >= kIOCEGLNumMethods) {
		if (m_provider)
			*targetP = m_provider;
		else
			return 0;
	} else
		*targetP = this;
#else
	*targetP = this;
#endif
	return const_cast<IOExternalMethod*>(&iofbFuncsCache[index]);
}

IOReturn CLASS::clientClose()
{
	GLLog(2, "%s\n", __FUNCTION__);
	Cleanup();
	if (!terminate(0))
		IOLog("%s: terminate failed\n", __FUNCTION__);
	m_owning_task = 0;
	m_provider = 0;
	return kIOReturnSuccess;
}

IOReturn CLASS::clientMemoryForType(UInt32 type, IOOptionBits* options, IOMemoryDescriptor** memory)
{
	VendorCommandBufferHeader* p;
	IOMemoryDescriptor* md;
	IOBufferMemoryDescriptor* bmd;
	void* fptr;
	size_t d;
	VendorCommandDescriptor result;
	uint32_t pcbRet;

#if LOGGING_LEVEL >= 3
	GLLog(3, "%s(%u, options_out, memory_out)\n", __FUNCTION__, static_cast<unsigned>(type));
#endif
	if (type > 4U || !options || !memory)
		return kIOReturnBadArgument;
	if (m_stream_error) {
		*options = m_stream_error;
		*memory = 0;
		return kIOReturnBadArgument;
	}
	/*
	 * Notes:
	 *   Cases 1 & 2 are called from GLD gldCreateContext
	 *   Cases 0 & 4 are called from submit_command_buffer
	 *   Case 3 is not used
	 */
	switch (type) {
		case 0:
			/*
			 * TBD: Huge amount of code to process previous command buffer
			 *   A0B7-AB58
			 */
			if (!m_shared || !m_ipp)
				return kIOReturnNotReady;
			m_shared->lockShared();
#if 1
			for (d = 0U; d != 21U; ++d)
				if (m_txs[i])
					addTextureToStream(m_txs[i]);
#endif
			pcbRet = processCommandBuffer(&result);
			if (m_stream_error) {
				m_shared->unlockShared();
				*options = m_stream_error;
				*memory = 0;
				return kIOReturnBadArgument;
			}
#if 1
			for (d = 0U; d != 21U; ++d)
				if (m_txs[d])
					removeTextureFromStream(m_txs[d]);
#endif
			m_shared->unlockShared();
			if (result.ds_count_dwords)
				m_ipp->submit_buffer(result.next, result.ds_count_dwords);
			if (pcbRet & 2U) {
				if (m_fbo[0])
					touchDrawFBO();
				if (m_surface_client)
					m_surface_client->touchRenderTarget();
			}
			m_command_buffer.xfer.complete(m_provider);
			lockAccel(m_provider);
			/*
			 * AB58: reinitialize buffer
			 */
			md = m_command_buffer.xfer.md;
			if (!md)
				return kIOReturnNoResources;
			md->retain();
			*options = 0;
			*memory = md;
			p = m_command_buffer.kernel_ptr;
			initCommandBufferHeader(p, m_command_buffer.size);
			p->flags = pcbRet /* var_88 */;
			p->downstream[-1] = 1;
			p->downstream[0] = 1U << 24;	// terminating token
			p->stamp = 0 /* this->0x7C */;
			unlockAccel(m_provider);
#if 1
			sleepForSwapCompleteNoLock(var_84);
#endif
			return kIOReturnSuccess;
		case 1:
			lockAccel(m_provider);
			md = m_context_buffer0.xfer.md;
			if (!md) {
				unlockAccel(m_provider);
				return kIOReturnNoResources;
			}
			md->retain();
			*options = 0;
			*memory = md;
			p = m_context_buffer0.kernel_ptr;
			bzero(p, sizeof *p);
			p->flags = 1;
			p->data[3] = 1007;
			unlockAccel(m_provider);
			return kIOReturnSuccess;
		case 2:
			if (!m_fences) {
				m_fences_len = page_size;
				bmd = IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task,
																  kIOMemoryKernelUserShared |
																  kIOMemoryPageable |
																  kIODirectionInOut,
																  m_fences_len,
																  page_size);
				m_fences = bmd;
				if (!bmd)
					return kIOReturnNoResources;
				m_fences_ptr = static_cast<typeof m_fences_ptr>(bmd->getBytesNoCopy());
			} else {
				d = 2U * m_fences_len;
				bmd = IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task,
																  kIOMemoryKernelUserShared |
																  kIOMemoryPageable |
																  kIODirectionInOut,
																  d,
																  page_size);
				if (!bmd)
					return kIOReturnNoResources;
				fptr = bmd->getBytesNoCopy();
				memcpy(fptr, m_fences_ptr, m_fences_len);
				m_fences->release();
				m_fences = bmd;
				m_fences_len = d;
				m_fences_ptr = static_cast<typeof m_fences_ptr>(fptr);
			}
			if (m_ipp)
				m_ipp->setFences(m_fences_ptr, m_fences_len);
			m_fences->retain();
			*options = 0;
			*memory = m_fences;
			return kIOReturnSuccess;
		case 3:
#if 1
			md = m_provider->offset 0xB4;
			if (!md)
				return kIOReturnNoResources;
			md->retain();
			*options = 0;
			*memory = md;
			return kIOReturnSuccess;
#endif
			break;
		case 4:
			lockAccel(m_provider);
			md = m_command_buffer.xfer.md;
			if (!md) {
				unlockAccel(m_provider);
				return kIOReturnNoResources;
			}
			md->retain();
			*options = 0;
			*memory = md;
			p = m_command_buffer.kernel_ptr;
			initCommandBufferHeader(p, m_command_buffer.size);
			p->flags = 0;
			p->downstream[-1] = 1;
			p->downstream[0] = 1U << 24;// terminating token
			p->stamp = 0;				// TBD: from this+0x7C
			unlockAccel(m_provider);
			return kIOReturnSuccess;
	}
	/*
	 * Note: Intel GMA 950 defaults to returning kIOReturnBadArgument
	 */
	return super::clientMemoryForType(type, options, memory);
}

IOReturn CLASS::connectClient(IOUserClient* client)
{
	CEsvga2Device* d;
	CEsvga2Shared* s;
	GLLog(2, "%s(%p), name == %s\n", __FUNCTION__, client, client ? client->getName() : "NULL");
	d = OSDynamicCast(CEsvga2Device, client);
	if (!d)
		return kIOReturnError;
	if (d->getOwningTask() != m_owning_task)
		return kIOReturnNotPermitted;
	s = d->getShared();
	if (!s)
		return kIOReturnNotReady;
	s->retain();
	if (m_shared)
		m_shared->release();
	m_shared = s;
	return kIOReturnSuccess;
}

#if 1
/*
 * Note: IONVGLContext has an override on this method
 *   In OS 10.5 it redirects function number 17 to get_data_buffer()
 *   In OS 10.6 it redirects function number 13 to get_data_buffer()
 *     Sets structureOutputSize for get_data_buffer() to 12
 */
IOReturn CLASS::externalMethod(uint32_t selector, IOExternalMethodArguments* arguments, IOExternalMethodDispatch* dispatch, OSObject* target, void* reference)
{
	return super::externalMethod(selector, arguments, dispatch, target, reference);
}
#endif

bool CLASS::start(IOService* provider)
{
	m_provider = OSDynamicCast(CEsvga2Accel, provider);
	if (!m_provider)
		return false;
	if (!super::start(provider))
		return false;
	m_log_level = imax(m_provider->getLogLevelGLD(), m_provider->getLogLevelAC());
	m_mem_type = 4U;
	m_gc = OSSet::withCapacity(2U);
	if (!m_gc) {
		GLLog(1, "%s: OSSet::withCapacity failed\n", __FUNCTION__);
		super::stop(provider);
		return false;
	}
	m_ipp = CEsvga2IPP::factory();
	if (!m_ipp ||
		!m_ipp->start(m_provider, m_log_level)) {
		GLLog(1, "%s: Failed to create Pipeline Processor\n", __FUNCTION__);
		goto bad;
	}
	// TBD getVRAMDescriptors
	if (!allocAllContextBuffers()) {
		GLLog(1, "%s: allocAllContextBuffers failed\n", __FUNCTION__);
		goto bad;
	}
	if (!allocCommandBuffer(&m_command_buffer, 0x10000U)) {
		GLLog(1, "%s: allocCommandBuffer failed\n", __FUNCTION__);
		goto bad;
	}
	m_command_buffer.kernel_ptr->flags = 2U;
	m_command_buffer.kernel_ptr->downstream[1] = 1U << 24;
	return true;

bad:
	Cleanup();
	super::stop(provider);
	return false;
}

bool CLASS::initWithTask(task_t owningTask, void* securityToken, UInt32 type)
{
	m_log_level = LOGGING_LEVEL;
	if (!super::initWithTask(owningTask, securityToken, type))
		return false;
	m_owning_task = owningTask;
	Init();
	return true;
}

HIDDEN
CLASS* CLASS::withTask(task_t owningTask, void* securityToken, uint32_t type)
{
	CLASS* inst;

	inst = new CLASS;

	if (inst && !inst->initWithTask(owningTask, securityToken, type))
	{
		inst->release();
		inst = 0;
	}

	return (inst);
}

#pragma mark -
#pragma mark IONVGLContext Methods
#pragma mark -

HIDDEN
IOReturn CLASS::set_surface(uintptr_t surface_id, uintptr_t /* eIOGLContextModeBits */ context_mode_bits, uintptr_t c3, uintptr_t c4)
{
	CEsvga2Surface* surface_client;
	IOReturn rc;
	GLLog(2, "%s(%#lx, %#lx, %lu, %lu)\n", __FUNCTION__, surface_id, context_mode_bits, c3, c4);
	if (!m_ipp)
		return kIOReturnNotReady;
	m_ipp->discard_cached_state();
	if (!surface_id) {
		if (m_surface_client) {
			if (!m_fbo[0])
				m_ipp->detach_render_targets();
			m_surface_client->detachGL();
			m_surface_client->release();
			m_surface_client = 0;
		}
		return kIOReturnSuccess;
	}
	if (!m_provider)
		return kIOReturnNotReady;
	surface_client = m_provider->findSurfaceForID(static_cast<uint32_t>(surface_id));
	if (!surface_client)
		return kIOReturnNotFound;
	if (surface_client == m_surface_client)
		/*
		 * Note: When the surface is resized (any size change), the
		 *   GLD issues a new DrawBuffer command, which causes
		 *   the new surfaces to get attached, in case they were
		 *   recreated by resizeGL().
		 *   If the size doesn't change, the GLD does not issue
		 *   a new DrawBuffer command.
		 */
		return m_surface_client->resizeGL();
	surface_client->retain();
	if (m_surface_client) {
		if (!m_fbo[0])
			m_ipp->detach_render_targets();
		m_surface_client->detachGL();
		m_surface_client->release();
		m_surface_client = 0;
	}
	rc = surface_client->attachGL(static_cast<uint32_t>(context_mode_bits));
	if (rc != kIOReturnSuccess) {
		surface_client->release();
		GLLog(1, "%s: attachGL failed, rc %#x\n", __FUNCTION__, rc);
		return rc;
	}
	m_surface_client = surface_client;
	GLLog(3, "%s:   surface_client %p\n", __FUNCTION__, m_surface_client);
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::set_swap_rect(intptr_t x, intptr_t y, intptr_t w, intptr_t h)
{
	GLLog(2, "%s(%ld, %ld, %ld, %ld)\n", __FUNCTION__, x, y, w, h);
#if 1
	IOAccelBounds this->0x94;
	this->0x94.x = x;
	this->0x94.y = y;
	this->0x94.w = w;
	this->0x94.h = h;
#endif
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::set_swap_interval(intptr_t c1, intptr_t c2)
{
	GLLog(2, "%s(%ld, %ld)\n", __FUNCTION__, c1, c2);
#if 1
	IOAccelSize this->0x9C;
	this->0x9C.w = c1;
	this->0x9C.h = c2;
#endif
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::get_config(uint32_t* c1, uint32_t* c2, uint32_t* c3)
{
	uint32_t const vram_size = m_provider->getVRAMSize();

	// 0x40000U - GMA 950 [The GMA 900 is unsupported on OS 10.7]
	*c1 = version_major >= 11 ? 0x40000U : 0U;
	*c2 = vram_size;		// same as c3 in CEsvga2Device::get_config, total memory available for textures (no accounting by CEsvga2)
	*c3 = vram_size;		// same as c4 in CEsvga2Device::get_config, total VRAM size
	GLLog(2, "%s(*%u, *%u, *%u)\n", __FUNCTION__, *c1, *c2, *c3);
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::get_surface_size(uint32_t* inner_width, uint32_t* inner_height, uint32_t* outer_width, uint32_t* outer_height)
{
	if (!m_surface_client)
		return kIOReturnError;
	m_surface_client->getBoundsForGL(inner_width, inner_height, outer_width, outer_height);
	GLLog(2, "%s(*%u, *%u, *%u, *%u)\n", __FUNCTION__, *inner_width, *inner_height, *outer_width, *outer_height);
#if 1
	ebx = m_surface_client->0xE70.w;	// sign extended
	ecx = m_surface_client->0xE70.h;	// sign extended
	edi = m_surface_client->0xE6C.w;	// zero extended
	eax = m_surface_client->0xE6C.h;	// zero extended
	if (ebx != edi || ecx != eax) {
		esi = ebx;
		ebx = ecx;
		ecx = edi;
		edx = eax;
	} else {
		ebx = this->0x1A8;
		eax = m_surface_client->0xE14;	// associated Intel915TextureBuffer*
		ecx = eax->width;
		edx = eax->height;	// zero extended
		while (ebx > 0) {
			if (ecx >= 2) ecx /= 2;
			if (edx >= 2) edx /= 2;
			--ebx;
		}
		esi = ecx;
		ebx = edx;
	}
	*inner_width  = esi;
	*inner_height = ebx;
	*outer_width  = ecx;
	*outer_height = edx;
#endif
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::get_surface_info(uintptr_t surface_id, uint32_t* mode_bits, uint32_t* width, uint32_t* height)
{
	CEsvga2Surface* surface_client;
	uint32_t inner_width, inner_height, outer_width, outer_height;

	if (!m_provider)
		return kIOReturnNotReady;
	lockAccel(m_provider);
	if (!surface_id)
		goto bad_exit;
	surface_client = m_provider->findSurfaceForID(static_cast<uint32_t>(surface_id));
	if (!surface_client)
		goto bad_exit;
	*mode_bits = static_cast<uint32_t>(surface_client->getOriginalModeBits());
#if 1
	uint32_t some_mask = surface_client->0x10F0;
	if (some_mask & 8U)
		*mode_bits |= 0x200U;
	else if (some_mask & 4U)
		*mode_bits |= 0x100U;
#endif
	surface_client->getBoundsForGL(&inner_width,
								   &inner_height,
								   &outer_width,
								   &outer_height);
#if 1
	if (inner_width != outer_width || inner_height != outer_height) {
		*width  = inner_width;
		*height = inner_height;
	} else {
		*width  = surface_client->0xE14->width;
		*height = surface_client->0xE14->height;	// zero extended
	}
#else
	*width  = outer_width;
	*height = outer_height;
#endif
	unlockAccel(m_provider);
	GLLog(3, "%s(%#lx, *%#x, *%u, *%u)\n", __FUNCTION__, surface_id, *mode_bits, *width, *height);
	return kIOReturnSuccess;

bad_exit:
	*mode_bits = 0U;
	*width = 0U;
	*height = 0U;
	unlockAccel(m_provider);
	return kIOReturnBadArgument;
}

HIDDEN
IOReturn CLASS::read_buffer(struct sIOGLContextReadBufferData const* struct_in, size_t struct_in_size)
{
	IOReturn rc;
	VendorTransferBuffer xfer;
	SVGA3dSurfaceImageId hostImage;
	SVGA3dCopyBox copyBox;
	CEsvga2Accel::ExtraInfoEx extra;

	GLLog(2, "%s(struct_in, %lu)\n", __FUNCTION__, struct_in_size);
	if (struct_in_size < sizeof *struct_in)
		return kIOReturnBadArgument;
#if LOGGING_LEVEL >= 3
	GLLog(3, "%s:  x == %u, y == %u, w == %u, h == %u, data_type == %u, pitch == %u, addr == %#llx\n", __FUNCTION__,
		  struct_in->x, struct_in->y, struct_in->width, struct_in->height,
		  struct_in->data_type, struct_in->pitch, struct_in->addr);
#endif
#if 1
	// var_20 = 0xa0a;
	// var_34 = 3;
	// var_74 = struct_in->data[0];
	// var_70 = struct_in->data[1];
	// var_6c = struct_in->data[2];
	// var_68 = struct_in->data[3];
	// var_60 = struct_in->addr;
	switch (struct_in->data_type) {
		case 0U: // 7D47
			selector = 1U;
			break;
		case 1U: // 7D50
			selector = 0U;
			break;
		case 4U: // 7D59
			selector = 4U;
			break;
		case 7U: // 7D62
			selector = 2U;
			break;
		case 8U: // 7D6B
			selector = 3U;
			break;
		case 10U: // 7D35
			selector = 5U;
			break;
		case 11U: // 7D3E
			selector = 6U;
			break;
		default:
			return kIOReturnBadArgument;
	}
#endif
	/*
	 * Known data types
	 *   4U - depth buffer
	 */
	if (!m_surface_client)	// Note: Apple's code doesn't implement read_buffer on a ReadFBO (!)
		return kIOReturnCannotLock;
	if (struct_in->data_type != 4U)
		return kIOReturnSuccess /* kIOReturnUnsupported */;
	bzero(&hostImage, sizeof hostImage);
	if (!m_surface_client->getSurfacesForGL(0, &hostImage.sid))
		return kIOReturnCannotLock;
	if (!isIdValid(hostImage.sid))
		return kIOReturnNotAttached;
	bzero(&xfer, sizeof xfer);
	xfer.init();
	bzero(&copyBox, sizeof copyBox);
	copyBox.x = struct_in->x;
	copyBox.y = struct_in->y;
	copyBox.w = struct_in->width;
	copyBox.h = struct_in->height;
	copyBox.d = 1U;
	extra.mem_offset_in_gmr = static_cast<vm_offset_t>(struct_in->addr & page_mask);
	extra.mem_pitch = struct_in->pitch;
	extra.mem_limit = 0xFFFFFFFFU;
	extra.suffix_flags = 2U;
	/*
	 * Note: should check that address doesn't refer to a read-only region,
	 *   but Apple's code doesn't do this.
	 */
	xfer.md = IOMemoryDescriptor::withAddressRange(struct_in->addr,
												   struct_in->height * struct_in->pitch,
												   kIODirectionInOut,
												   m_owning_task);
	if (!xfer.md)
		return kIOReturnNoResources;
	rc = xfer.prepare(m_provider);
	if (rc != kIOReturnSuccess) {
		xfer.discard();
		return rc;
	}
	extra.mem_gmr_id = xfer.gmr_id;
	rc = m_provider->surfaceDMA3DEx(&hostImage,
									SVGA3D_READ_HOST_VRAM,
									&copyBox,
									&extra,
									&xfer.fence);
	xfer.complete(m_provider);
	xfer.discard();
	return rc;
}

HIDDEN
IOReturn CLASS::finish()
{
	GLLog(2, "%s()\n", __FUNCTION__);
	if (m_provider)
		return m_provider->SyncFIFO();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::wait_for_stamp(uintptr_t stamp)
{
	GLLog(2, "%s(%lu)\n", __FUNCTION__, stamp);
	if (stamp && m_provider)
		m_provider->SyncToFence(static_cast<uint32_t>(stamp));
	return kIOReturnSuccess;
}

#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060
HIDDEN
IOReturn CLASS::new_texture(struct VendorNewTextureDataStruc const* struct_in,
							struct sIONewTextureReturnData* struct_out,
							size_t struct_in_size,
							size_t* struct_out_size)
{
	GLLog(2, "%s(struct_in, struct_out, %lu, %lu)\n", __FUNCTION__, struct_in_size, *struct_out_size);
	bzero(struct_out, *struct_out_size);
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::delete_texture(uintptr_t c1)
{
	GLLog(2, "%s(%lu)\n", __FUNCTION__, c1);
	return kIOReturnSuccess;
}
#endif

HIDDEN
IOReturn CLASS::become_global_shared(uintptr_t c1)
{
	GLLog(2, "%s(%lu)\n", __FUNCTION__, c1);
	return kIOReturnUnsupported;	// Not implemented in Intel GMA 950
}

#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060
HIDDEN
IOReturn CLASS::page_off_texture(struct sIODevicePageoffTexture const* struct_in, size_t struct_in_size)
{
	GLLog(2, "%s(struct_in, %lu)\n", __FUNCTION__, struct_in_size);
	return kIOReturnSuccess;
}
#endif

HIDDEN
IOReturn CLASS::purge_texture(uintptr_t c1)
{
	GLLog(2, "%s(%lu)\n", __FUNCTION__, c1);
	/*
	 * TBD
	 */
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::set_surface_volatile_state(uintptr_t /* eSurfaceVolatileState */ new_state)
{
	GLLog(2, "%s(%lu)\n", __FUNCTION__, new_state);
#if 1
	lockAccel(m_provider);
	this->0xC0 = new_state;
	if (surface_client)
		surface_client->set_volatile_state(new_state);
	unlockAccel(m_provider);
#endif
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::set_surface_get_config_status(struct sIOGLContextSetSurfaceData const* struct_in,
											  struct sIOGLContextGetConfigStatus* struct_out,
											  size_t struct_in_size,
											  size_t* struct_out_size)
{
	CEsvga2Surface* surface_client;
	IOReturn rc;
	eIOAccelSurfaceScaleBits scale_options;
	IOAccelSurfaceScaling scale;	// var_44

	GLLog(2, "%s(struct_in, struct_out, %lu, %lu)\n", __FUNCTION__, struct_in_size, *struct_out_size);
	if (struct_in_size < sizeof *struct_in ||
		*struct_out_size < sizeof *struct_out)
		return kIOReturnBadArgument;
	if (!m_provider)
		return kIOReturnNotReady;
#if LOGGING_LEVEL >= 2
	if (m_log_level >= 3) {
		GLLog(3, "%s:   surface_id == %#x, cmb == %#x, smb == %#x, dr_hi == %u, dr_lo == %u, volatile == %u\n", __FUNCTION__,
			  struct_in->surface_id, struct_in->context_mode_bits, struct_in->surface_mode,
			  struct_in->dr_options_hi, struct_in->dr_options_lo, struct_in->volatile_state);
		GLLog(3, "%s:   set_scale == %u, scale_options == %#x, scale_width == %u, scale_height == %u\n", __FUNCTION__,
			  struct_in->set_scale, struct_in->scale_options,
			  static_cast<uint16_t>(struct_in->scale_width),
			  static_cast<uint16_t>(struct_in->scale_height));
	}
#endif
	*struct_out_size = sizeof *struct_out;
	bzero(struct_out, sizeof *struct_out);
	if (struct_in->surface_id &&
		struct_in->surface_mode != 0xFFFFFFFFU) {
		lockAccel(m_provider);
		surface_client = m_provider->findSurfaceForID(struct_in->surface_id);
		unlockAccel(m_provider);
		if (surface_client &&
			((surface_client->getOriginalModeBits() & 15) != static_cast<int>(struct_in->surface_mode & 15U)))
			return kIOReturnError;
	}
	rc = set_surface(struct_in->surface_id,
					 struct_in->context_mode_bits,
					 struct_in->dr_options_hi,
					 struct_in->dr_options_lo);
	if (rc != kIOReturnSuccess)
		return rc;
	if (!m_surface_client)	// Moved
		return kIOReturnError;
	if (struct_in->set_scale) {
		if (!(struct_in->scale_options & 1U))
			return kIOReturnUnsupported;
		bzero(&scale, sizeof scale);
		scale.buffer.x = 0;
		scale.buffer.y = 0;
		scale.buffer.w = static_cast<int16_t>(struct_in->scale_width);
		scale.buffer.h = static_cast<int16_t>(struct_in->scale_height);
		scale.source.w = scale.buffer.w;
		scale.source.h = scale.buffer.h;
		lockAccel(m_provider);	// Moved
#if 1
		while (1) {
			if (!m_surface_client) {
				unlockAccel(m_provider);
				return kIOReturnUnsupported;
			}
			if (m_surface_client->(byte @0x10CA) == 0)
				break;
			IOLockSleep(/* lock @m_provider+0x960 */, m_surface_client, 1);
		}
#endif
		scale_options = static_cast<eIOAccelSurfaceScaleBits>(((struct_in->scale_options >> 2) & 1U) | (struct_in->scale_options & 2U));
		/*
		 * Note: Intel GMA 950 actually calls set_scaling.  set_scale there is a wrapper
		 *   for set_scaling that validates the struct size and locks the accelerator
		 */
		rc = m_surface_client->set_scale(scale_options, &scale, sizeof scale);
		unlockAccel(m_provider);
		if (rc != kIOReturnSuccess)
			return rc;
	}
	rc = set_surface_volatile_state(struct_in->volatile_state);
	if (rc != kIOReturnSuccess)
		return rc;
	rc = get_config(&struct_out->config[0],
					&struct_out->config[1],
					&struct_out->config[2]);
	if (rc != kIOReturnSuccess)
		return rc;
	rc = get_surface_size(&struct_out->inner_width,
						  &struct_out->inner_height,
						  &struct_out->outer_width,
						  &struct_out->outer_height);
	if (rc != kIOReturnSuccess)
		return rc;
	rc = get_status(&struct_out->status);
	if (rc != kIOReturnSuccess)
		return rc;
	struct_out->surface_mode_bits = static_cast<uint32_t>(m_surface_client->getOriginalModeBits());
#if LOGGING_LEVEL >= 2
	if (m_log_level >= 3) {
		GLLog(3, "%s:   config %#x, %u, %u, inner [%u, %u], outer [%u, %u], status %u, smb %#x\n", __FUNCTION__,
			  struct_out->config[0], struct_out->config[1], struct_out->config[2],
			  struct_out->inner_width, struct_out->inner_height, struct_out->outer_width, struct_out->outer_height,
			  struct_out->status, struct_out->surface_mode_bits);
	}
#endif
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::reclaim_resources()
{
	GLLog(2, "%s()\n", __FUNCTION__);
#if 1
	lockAccel(m_provider);
	dword ptr m_provider->0x628 = 0x10000;
	unlockAccel(m_provider);
#endif
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::get_data_buffer(struct sIOGLContextGetDataBuffer* struct_out, size_t* struct_out_size)
{
	GLLog(2, "%s(struct_out, %lu)\n", __FUNCTION__, *struct_out_size);
	return kIOReturnError;	// Not implemented in Intel GMA 950
}

HIDDEN
IOReturn CLASS::set_stereo(uintptr_t c1, uintptr_t c2)
{
	GLLog(2, "%s(%lu, %lu)\n", __FUNCTION__, c1, c2);
#if 1
	IOReturn rc;

	lockAccel(m_provider);
	dword ptr m_provider->0x148[c2] = c1;
	if (dword ptr m_provider->0xFC[9 * c2] && (c1 & 2U))
		c1 &= 1U;
	rc = m_provider->setupStereo(c2, c1);
	unlockAccel(m_provider);
	return rc;
#endif
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::purge_accelerator(uintptr_t c1)
{
	GLLog(2, "%s(%lu)\n", __FUNCTION__, c1);
#if 1
	return m_provider->purgeAccelerator(c1);
#endif
	return kIOReturnSuccess;
}

#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060
HIDDEN
IOReturn CLASS::get_channel_memory(struct sIODeviceChannelMemoryData* struct_out, size_t* struct_out_size)
{
	GLLog(2, "%s(struct_out, %lu)\n", __FUNCTION__, *struct_out_size);
	bzero(struct_out, *struct_out_size);
	return kIOReturnSuccess;
}
#else
HIDDEN
IOReturn CLASS::submit_command_buffer(uintptr_t do_get_data,
									  struct sIOGLGetCommandBuffer* struct_out,
									  size_t* struct_out_size)
{
	IOReturn rc;
	IOOptionBits options; // var_1c
	IOMemoryDescriptor* md; // var_20
	IOMemoryMap* mm; // var_3c
	struct sIOGLContextGetDataBuffer db;
	size_t dbsize;

#if LOGGING_LEVEL >= 3
	GLLog(3, "%s(%lu, struct_out, %lu)\n", __FUNCTION__, do_get_data, *struct_out_size);
#endif
	if (*struct_out_size < sizeof *struct_out)
		return kIOReturnBadArgument;
	options = 0;
	md = 0;
	*struct_out_size = sizeof *struct_out;
	bzero(struct_out, sizeof *struct_out);
	rc = clientMemoryForType(m_mem_type, &options, &md);
	m_mem_type = 0U;
	if (rc != kIOReturnSuccess)
		return rc;
	mm = md->createMappingInTask(m_owning_task,
								 0,
								 kIOMapAnywhere);
	md->release();
	if (!mm)
		return kIOReturnNoMemory;
	struct_out->addr[0] = mm->getAddress();
	struct_out->len[0] = static_cast<uint32_t>(mm->getLength());
	if (do_get_data != 0) {
		// increment dword @offset 0x8ec on the provider
		dbsize = sizeof db;
		rc = get_data_buffer(&db, &dbsize);
		if (rc != kIOReturnSuccess) {
			mm->release();	// Added
			return rc;
		}
		struct_out->addr[1] = db.addr;
		struct_out->len[1] = db.len;
	}
	// IOLockLock on provider @+0xC4
	m_gc->setObject(mm);
	// IOUnlockLock on provider+0xC4
	mm->release();
	return rc;
}

HIDDEN
IOReturn CLASS::filter_control(struct sIOGLFilterControl const* struct_in,
							   struct sIOGLFilterControl* struct_out,
							   size_t struct_in_size,
							   size_t* struct_out_size)
{
	GLLog(2, "%s(struct_in, struct_out, %lu, %lu)\n", __FUNCTION__, struct_in_size, *struct_out_size);
	return kIOReturnUnsupported;
}
#endif

#pragma mark -
#pragma mark NVGLContext Methods
#pragma mark -

HIDDEN
IOReturn CLASS::get_query_buffer(uintptr_t c1, struct sIOGLGetQueryBuffer* struct_out, size_t* struct_out_size)
{
	GLLog(2, "%s(%lu, struct_out, %lu)\n", __FUNCTION__, c1, *struct_out_size);
	return kIOReturnUnsupported;
}

HIDDEN
IOReturn CLASS::get_notifiers(uint32_t*, uint32_t*)
{
	GLLog(2, "%s(out1, out2)\n", __FUNCTION__);
	return kIOReturnUnsupported;
}

#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060
HIDDEN
IOReturn CLASS::new_heap_object(struct sNVGLNewHeapObjectData const* struct_in,
								struct sIONewTextureReturnData* struct_out,
								size_t struct_in_size,
								size_t* struct_out_size)
{
	GLLog(2, "%s(struct_in, struct_out, %lu, %lu)\n", __FUNCTION__, struct_in_size, *struct_out_size);
	return kIOReturnUnsupported;
}
#endif

HIDDEN
IOReturn CLASS::kernel_printf(char const* str, size_t str_size)
{
	GLLog(2, "%s: %.80s\n", __FUNCTION__, str);
	return kIOReturnUnsupported;
}

HIDDEN
IOReturn CLASS::nv_rm_config_get(uint32_t const* struct_in,
								 uint32_t* struct_out,
								 size_t struct_in_size,
								 size_t* struct_out_size)
{
	GLLog(2, "%s(struct_in, struct_out, %lu, %lu)\n", __FUNCTION__, struct_in_size, *struct_out_size);
	return kIOReturnUnsupported;
}

HIDDEN
IOReturn CLASS::nv_rm_config_get_ex(uint32_t const* struct_in,
									uint32_t* struct_out,
									size_t struct_in_size,
									size_t* struct_out_size)
{
	GLLog(2, "%s(struct_in, struct_out, %lu, %lu)\n", __FUNCTION__, struct_in_size, *struct_out_size);
	return kIOReturnUnsupported;
}

HIDDEN
IOReturn CLASS::nv_client_request(void const* struct_in,
								  void* struct_out,
								  size_t struct_in_size,
								  size_t* struct_out_size)
{
	GLLog(2, "%s(struct_in, struct_out, %lu, %lu)\n", __FUNCTION__, struct_in_size, *struct_out_size);
	return kIOReturnUnsupported;
}

HIDDEN
IOReturn CLASS::pageoff_surface_texture(struct sNVGLContextPageoffSurfaceTextureData const* struct_in, size_t struct_in_size)
{
	GLLog(2, "%s(struct_in, %lu)\n", __FUNCTION__, struct_in_size);
	return kIOReturnUnsupported;
}

HIDDEN
IOReturn CLASS::get_data_buffer_with_offset(struct sIOGLContextGetDataBuffer* struct_out, size_t* struct_out_size)
{
	GLLog(2, "%s(struct_out, %lu)\n", __FUNCTION__, *struct_out_size);
	return kIOReturnUnsupported;
}

HIDDEN
IOReturn CLASS::nv_rm_control(uint32_t const* struct_in,
							  uint32_t* struct_out,
							  size_t struct_in_size,
							  size_t* struct_out_size)
{
	GLLog(2, "%s(struct_in, struct_out, %lu, %lu)\n", __FUNCTION__, struct_in_size, *struct_out_size);
	return kIOReturnUnsupported;
}

HIDDEN
IOReturn CLASS::get_power_state(uint32_t*, uint32_t*)
{
	GLLog(2, "%s(out1, out2)\n", __FUNCTION__);
	return kIOReturnUnsupported;
}

#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
HIDDEN
IOReturn CLASS::set_watchdog_timer(uintptr_t c1)
{
	GLLog(2, "%s(%lu)\n", __FUNCTION__, c1);
	return kIOReturnUnsupported;
}

HIDDEN
IOReturn CLASS::GetHandleIndex(uint32_t*, uint32_t*)
{
	GLLog(2, "%s(out1, out2)\n", __FUNCTION__);
	return kIOReturnUnsupported;
}

HIDDEN
IOReturn CLASS::ForceTextureLargePages(uintptr_t c1)
{
	GLLog(2, "%s(%lu)\n", __FUNCTION__, c1);
	return kIOReturnUnsupported;
}
#endif
