// Copyright 2013 Mirus Project

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <cpu/gdt.hpp>

// methods and variables used in the asm file
namespace mirus
{
    extern "C"
    {
        void gdt_flush();
        mirus::gdt_ptr gp;
    }
}

static mirus::gdt_entry _gdt[6];

void mirus::gdt::set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
    using namespace mirus;

    // descriptor base address
    _gdt[num].base_low = (base & 0xFFFF);
    _gdt[num].base_middle = (base >> 16) & 0xFF;
    _gdt[num].base_high = (base >> 24) & 0xFF;

    // limits
    _gdt[num].limit_low = (limit & 0xFFFF);
    _gdt[num].granularity = ((limit >> 16) & 0x0F);

    // flags + granularity
    _gdt[num].granularity |= (gran & 0xF0);
    _gdt[num].access = access;
}

void mirus::gdt::install()
{
    using namespace mirus;

#ifdef _DEBUG_ON
    mirus::debugger::write("Installing GDT\n");
#endif

    gp.limit = (sizeof(gdt_entry) * 6) - 1;
    gp.base = (unsigned long)&_gdt;

    // Null descriptor
    gdt::set_gate(0, 0, 0, 0, 0);

    // code seg
    gdt::set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // data seg
    gdt::set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // user code
    gdt::set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // user data
    gdt::set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    // flush it
    gdt_flush();
}
