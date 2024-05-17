DEF_IR_OPERATOR(Add,          1)
DEF_IR_OPERATOR(Sub,          2)
DEF_IR_OPERATOR(Mul,          3)
DEF_IR_OPERATOR(And,          4)
DEF_IR_OPERATOR(Or,           5)
DEF_IR_OPERATOR(Equal,        6)
DEF_IR_OPERATOR(NotEqual,     7)
DEF_IR_OPERATOR(Div,          8)
DEF_IR_OPERATOR(Rem,          9)
DEF_IR_OPERATOR(Not,          10)
DEF_IR_OPERATOR(CallStart,    11) // <var_id_to_ret> CallStart <func_id> <n_param_expected>
DEF_IR_OPERATOR(CallEnd,      12) // <var_id_to_ret> CallEnd <func_id> <n_param_got
DEF_IR_OPERATOR(Assign,       13) // <var_id_dest> Assign <var_id_src> 0
DEF_IR_OPERATOR(Store,        14) // push like
DEF_IR_OPERATOR(Label,        15) // <label_id> Label 0 0
DEF_IR_OPERATOR(Func,         16) // <func_id> Func <n_param> 0
DEF_IR_OPERATOR(Jump,         17) // <label_id> Jump 0 0
DEF_IR_OPERATOR(JumpIf,       18) // <label_id> Jump <cond> 0
DEF_IR_OPERATOR(Tmp,          19) // <var_id> Tmp 0 0
DEF_IR_OPERATOR(Return,       20) // <var_id> Return 0 0
DEF_IR_OPERATOR(Parameter,    21) // <func_id> Parameter <var_id> 0
// FIXME V
DEF_IR_OPERATOR(Global,       22) // <var_id> Global 0 0
DEF_IR_OPERATOR(Less,         23)
DEF_IR_OPERATOR(LessEqual,    24)
DEF_IR_OPERATOR(Greater,      25)
DEF_IR_OPERATOR(GreaterEqual, 26)

// str_lit unsupported
//