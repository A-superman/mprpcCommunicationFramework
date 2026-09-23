#pragma once
#include <google/protobuf/service.h>
#include <string>

class MprpcController: public google::protobuf::RpcController
{
public:
    MprpcController();
    // RpcController中的虚函数 7个
    void Reset();                               // 重置状态
    bool Failed() const;                        // 一个rpc调用是否失败
    std::string ErrorText() const;              // 调用失败时会返回错误信息
    void SetFailed(const std::string& reason);  // 保存失败原因
    // 目前未实现具体的功能
    void StartCancel(); // 取消调用
    bool IsCanceled() const; // 服务是否被取消
    void NotifyOnCancel(google::protobuf::Closure* callback); // 当rpc被取消，通知回调函数执行
private:
    bool m_failed; // rpc方法执行过程中的状态
    std::string m_errText; // RPC方法执行过程中的错误信息
};