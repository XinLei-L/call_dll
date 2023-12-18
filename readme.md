##### C++调用C#生成的动态链接库dll

###### linux系统上搭建C#运行环境

- mono环境

  ```shell
  sudo apt-get update
  sudo ap-get install mono-complete
  ```

  

- dotnet安装

  进入到官网，下载二进制安装包，根据官网提示进行安装

  https://dotnet.microsoft.com/zh-cn/download/dotnet/6.0

  ![1702017244534](C:\Users\Acer\AppData\Roaming\Typora\typora-user-images\1702017244534.png)

  

###### src文件

- example.cs

  C#代码：主要实现两个接口

  接口HelloWorld：输出Hello from C#!

  接口Add：整型相加

- MyLibrary.dll

  ```shell
  mcs -target:library -out:MyLibrary.dll example.cs 
  ```

- test.cpp

  调用MyLibrary.dll库的Add接口，实现两个整数相加
  
  对test.cpp进行编译，生成执行文件my_cpp_program
  
  ```shell
  g++ -o my_cpp_program test.cpp -I /usr/include/mono-2.0 -lmono-2.0
  ```
  
  