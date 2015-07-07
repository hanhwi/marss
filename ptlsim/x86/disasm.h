#ifndef _DISASM_H
#define _DISASM_H

#include <string>

typedef unsigned long long W64;

struct disassemble_info;

class Disasm{
    static const int BUFSIZE = 1000;
    unsigned char buf[BUFSIZE];
    disassemble_info *disasm_info;
public:
    Disasm();
    ~Disasm();
    void init(W64 base_rip, unsigned char *buf_addr, int buf_size);
    std::string disasm(W64 rip);
};
#endif
