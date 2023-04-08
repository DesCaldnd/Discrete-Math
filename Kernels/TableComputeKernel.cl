struct ExpressionSymbol
{
	enum Type{
		Oper, Var
	};
	char Symbol;
	bool Value;
	enum Type type;
};

int stack_index = -1;

unsigned int power_of_2(int power)
{
	unsigned int result = 1;
	
	for (int i = 0; i < power; ++i, result*=2);
	
	return result;
}

__kernel void compute_table(__global struct ExpressionSymbol* variables, int var_count, __global struct ExpressionSymbol* expression, int expr_sym_count, int oper_count,
	__global unsigned int* trues, __global bool* fAnswer, __local struct ExpressionSymbol* stack, __local struct ExpressionSymbol* vars_init)
{
	int gid = get_global_id(0);
	int lid = get_local_id(0);
	int width = var_count + oper_count;
	int oper = 0;
	unsigned int vars_2 = power_of_2(var_count);
	
	for (int i = 0; i < var_count; ++i)
	{
		struct ExpressionSymbol var = variables[i];
		var.Value = (1u << (var_count - i - 1)) & gid;
		vars_init[var_count * lid + i] = var;
		fAnswer[(vars_2 - gid - 1) * width + i] = var.Value;
	}
	
	for (int i = 0; i < expr_sym_count; ++i)
	{
		if (expression[i].type == Var)
		{
			++stack_index;
			struct ExpressionSymbol var;
			var.type = Var;
			bool val;
			char sym = expression[i].Symbol;
			for (int j = 0; j < var_count; ++j)
			{
				if (vars_init[var_count * lid + j].Symbol == sym)
				{
					var.Value = vars_init[var_count * lid + j].Value;
					break;
				}
			}
			stack[(expr_sym_count - oper_count) * lid + stack_index] = var;
		} else
		{
			if (expression[i].Symbol == '-')
			{
				stack[(expr_sym_count - oper_count) * lid + stack_index].Value = !stack[(expr_sym_count - oper_count) * lid + stack_index].Value;
			} else
			{
				struct ExpressionSymbol var_1, var_2;
				var_2 = stack[(expr_sym_count - oper_count) * lid + stack_index];
				--stack_index;
				var_1 = stack[(expr_sym_count - oper_count) * lid + stack_index];
				--stack_index;
				bool f = var_1.Value, s = var_2.Value;
				char sym = expression[i].Symbol;
				switch (sym)
				{
					case '&':
					{
						f = f && s;
						break;
					}
					case '|':
					{
						f = f || s;
					break;
					}
					case '<':
					{
						f = f <= s;
						break;
					}
					case '~':
					{
						f = f == s;
						break;
					}
					case '/':
					{
						f = f && !s;
						break;
					}
					case '+':
					{
						f = f != s;
						break;
					}
				}
				var_1.Value = f;
				++stack_index;
				stack[(expr_sym_count - oper_count) * lid + stack_index] = var_1;
				fAnswer[(vars_2 - gid - 1) * width + var_count + oper] = var_1.Value;
				++oper;
			}
		}
	}
	if (fAnswer[(vars_2 - gid - 1) * width + var_count + oper_count - 1] == true)
	{
		atomic_add(trues, 1u);
	}
}