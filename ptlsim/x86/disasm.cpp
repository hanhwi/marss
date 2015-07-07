#include <disasm.h>
#include <string>
#include <map>
#include <cstdio>
extern "C"{
#include <dis-asm.h>
}

Disasm::Disasm()
    :disasm_info(new disassemble_info)
{
    FILE *out = fmemopen(buf, BUFSIZE, "w");
    INIT_DISASSEMBLE_INFO(*disasm_info, out, fprintf);
}

Disasm::~Disasm()
{
    fclose(disasm_info->stream);
    delete disasm_info;
}

void Disasm::init(W64 base_rip, unsigned char *buf_addr, int buf_size)
{
    // Reset out file pointer
    rewind(disasm_info->stream);
    disasm_info->buffer = buf_addr;
    disasm_info->buffer_vma = base_rip;
    disasm_info->buffer_length = buf_size;
    disasm_info->endian = BFD_ENDIAN_LITTLE;
    disasm_info->mach = bfd_mach_x86_64;
}

std::string Disasm::disasm(W64 rip)
{
    print_insn_i386(rip, disasm_info);
    fputc('\0', disasm_info->stream);
    fflush(disasm_info->stream);
    rewind(disasm_info->stream);
    
    return std::string((const char *)buf);
}

