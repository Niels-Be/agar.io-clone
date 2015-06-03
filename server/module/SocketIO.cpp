//
// Created by niels on 02.06.15.
//

#include "SocketIO.h"


SocketIO::SocketIO(const v8::Local<v8::Object>& mSocket) : mSocket(mSocket) {

}

void SocketIO::emit(String name, const list<v8::Local<v8::Value> >& val) {

}

void SocketIO::on(String name, function<void(SocketIO*, const v8::FunctionCallbackInfo<v8::Value>&)>&& cb) {

}

uint64_t SocketIO::getId() const {
	return 0;
}
