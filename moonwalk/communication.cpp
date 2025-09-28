#include "communication.hpp"

void request_module_base(const char module_str[64], PPROCESS_INFO information) { // process is statically set inside of driver
    volatile PREQUEST req = (PREQUEST)globals::shared_page;
    req->id = (operation_t)IN_PROGRESS;
    req->value = 0;
    strcpy_s(req->module_name, module_str);
    req->id = module_base_op;
    while (req->id == module_base_op) {};
    if (req->id == fail_module) {
        return;
    }
    information->process_base = (UINT64)req->address;
    information->module_base = req->value;

}

NTSTATUS kread(UINT64 kernel_address, ULONG size, void* user_buffer) {

    volatile PREQUEST req = (PREQUEST)globals::shared_page; // must be aligned

    // add functionality to find free request slot instead of just using the first slot


    req->address = (PVOID)kernel_address;
    req->size = size;
    req->id = kread_op;
    req->user_buffer = user_buffer;
    while (req->id == kread_op) {

    } 		// hang until we get a response, FIXME: add FAILURE response.
    if (req->id == fail_kread) {
        return 0xC0000001; // STATUS_UNSUCCESSFUL
    }
    // clean buffer
    memset((PVOID)((UINT64)globals::shared_page), 0, sizeof(REQUEST)); // just clear max instead of sizeof)(T), compiler optimizes to 1 move instruction anyways
    return 0;
}