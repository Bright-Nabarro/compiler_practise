```
# 开始符
CompUnit    ::= FuncDef;

# 函数定义与返回类型
FuncDef     ::= Type Ident "(" ParamList ")" Block;

# 函数参数列表定义
ParamList   ::= /* empty */ | Param ("," Param)* ;
Param       ::= Type Ident; 
Type        ::= "int" | "void" ;

# 块
Block       ::= "{" Stmt* "}";

# 语句，只能包含return
Stmt        ::= "return" Expr ";";
Expr        ::= Number | Ident ;
Number      ::= INT_LITERAL;
Ident       ::= [a-zA-Z_][0-9a-zA-Z_]*;
```

