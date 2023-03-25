struct ExpressionSymbol
{
	enum Type{
		Oper, Var
	};
	char Symbol;
	bool Value;
	enum Type type;
};

__kernel void compute_table(__global struct ExpressionSymbol* variables, int var_count, __global struct ExpressionSymbol* expression, int expr_sym_count, int oper_count,
	__global unsigned int* trues, __global bool* fAnswer)
{
	int id = get_global_id(0);
	int width = var_count + oper_count;
	for (int i = 0; i < width; ++i)
		fAnswer[id * width + i] = true;
}