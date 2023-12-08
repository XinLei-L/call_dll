#include <iostream>
#include <cstring>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/exception.h>

int main() {
    // 初始化 Mono 运行时。"MyLibrary" 是传递给 Mono 运行时的域名，它在 Mono 中用于隔离不同的应用程序域。
    mono_jit_init("MyLibrary");

    // 获取当前域
    MonoDomain *domain = mono_domain_get();

    // 打开 C# 动态库（DLL 文件）
    MonoAssembly *assembly = mono_domain_assembly_open(domain, "MyLibrary.dll");
    if (!assembly) {
        std::cerr << "Error opening assembly: MyLibrary.dll" << std::endl;
        return 1;
    }

    // 获取程序集的镜像。程序集是 Mono 中的一个概念，它是一组相关的代码和资源的集合。镜像是程序集的内存映像，包含了编译后的代码。
    MonoImage *image = mono_assembly_get_image(assembly);

    // Find and invoke the Add method
    // const char *namespace_name = " ";  // 设置C#类的命名空间
    const char *class_name = "MyLibrary";  // 设置 C# 类的名称
    const char *method_name = "Add";  // 设置C#类的函数接口名

    // 通过类名从镜像中获取 MonoClass。这是获取包含 C# 类信息的 MonoClass 结构的一种方式。
    // MonoClass *klass = mono_class_from_name(image, namespace_name, class_name);
    MonoClass *klass = mono_class_from_name(image,"",class_name);  // 这里没有命名空间爱你，设置为空

    if (!klass) {
        std::cerr << "Error finding class: " << class_name << std::endl;
        return 1;
    }

    // 通过方法名从类中获取 MonoMethod。这是获取 C# 方法信息的一种方式。
    MonoMethod *method = mono_class_get_method_from_name(klass, method_name, -1);
    if (!method) {
        std::cerr << "Error finding method: " << method_name << std::endl;
        return 1;
    }
    int a = 4;
    int b = 3;

    // Prepare arguments and invoke the method
    // 定义参数数组，将整数地址作为参数传递给 C# 方法
    void *args[2];
    args[0] = &a;
    args[1] = &b;

    // exc 是一个指向 MonoObject 类型的指针，用于存储可能在调用 C# 方法时抛出的异常。在开始时，将其初始化为 nullptr，表示当前没有异常。
    MonoObject *exc = nullptr;

    // 使用 Mono Runtime 调用 C# 方法。method 是先前获取的 C# 方法，args 是传递给方法的参数数组，result 是调用方法后的返回值。
    MonoObject *result = mono_runtime_invoke(method, nullptr, args, &exc);

    /*
        如果调用方法时发生异常，将获取异常信息并输出到标准错误流。
        在调用 mono_runtime_invoke 后，如果 exc 不为 nullptr，说明在调用 C# 方法时发生了异常。
        使用 mono_object_get_class 获取异常对象的类。
        使用 mono_class_get_property_from_name 获取异常类中名为 "Message" 的属性。
        使用 mono_property_get_value 获取异常消息，并将其输出到标准错误流。
        最后，通过 mono_free 释放用于存储异常消息的内存，并返回一个错误代码（1）。
    */
    if (exc) {
        MonoClass *e_class = mono_object_get_class(exc);
        MonoString *e_message = (MonoString *)mono_property_get_value(mono_class_get_property_from_name(e_class, "Message"), exc, NULL, NULL);
        const char *e_message_str = mono_string_to_utf8(e_message);
        std::cerr << "Exception: " << e_message_str << std::endl;
        mono_free(const_cast<void*>(static_cast<const void*>(e_message_str)));
        return 1;
    }

    // Extract the result
    // 提取 C# 方法返回的整数结果。mono_object_unbox 函数用于解箱装箱后的值，然后使用 memcpy 将结果从 MonoObject 拷贝到本地整数变量中。
    int resultValue;
    mono_object_unbox(result);
    memcpy(&resultValue, mono_object_unbox(result), sizeof(int));

    // Print the result
    std::cout << "Result of addition: " << resultValue << std::endl;

    // 清理 Mono 运行时资源。在程序结束时，应该清理 Mono 运行时占用的资源
    mono_jit_cleanup(domain);

    return 0;
}

