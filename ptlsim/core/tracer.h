#ifndef TRACER_H
#define TRACER_H
#include <globals.h>
#include <superstl.h>
#include <ptlsim.h>
#include <ptlhwdef.h>

#include <string>
#include <iostream>
#include <fstream>

extern const OpcodeInfo opinfo[OP_MAX_OPCODE];


// Control global parameter
// Need to be improved
// bool traceenable
// bool config.trace_print_uop
// bool config.trace_print_mop

class InstTracer;

class TraceEntry: public selfqueuelink{
    friend class InstTracer;
    W64 rip;
    std::string macroop;
    bool som, eom;
    bool valid;
    int opcode;
public:
    void init(W64 rip, std::string macroop, bool som, bool eom, bool valid, int opcode){
        this->rip = rip;
        this->macroop = macroop;
        this->som = som;
        this->eom = eom;
        this->valid = valid;
        this->opcode = opcode;
    }

    virtual void dump_mop(ostream &os){
        os << (valid ? "" : "!");
        os << "0x" << std::hex << rip << ": " << macroop << '\n';
        os << std::dec;
    }
    virtual void dump_uop(ostream &os){
        static const char *indent = config.trace_print_mop ? "    " : "";
        os << indent << (valid ? "" : "!") << opinfo[opcode].name << '\n';
    }

    void dump(ostream &os){
        if (config.trace_print_mop && som)
            dump_mop(os);

        if (config.trace_print_uop)
            dump_uop(os);
    }
};

class InstTracer{
    std::string name;
    ofstream of;
    TraceEntry entry_head;
    ostream *log;
public:
    InstTracer(){
        this->log = NULL;
        // It is for checking initialization step;
    }

    void init(std::string name, char *filename)
    {
        entry_head.reset();
        this->of.open(filename);
        this->name = name;
        this->log = &of;
    }

    void init(char * name, char *filename)
    {
        entry_head.reset();
        this->of.open(filename);
        this->name = std::string((const char *)name);
        this->log = &of;
    }

    void init(std::string name, ostream *os)
    {
        entry_head.reset();
        this->name = name;
        this->log = os;
    }

    void init(char * name, ostream *os)
    {
        entry_head.reset();
        this->name = std::string((const char *)name);
        this->log = os;
    }

    // Allocate new trace entry at the end of entry queue
    // Caller has to initialize the entry.
    TraceEntry *alloc_new_entry(void);
    void invalidate_from(TraceEntry *ent);
    void print_until(TraceEntry *ent, bool deleted=true, bool inclusive=true);
};

#endif
