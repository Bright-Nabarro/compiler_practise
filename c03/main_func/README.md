change yyin to yy_scan_bytes 

# 目前待改正问题
1. 语义分析(生成代码阶段)报错没有关联到源代码位置，需要在语法树中添加位置的记录信息
2. general_visitor的llvm::Type* m_void_ty; llvm::Type* m_int32_ty; 需要放在一个更加通用的工具命名空间或者工具类中, 包括生成zero
3. visit 转发待实现 (困难)
4. handle的错误输出需要与main统一
5. handle 的返回值使用std::expected包装
6. 较多实现没有标记为[[nodiscard]]

