change yyin to yy_scan_bytes 

# 目前待改正问题
1. 进行中... 语义分析(生成代码阶段)报错没有关联到源代码位置，需要在语法树中添加位置的记录信息
3. visit 转发实现 (困难)
4. handle的错误输出需要与main统一
5. handle 的返回值使用std::expected包装
6. 较多实现没有标记为[[nodiscard]]
7. ast.hpp 分割，实现分割，功能分割，统一宏定义
8. ast.hpp 模板添加概念约束
9. 添加优化的参数选项
10. ctype class改名，添加更多帮助型方法
