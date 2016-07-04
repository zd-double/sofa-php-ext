// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhangdi Di)

#ifndef EXT_RPC_SERVICE_STUB_IMPL_H
#define EXT_RPC_SERVICE_STUB_IMPL_H

extern "C" {
#include <php.h>
#include <ext/standard/php_string.h>
#include <ext/standard/php_var.h>
#include <Zend/zend_exceptions.h>
}

#include <set>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>

#ifdef add_method
#undef add_method
#include <google/protobuf/descriptor.pb.h>
#endif

#include <google/protobuf/dynamic_message.h>

#include <sofa/pbrpc/pbrpc.h>
#include <sofa/pbrpc/rpc_channel.h>
#include <sofa/pbrpc/rpc_controller.h>

namespace sofa_php_ext
{

const static std::string FILE_PROTO = "file_proto";

struct MethodWrapper 
{
    MethodWrapper() : _method(NULL),
                      _request(NULL),
                      _response(NULL) 
    { }

    ~MethodWrapper() 
    { }

    const google::protobuf::MethodDescriptor* _method;
    const google::protobuf::Descriptor* _request;
    const google::protobuf::Descriptor* _response;
};

typedef std::map<std::string, MethodWrapper*> MethodBoard;
typedef std::set<std::string> MessageSet;

enum PbType 
{
    PB_TYPE_DOUBLE = 1,
    PB_TYPE_FIXED32,
    PB_TYPE_FIXED64,
    PB_TYPE_FLOAT,
    PB_TYPE_INT,
    PB_TYPE_SIGNED_INT,
    PB_TYPE_STRING,
    PB_TYPE_BOOL,
    PB_TYPE_UINT64
};

class PhpRpcServiceStubImpl 
{
public:
    PhpRpcServiceStubImpl(sofa::pbrpc::RpcChannel* channel,
                        sofa::pbrpc::RpcController* controller,
                       const std::string& package_name,
                       const std::string& service_name) : 
                       _timeout(100),
                       _package_name(package_name),
                       _service_name(service_name),
                       _method_board(NULL),
                       _pool(NULL), 
                       _file_proto(NULL),
                       _service_proto(NULL),
                       _file_descriptor(NULL)
    {
        if (channel == NULL || controller == NULL) 
        {
            SLOG(FATAL, "create service stub failed");
            return;
        }
        _channel = channel;
        _cntl = controller;
        _method_board = new MethodBoard();
        _message_set = new MessageSet();
        _pool = new google::protobuf::DescriptorPool();
        _file_proto = new google::protobuf::FileDescriptorProto();
        _file_proto->set_name(FILE_PROTO);
        _file_proto->set_package(_package_name);
        _service_proto = _file_proto->add_service();
        _service_proto->set_name(_service_name);
    }

    ~PhpRpcServiceStubImpl() 
    {
        MethodBoard::iterator it = _method_board->begin();
        MethodBoard::iterator end = _method_board->end();
        for (; it != end; ++it)
        {
            delete it->second;
            it->second = NULL;
        }
        delete _method_board;
        _method_board = NULL;
        delete _message_set;
        _message_set = NULL;
        delete _pool;
        _pool = NULL;
        delete _file_proto;
        _file_proto = NULL;
        delete _channel;
        _channel = NULL;
        delete _cntl;
        _cntl = NULL;
    }

    void SetTimeout(long timeout);

    bool Failed();

    std::string ErrorText();

    void RegisterMethod(const std::string& method_name,
                        zval* request,
                        zval* response);

    void InitMethods();

    void CallMethod(const std::string& method_name,
                    zval* request, 
                    zval* response, 
                    zval* closure);

private:
    zval* GetFieldDescriptors(zval* obj);

    zval** GetFieldDescriptor(zval* obj, uint32_t field_number);

    zval** GetFieldType(zval** field_descriptor, uint32_t field_number);

    zval** GetRequiredLabel(zval** field_descriptor, uint32_t field_number);

    zval** GetRepeatedLabel(zval** field_descriptor, uint32_t field_number);

    const char* GetFieldName(zval** field_descriptor);

    zval** GetUserValues(zval* obj);

    void GetMessageType(zval* obj, 
                        std::string& message_type);

    zval* GetSubMessage(zval* obj,
                       ulong field_number);

    int GetSofaFieldType(zval** type, 
                         google::protobuf::FieldDescriptorProto::Type& pb_type); 

    int ToSofaField(zval** type, 
                    const bool repeated,
                    zval** value, 
                    google::protobuf::Message* sofa_msg, 
                    const google::protobuf::FieldDescriptor* field, 
                    const google::protobuf::Reflection* reflection);

    int PhpTransformToSofa(zval* user_msg,
                        google::protobuf::Message* sofa_msg);

    int ToUserField(zval* value, 
                    bool repeated,
                    int array_size,
                    google::protobuf::FieldDescriptor::Type sofa_type, 
                    const google::protobuf::Message* sofa_msg, 
                    const google::protobuf::FieldDescriptor* field, 
                    const google::protobuf::Reflection* reflection); 

    int CreateMessage(zval* message,
                      const std::string& message_type, 
                      google::protobuf::FileDescriptorProto* file_proto);

    int CreateMessageDescriptor(zval* message,
                                zval* message_descriptor,  
                                google::protobuf::DescriptorProto* message_proto,
                                google::protobuf::FileDescriptorProto* file_proto); 


    int SofaTransformToPhp(zval* user_msg, 
                            const google::protobuf::Message* sofa_msg);

private:
    long _timeout;
    std::string _package_name;
    std::string _service_name;
    MethodBoard* _method_board;
    MessageSet* _message_set;
    
    sofa::pbrpc::RpcChannel* _channel;
    sofa::pbrpc::RpcController* _cntl;
    google::protobuf::DescriptorPool* _pool;
    google::protobuf::FileDescriptorProto* _file_proto;
    google::protobuf::ServiceDescriptorProto* _service_proto;
    const google::protobuf::FileDescriptor* _file_descriptor;
};

}

#endif  // EXT_RPC_SERVICE_STUB_IMPL_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
