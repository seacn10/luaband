# luaband整体结构

![image](https://github.com/seacn10/luaband/blob/master/img/struct.jpg)

# 模块功能

lua解析器为lua源代码，目前是用的是5.2.3，可以更新。lua debug 为解析出错时，捕捉解析器抛出的错误信息

lua Engine 对 lua C API的封装，负责与与lua_State进行交互。

Storage对脚本文件抽象，可以来自文件，网络或者内存。可自定义脚本加密解密，以及预编译。

Host对脚本执行点的挂载，hook脚本进入，执行，完结各个时间点。

Variant实现lua中变量转换到C中变量，lua调用c函数参数传递的原型。

wrapper负责组织storage和host。

impl为用户自定义C函数部分，其中实现一个Print函数。可添加自定义C函数。

Engine Manage 负责管理wrapper、impl，为最终暴露给用户的接口。

# 编译方式

本项目用visual studio 2013 编译，需要用到部分windows头文件。

# example

CLuaEngineImpl中的print为自定义打印函数
