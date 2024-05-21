DEF_IR_OPERATOR(Add,          1) // done
DEF_IR_OPERATOR(Sub,          2) // done
DEF_IR_OPERATOR(Mul,          3) // done
DEF_IR_OPERATOR(And,          4) // done
DEF_IR_OPERATOR(Or,           5) // done
DEF_IR_OPERATOR(Equal,        6) // done
DEF_IR_OPERATOR(NotEqual,     7) // done
DEF_IR_OPERATOR(Div,          8)
DEF_IR_OPERATOR(Rem,          9)
DEF_IR_OPERATOR(Not,          10) // done
DEF_IR_OPERATOR(CallStart,    11) // <var_id_to_ret> CallStart <func_id> <n_param_expected>
DEF_IR_OPERATOR(CallEnd,      12) // <var_id_to_ret> CallEnd <func_id> <n_param_got
DEF_IR_OPERATOR(Assign,       13) // <var_id_dest> Assign <var_id_src> 0
// DEF_IR_OPERATOR(Store,        14) // push like ???
DEF_IR_OPERATOR(Label,        14) // <label_id> Label 0 0
DEF_IR_OPERATOR(Func,         15) // <func_id> Func <n_param> 0
DEF_IR_OPERATOR(Jump,         16) // <label_id> Jump 0 0
DEF_IR_OPERATOR(JumpIf,       17) // <label_id> Jump <cond> 0
DEF_IR_OPERATOR(Tmp,          18) // <var_id> Tmp 0 0
DEF_IR_OPERATOR(Return,       19) // <var_id> Return 0 0
DEF_IR_OPERATOR(Parameter,    20) // <func_id> Parameter <var_id> 0
// FIXME V
DEF_IR_OPERATOR(Global,       21) // <var_id> Global 0 0
DEF_IR_OPERATOR(Less,         22)
DEF_IR_OPERATOR(LessEqual,    23)
DEF_IR_OPERATOR(Greater,      24)
DEF_IR_OPERATOR(GreaterEqual, 25)

// str_lit unsupported
//