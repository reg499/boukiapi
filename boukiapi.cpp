#include <node.h>
#include <nan.h>
#include <Windows.h>
#include <dpapi.h>
#include <functional>
void UnprotectDataHelper(Nan::NAN_METHOD_ARGS_TYPE info) {
    v8::Isolate* isolate = info.GetIsolate();
    DATA_BLOB encryptedData;
    DATA_BLOB optionalEntropy;
    DATA_BLOB decryptedData;
    auto encryptedBuffer = node::Buffer::Data(info[0]);
    auto encryptedBufferLength = node::Buffer::Length(info[0]);
    optionalEntropy.pbData = nullptr;
    if (!info[1]->IsNull()) {
        optionalEntropy.pbData = reinterpret_cast<BYTE*>(node::Buffer::Data(info[1]));
        optionalEntropy.cbData = node::Buffer::Length(info[1]);
    } else {
        optionalEntropy.cbData = 0;
    }
    encryptedData.pbData = reinterpret_cast<BYTE*>(encryptedBuffer);
    encryptedData.cbData = encryptedBufferLength;
    bool success = CryptUnprotectData(
        &encryptedData,
        nullptr,
        optionalEntropy.pbData ? &optionalEntropy : nullptr,
        nullptr,
        nullptr,
        0,
        &decryptedData
    );
    if (success) {
        auto returnBuffer = Nan::CopyBuffer(reinterpret_cast<const char*>(decryptedData.pbData), decryptedData.cbData).ToLocalChecked();
        LocalFree(decryptedData.pbData);
        info.GetReturnValue().Set(returnBuffer);
    } else {
        info.GetReturnValue().Set(Nan::Null());
    }
}
NAN_METHOD(UnprotectData) {
    UnprotectDataHelper(info);
}
NAN_MODULE_INIT(Initialize) {
    Nan::Set(
        target,
        Nan::New<v8::String>("unprotectData").ToLocalChecked(),
        Nan::GetFunction(Nan::New<v8::FunctionTemplate>(UnprotectData)).ToLocalChecked()
    );
}
NODE_MODULE(binding, Initialize)
