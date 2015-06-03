#include <node.h>

using namespace v8;

void Method(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, "world"));
}

//Arg1: This pointer to Gamefield class object
void startUpdater(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  if (args.Length() < 1 || !args[0]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }
  
  Local<Object> obj = args[0]->ToObject();
  obj->Set(String::NewFromUtf8(isolate, "msg"), args[1]->ToString());
  obj->Set("msg2", args[1]->ToString());

  args.GetReturnValue().Set(obj);
}

void init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "hello", Method);
  NODE_SET_METHOD(exports, "startUpdater", startUpdater);
}

NODE_MODULE(GamefieldUpdater, init)
