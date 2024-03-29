#!/usr/bin/env python3
#
#
#    CEsvga2 - ChrisEric1 Super Video Graphics Array 2
#    Copyright (C) 2023-2024, Christopher Eric Lentocha
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as published
#    by the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.
#
#    You should have received a copy of the GNU Affero General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
#

import os
import sys
import tempfile
import subprocess
import macholib.MachO

from io import BytesIO
from pathlib import Path
from sys import argv

from macholib.mach_o import (
    LC_VERSION_MIN_MACOSX,
    LC_DATA_IN_CODE,
)

#
# Binary specified in arguments.
#
input_path = Path(argv[1].strip())

#
# Check if we even have a 32-bit binary or 32-bit slice.
#
result = subprocess.run(["lipo", input_path, "-info"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
if "i386" not in result.stdout:
    print("Binary is not 32-bit, nothing to do")
    sys.exit(0)

#
# Extract i386 slice from FAT binary if needed.
#
print("Processing {}".format(input_path))
if "Non-fat" not in result.stdout:
    print("Binary is fat, extracting 32-bit slice")
    is_fat_binary = True
    slice_temp_file, slice_path = tempfile.mkstemp()
    slice_path = Path(slice_path)
    os.close(slice_temp_file)
    subprocess.run(["lipo", input_path, "-thin", "i386", "-output", slice_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

else:
    print("Binary is pure 32-bit")
    is_fat_binary = False
    slice_path = input_path

#
# Read Mach-O binary.
#
original_file = BytesIO(slice_path.read_bytes())
machFile = macholib.MachO.MachO(slice_path)

#
# Strip LC_VERSION_MIN_MACOSX command.
#
for cmd in machFile.headers[0].commands:
    if (cmd[0].cmd == LC_VERSION_MIN_MACOSX):
        machFile.headers[0].changedHeaderSizeBy(-cmd[0].cmdsize)
        machFile.headers[0].commands.remove(cmd)
        machFile.headers[0].header.ncmds -= 1
        print("Removed LC_VERSION_MIN_MACOSX")

#
# Strip LC_DATA_IN_CODE command if zero.
#
for cmd in machFile.headers[0].commands:
    if (cmd[0].cmd == LC_DATA_IN_CODE):
        if (cmd[1].datasize == 0):
            machFile.headers[0].changedHeaderSizeBy(-cmd[0].cmdsize)
            machFile.headers[0].commands.remove(cmd)
            machFile.headers[0].header.ncmds -= 1
            print("Removed LC_DATA_IN_CODE")
        else:
            print("LC_DATA_IN_CODE data size is non-zero!")
            if (is_fat_binary):
                slice_path.unlink()
            sys.exit(-1)

#
# Align symbol table offset.
#
symCmd = machFile.headers[0].getSymbolTableCommand()

oldSymOff = symCmd.symoff
newSymOff = (oldSymOff + 3) & ~(3)
print("Old symbol table offset at {}, new symbol table will be at {}".format(oldSymOff, newSymOff))
symOffDelta = newSymOff - oldSymOff

#
# Align string table offset.
#
oldStrOff = symCmd.stroff
newStrOff = ((oldStrOff + symOffDelta) + 3) & ~(3)
print("Old string table offset at {}, new string table will be at {}".format(oldStrOff, newStrOff))
strOffDelta = newStrOff - (oldStrOff + symOffDelta)

#
# Write new offsets to symbol table command.
#
symCmd.symoff = newSymOff
symCmd.stroff = newStrOff

#
# Write Mach-O header to new file.
#
with open(slice_path, "wb") as new_file:
    machFile.headers[0].write(new_file)
    original_file.seek(new_file.tell())

    #
    # Copy rest of file and pad accordingly.
    #
    if oldSymOff > oldStrOff:
        new_file.write(original_file.read(oldStrOff - original_file.tell()))
        new_file.write(b"\0" * strOffDelta)
        new_file.write(original_file.read(oldSymOff - original_file.tell()))
        new_file.write(b"\0" * symOffDelta)
        new_file.write(original_file.read())
    else:
        new_file.write(original_file.read(oldSymOff - original_file.tell()))
        new_file.write(b"\0" * symOffDelta)
        new_file.write(original_file.read(oldStrOff - original_file.tell()))
        new_file.write(b"\0" * strOffDelta)
        new_file.write(original_file.read())

print("Wrote new binary to {}".format(slice_path))
if is_fat_binary:
    print("Replacing 32-bit slice in fat binary")
    result = subprocess.run(["lipo", input_path, "-replace", "i386", slice_path, "-output", input_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    slice_path.unlink()
