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

#ifndef __CESVGA2SHARED_H__
#define __CESVGA2SHARED_H__

#include <IOKit/IOLib.h>

struct GLDSysObject;
struct CEsvga2TextureBuffer;

class CEsvga2Shared : public OSObject
{
	OSDeclareDefaultStructors(CEsvga2Shared);

private:
	task_t m_owning_task;				// offset  0x8
	class CEsvga2Accel* m_provider;		// offset  0xC
	void** m_handle_table;				// offset 0x10
	uint32_t m_num_handles;				// offset 0x14
#if 1
	uint8_t* m_bitmap;					// offset 0x18, attached to m_handle_table
#endif
										// offset 0x1C - unknown
										// offset 0x20, [linked list of structs describing sys object pools]
	CEsvga2TextureBuffer* m_texture_list;	// offset 0x24, linked listed of textures
										// ends   0x28
	class IOMemoryMap* m_client_sys_objs_map;
	void* m_client_sys_objs_kernel_ptr;
	IOLock* m_shared_lock;
	int m_log_level;

	void Cleanup();
	bool alloc_handles();
	void free_handles();
	bool alloc_buf_handle(void* entry, uint32_t* object_id);
	void free_buf_handle(void* entry, uint32_t object_id);
	static void* allocVendorTextureBuffer(size_t bytes) { return IOMalloc(bytes); }	// calls function with similar name on provider
	static void releaseVendorTextureBuffer(void* p, size_t bytes) { IOFree(p, bytes); } // calls function with similar name on provider
	void link_texture_at_head(CEsvga2TextureBuffer*);
	void unlink_nonhead_texture(CEsvga2TextureBuffer*);
	void unlink_texture(CEsvga2TextureBuffer*);
	IOReturn alloc_client_shared(GLDSysObject**, mach_vm_address_t*);
	void free_client_shared(GLDSysObject*);
#if 1
	GLDSysObject* find_client_shared(uint32_t object_id);
	void deref_client_shared(uint32_t object_id, int addend = -0x10000);
#endif
	static void delete_texture_internal(class CEsvga2Accel*,
										CEsvga2Shared*,
										CEsvga2TextureBuffer*);
	static void free_orphan_texture(class CEsvga2Accel*, struct IOTextureBuffer*);
	static void finalize_texture(class CEsvga2Accel*, CEsvga2TextureBuffer*);
	CEsvga2TextureBuffer* new_agp_texture(mach_vm_address_t pixels,
										  size_t texture_size,
										  uint32_t read_only,
										  mach_vm_address_t* sys_obj_client_addr);
	CEsvga2TextureBuffer* common_texture_init(uint8_t object_type);

public:
	/*
	 * Methods overridden from superclass
	 */
	bool init(task_t owningTask, class CEsvga2Accel* provider, int logLevel);
	void free();
	static CEsvga2Shared* factory(task_t owningTask, class CEsvga2Accel* provider, int logLevel);

	CEsvga2TextureBuffer* findTextureBuffer(uint32_t object_id);
	bool initializeTexture(CEsvga2TextureBuffer*, struct VendorNewTextureDataStruc const*);
	CEsvga2TextureBuffer* new_surface_texture(uint32_t surface_id,
											  uint32_t fb_idx_mask,
											  mach_vm_address_t* sys_obj_client_addr);
	CEsvga2TextureBuffer* new_iosurface_texture(uint32_t, uint32_t, uint32_t, uint32_t, mach_vm_address_t*);
	CEsvga2TextureBuffer* new_texture(uint32_t size0,
									  uint32_t size1,
									  mach_vm_address_t pixels,
									  size_t texture_size,
									  uint32_t read_only,
									  mach_vm_address_t* client_texture_buffer_addr,
									  mach_vm_address_t* sys_obj_client_addr);
	CEsvga2TextureBuffer* new_agpref_texture(mach_vm_address_t pixels1,
											 mach_vm_address_t pixels2,
											 size_t texture_size,
											 uint32_t read_only,
											 mach_vm_address_t* sys_obj_client_addr);
	IOReturn pageoffDirtyTexture(CEsvga2TextureBuffer*);
	void delete_texture(CEsvga2TextureBuffer* texture) { delete_texture_internal(m_provider, this, texture); }
	void lockShared() { IOLockLock(m_shared_lock); }
	void unlockShared() { IOLockUnlock(m_shared_lock); }
};

#endif /* __CESVGA2SHARED_H__ */
