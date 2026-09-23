安装zookeeper protobuf
需要开启zookeeper               zkServer.sh start          

# mprpc 项目框架图

> 如果你在 VS Code 里直接打开的是源代码文件，看到的是文本代码块；要看带箭头的图，请用“Markdown: Open Preview”或右键打开预览。
```mermaid
flowchart TB
    %% 客户端
    subgraph Client["客户端（caller）"]
        C1["业务代码\n调用方"]
        C2["生成的 Stub\nUserService_Stub"]
        C3["MprpcChannel\nCallMethod()"]
        C4["MprpcController\n控制请求状态"]
        C5["Muduo TCP Client\n发送请求"]
    end

    %% 服务发现
    subgraph Registry["服务注册与发现"]
        ZK["ZooKeeper\n保存服务地址"]
        ZKUtil["ZkClient\n创建/读取节点"]
    end

    %% 服务端
    subgraph Server["服务端（callee）"]
        S1["业务服务实现\nUserService/Login"]
        S2["RpcProvider\nNotifyService() / Run()"]
        S3["ServiceMap + MethodMap"]
        S4["Muduo TcpServer\n接收连接和请求"]
        S5["Protobuf请求解析"]
        S6["反射调用服务方法"]
        S7["序列化响应并回发"]
    end

    %% 基础设施
    subgraph Base["基础设施层"]
        APP["MprpcApplication\n初始化配置"]
        CFG["MprpcConfig\n读取配置文件"]
        HDR["RpcHeader Proto\n协议头：service_name + method_name + args_size"]
        PROTO["Proto 定义\n请求/响应消息"]
    end

    %% 1. 客户端调用
    C1 --> C2
    C2 --> C3
    C3 --> C4
    C3 --> HDR
    C3 --> C5

    %% 2. 服务发现
    ZKUtil --> ZK
    ZK -->|返回 ip:port| C5
    ZK -->|服务注册| S2

    %% 3. 服务端处理
    C5 -->|RPC 请求| S4
    S4 --> S2
    S2 --> S3
    S3 --> S1
    S2 --> S5
    S5 --> S6
    S6 --> S7
    S7 -->|响应结果| C5

    %% 4. 基础配置
    APP --> CFG
    APP --> C3
    APP --> S2
    HDR --> C3
    HDR --> S5
    PROTO --> C2
    PROTO --> S1
```
## 流程说明
```mermaid
flowchart TB
    subgraph Client["客户端（caller）"]
        C1["业务代码"]
        C2["UserServiceRpc_Stub"]
        C3["MprpcChannel"]
        C4["MprpcController"]
        C5["Muduo TCP Client"]
    end

    subgraph Registry["服务注册与发现"]
        ZK["ZooKeeper"]
        ZKUtil["ZkClient"]
    end

    subgraph Server["服务端（callee）"]
        S1["UserService::Login\nUserService::Register"]
        S2["RpcProvider"]
        S3["ServiceMap + MethodMap"]
        S4["Muduo TcpServer"]
        S5["RpcHeader\nservice_name + method_name + args_size"]
        S6["解析请求并反射调用"]
        S7["序列化响应"]
    end

    C1 --> C2
    C2 --> C3
    C3 --> C4
    C3 --> ZKUtil
    ZKUtil --> ZK
    ZK -->|返回 ip:port| C3
    C3 -->|RPC请求| S4
    S4 --> S2
    S2 --> S3
    S3 --> S1
    S2 --> S5
    S5 --> S6
    S6 --> S7
    S7 -->|响应| C3
    S2 -->|注册服务| ZK
```


1. 客户端业务代码调用生成的 Stub。
2. MprpcChannel 负责打包 Protobuf 请求和 RPC Header。
3. ZkClient 查询 ZooKeeper，拿到目标服务的 ip 和 port。
4. 客户端连接服务端并发送请求。
5. RpcProvider 在服务端解析请求，定位 service 和 method。
6. 调用具体业务实现，例如 Login / Register。
7. 业务执行完成后，将响应回传给客户端。

这就是整个 mprpc 框架的核心调用链。

## 关键流程

1. 服务端启动时，RpcProvider::Run() 会读取配置，并在 ZooKeeper 中注册服务节点。
2. 客户端调用 Stub 方法时，MprpcChannel::CallMethod() 会先访问 ZooKeeper，拿到目标服务地址。
3. 客户端根据地址建立 TCP 连接，并把请求头和请求参数一起发送出去。
4. 服务端收到请求后，解析 RpcHeader，找到对应的 service 和 method，调用业务实现。
5. 业务函数执行完成后，返回响应并由框架序列化回传给客户端。

> 这个框架的核心是：Protobuf + Muduo + ZooKeeper + 自定义 RpcHeader。