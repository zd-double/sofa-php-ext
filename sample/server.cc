// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#include <signal.h>
#include <unistd.h>
#include <sofa/pbrpc/pbrpc.h>
#include "echo_service.pb.h"

class EchoServerImpl : public sofa::pbrpc::test::EchoServer
{
public:
    EchoServerImpl() {}
    virtual ~EchoServerImpl() {}

private:
    virtual void TestEcho(google::protobuf::RpcController* controller,
                      const sofa::pbrpc::test::EchoRequest* request,
                      sofa::pbrpc::test::EchoResponse* response,
                      google::protobuf::Closure* done)
    {
        sofa::pbrpc::RpcController* cntl = static_cast<sofa::pbrpc::RpcController*>(controller);
        SLOG(INFO, "Echo(): request message from %s: %s",
                cntl->RemoteAddress().c_str(), request->message().c_str());
        if (cntl->IsHttp()) {
            SLOG(INFO, "HTTP-PATH=\"%s\"", cntl->HttpPath().c_str());
            std::map<std::string, std::string>::const_iterator it;
            const std::map<std::string, std::string>& query_params = cntl->HttpQueryParams();
            for (it = query_params.begin(); it != query_params.end(); ++it) {
                SLOG(INFO, "QueryParam[\"%s\"]=\"%s\"", it->first.c_str(), it->second.c_str());
            }
            const std::map<std::string, std::string>& headers = cntl->HttpHeaders();
            for (it = headers.begin(); it != headers.end(); ++it) {
                SLOG(INFO, "Header[\"%s\"]=\"%s\"", it->first.c_str(), it->second.c_str());
            }
        }
        response->set_message("echo message: " + request->message());
        done->Run();
    }
};

bool thread_init_func()
{
    sleep(1);
    SLOG(INFO, "Init work thread succeed");
    return true;
}

void thread_dest_func()
{
    SLOG(INFO, "Destroy work thread succeed");
}

int main()
{
    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    // Define an rpc server.
    sofa::pbrpc::RpcServerOptions options;
    options.work_thread_init_func = sofa::pbrpc::NewPermanentExtClosure(&thread_init_func);
    options.work_thread_dest_func = sofa::pbrpc::NewPermanentExtClosure(&thread_dest_func);
    sofa::pbrpc::RpcServer rpc_server(options);

    // Start rpc server.
    if (!rpc_server.Start("0.0.0.0:12321")) {
        SLOG(ERROR, "start server failed");
        return EXIT_FAILURE;
    }
    
    // Register service.
    sofa::pbrpc::test::EchoServer* echo_service = new EchoServerImpl();
    if (!rpc_server.RegisterService(echo_service)) {
        SLOG(ERROR, "export service failed");
        return EXIT_FAILURE;
    }

    // Wait signal.
    rpc_server.Run();

    // Stop rpc server.
    rpc_server.Stop();

    // Delete closures.
    // Attention: should delete the closures after server stopped, or may be crash.
    delete options.work_thread_init_func;
    delete options.work_thread_dest_func;

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
