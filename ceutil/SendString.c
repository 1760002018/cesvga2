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

#include "VLog.h"

#define BDOOR_MAGIC 0x564D5868U
#define BDOOR_PORT 0x5658U
#define BDOORHB_PORT 0x5659U
#define PROTO 0x49435052U
#define BDOOR_CMD_GETSCREENSIZE 15U
#define BDOOR_CMD_MESSAGE 30U
#define BDOORHB_CMD_MESSAGE 0U

#define BACKDOOR_VARS() \
	unsigned eax = 0U, edx = 0U, edi = 0U; \
	unsigned long ebx = 0U, ecx = 0U, esi = 0U;

#define BACKDOOR_ASM(op, port) \
	{ \
		eax = BDOOR_MAGIC; \
		edx = (edx & 0xFFFF0000U) | port; \
		__asm__ volatile (op : "+a" (eax), "+b" (ebx), \
			"+c" (ecx), "+d" (edx), "+S" (esi), "+D" (edi)); \
	}

#define BACKDOOR_ASM_IN()       BACKDOOR_ASM("in %%dx, %0", BDOOR_PORT)
#define BACKDOOR_ASM_HB_OUT()   BACKDOOR_ASM("cld; rep outsb", BDOORHB_PORT)
#define BACKDOOR_ASM_HB_IN()    BACKDOOR_ASM("cld; rep insb", BDOORHB_PORT)

__attribute__((visibility("hidden")))
char CELog_SendString(char const* str)
{
	unsigned long size;
	unsigned short channel_id;

	if (!str)
		return 0;

	BACKDOOR_VARS()

	ecx = BDOOR_CMD_MESSAGE | 0x00000000U;  /* Open */
	ebx = PROTO;
	BACKDOOR_ASM_IN()

	if (!(ecx & 0x00010000U)) {
		return 0;
	}

	channel_id = edx >> 16;
	for (size = 0U; str[size]; ++size);

	ecx = BDOOR_CMD_MESSAGE | 0x00010000U;  /* Send size */
	ebx = size;
	edx = channel_id << 16;
	esi = edi = 0U;
	BACKDOOR_ASM_IN()

	/* We only support the high-bandwidth backdoor port. */
	if (((ecx >> 16) & 0x0081U) != 0x0081U) {
		return 0;
	}

	ebx = 0x00010000U | BDOORHB_CMD_MESSAGE;
	ecx = size;
	edx = channel_id << 16;
	esi = (unsigned long) str;
	edi = 0U;
	BACKDOOR_ASM_HB_OUT()

	/* Success? */
	if (!(ebx & 0x00010000U)) {
		return 0;
	}

	ecx = BDOOR_CMD_MESSAGE | 0x00060000U;  /* Close */
	ebx = 0U;
	edx = channel_id << 16;
	esi = edi = 0U;
	BACKDOOR_ASM_IN()

	return 1;
}

__attribute__((visibility("hidden")))
void CEGetScreenSize(unsigned short* width, unsigned short* height)
{
	BACKDOOR_VARS()
	ecx = BDOOR_CMD_GETSCREENSIZE;
	BACKDOOR_ASM_IN()
	*width  = (unsigned short) (eax >> (8U * sizeof(unsigned short)));
	*height = (unsigned short) (eax & ((1U << (8U * sizeof(unsigned short))) - 1U));
}
