/////////////////////////////////////////////////////////////////////
//    文件名： Base64.h
//    工程:  0151122244
//    作者：刘嘉磊，修改者：刘嘉磊
//    描述: Base64加密算法函数的声明文件/头文件。
//    版本: 1.0 2017/8/22。
//    修改: 创建 2017/8/22。
/////////////////////////////////////////////////////////////////////
#ifndef BASE_64_H
#define BASE_64_H
std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);
#endif
