#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include "nt.h"

typedef struct _INFO {
    UINT64 process_id;
    UINT64 shared_page;
    PEPROCESS process;
}INFO, * PINFO;

extern INFO global_info;

enum operation_t {
    read_op = 0xFB,
    write_op = 0xFC,
    module_base_op = 0xFD,
    kread_op = 0xFE
};

enum success_op_t {
    success_read = read_op >> 1,
    success_write = write_op >> 1,
    success_module = module_base_op >> 1,
    success_kread = kread_op >> 1
};

enum failure_op_t {
    fail_read = read_op << 1,
    fail_write = write_op << 1,
    fail_module = module_base_op << 1,
    fail_kread = kread_op << 1
};

enum operation_status_t {
    success = 1,
    failure
};


typedef struct _REQUEST {
    operation_t id;
    PVOID address; // when requesting process base, this is a pointer to a const char*
    PVOID user_buffer;
    ULONG size;
    UINT64 value; // for write operation // also contains requested module base address (if requested)
    char module_name[64];
}REQUEST, * PREQUEST;

typedef struct _NEWREQUEST {
    operation_t id;
    operation_status_t status;
    PVOID Data; // data should be malloced
} NEWREQUEST, *PNEWREQUEST;

void communicate(PREQUEST shared_page);

#endif