#include"test.pb.h"
#include<iostream>
#include<string>
using namespace fixbug;

int main(){
    LoginResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(1);
    rc->set_errmsg("登陆处理失败了");

    GetFriendListResponse rsp1;
    ResultCode *rc1 = rsp.mutable_result();
    rc->set_errcode(0);

    User *u1 = rsp1.add_friend_list();
    u1->set_name("zhang san");
    u1->set_age(20);
    u1->set_sex(User::WOMAN);

    User *u2 = rsp1.add_friend_list();
    u1->set_name("li si");
    u1->set_age(22);
    u1->set_sex(User::MAN);
    
    std::cout << rsp1.friend_list_size() << std::endl;
    return 0;
}

int main1(){
    // 封装了login请求对象的数据
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");

    //对象数据序列化 =》 char*
    std::string send_str;
    if(req.SerializeToString(&send_str))
    {
        std::cout<<send_str<<std::endl;
    }

    //从send_str反序列化一个login请求对象
    LoginRequest reqB;
    if(reqB.ParseFromString(send_str))
    {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }
    return 0;
}