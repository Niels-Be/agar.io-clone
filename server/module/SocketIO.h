//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_SOCKETIO_H
#define AGARIO_SOCKETIO_H

#include "GlobalDefs.h"

class SocketIO {
private:
	v8::Local<v8::Object> mSocket;
	uint64_t mId;

public:
	SocketIO(const v8::Local<v8::Object>& mSocket);

	void emit(String name) {
		emit(name, list<v8::Local<v8::Value> >());
	}

	void emit(String name, v8::Local<v8::Value> val) {
		emit(name, list<v8::Local<v8::Value> > {val});
	}

	void emit(String name, const list<v8::Local<v8::Value> >& val);

	void on(String name, function<void(SocketIO*, const v8::FunctionCallbackInfo<v8::Value>&)>&& cb);

	uint64_t getId() const;
};


#endif //AGARIO_SOCKETIO_H

