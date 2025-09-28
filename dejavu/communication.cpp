#include "communication.h"
#include "routines.h"

INFO global_info = {};

void communicate(PREQUEST req) {
	if (!req)
		return;
	while (true) {

		switch (req->id) {

		case operation_t::read_op: {
			// not implemented
			break;
		}
		case operation_t::write_op: {
			// not implemented
			break;
		}
		case operation_t::module_base_op: {
			// not implemented
			break;
		}
		case operation_t::kread_op: {
			if (!NT_SUCCESS(kread((UINT64)req->address, req->size, req->user_buffer))) {
				req->id = (operation_t)failure_op_t::fail_kread;
			}
			req->id = (operation_t)0; // finished
			break;
		}
		
		}



	}
}