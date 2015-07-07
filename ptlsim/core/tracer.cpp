#include <tracer.h>

TraceEntry *InstTracer::alloc_new_entry(void){
    if (log && traceenable){
        TraceEntry *new_ent = new TraceEntry;
        new_ent->reset();
        new_ent->addtail(entry_head);
        return new_ent;
    }
    else
        return NULL;
}

void InstTracer::invalidate_from(TraceEntry *ent){
    if (ent){
        selfqueuelink *from = ent;
        while (from != &entry_head){
            ((TraceEntry *) from)->valid = false;
            from = from->next;
        }
        return;
    }
    return;
}

void InstTracer::print_until(TraceEntry *ent, bool deleted, bool inclusive){
    if (ent){
        TraceEntry *cur = (TraceEntry *) this->entry_head.next;

        while (cur != ent){
            cur->dump(*log);
            cur = (TraceEntry *)cur->next;
        }
    
        if (inclusive)
            ent->dump(*log);

        if (deleted){
            cur = (TraceEntry *) this->entry_head.next;
            while (cur != ent){
                TraceEntry *next = (TraceEntry *) cur->next;
                cur->unlink();
                delete cur;
                cur = next;
            }
            if (inclusive){
                ent->unlink();
                delete ent;
            }
        }
        return;
    }
    return;
}
