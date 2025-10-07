import operator as op
import math

# === Tokenizer ===
def tokenize(code):
    return code.replace('(', ' ( ').replace(')', ' ) ').split()

# === Parser ===
def parse(tokens):
    if not tokens:
        raise SyntaxError("Unexpected EOF")
    token = tokens.pop(0)
    if token == '(':
        L = []
        while tokens[0] != ')':
            L.append(parse(tokens))
        tokens.pop(0)  # Remove ')'
        return L
    elif token == ')':
        raise SyntaxError("Unexpected )")
    else:
        return atom(token)

def atom(token):
    try:
        return int(token)
    except ValueError:
        try:
            return float(token)
        except ValueError:
            return str(token)

# === Environment ===
def standard_env():
    env = {}
    env.update({
        '+': op.add,
        '-': op.sub,
        '*': op.mul,
        '/': op.truediv,
        'mod': op.mod,
        '>': op.gt,
        '<': op.lt,
        '>=': op.ge,
        '<=': op.le,
        '=': op.eq,
        'abs': abs,
        'pow': pow,
        'sqrt': math.sqrt,
        'max': max,
        'min': min,
        'print': print,
    })
    return env

# === Evaluator ===
def eval_expr(x, env):
    if isinstance(x, str):           # Variable reference
        return env[x]
    elif not isinstance(x, list):    # Constant literal
        return x
    elif x[0] == 'define':           # (define var expr)
        _, var, expr = x
        env[var] = eval_expr(expr, env)
    elif x[0] == 'if':               # (if test conseq alt)
        _, test, conseq, alt = x
        result = eval_expr(test, env)
        return eval_expr(conseq if result else alt, env)
    else:                            # Procedure call
        proc = eval_expr(x[0], env)
        args = [eval_expr(arg, env) for arg in x[1:]]
        return proc(*args)

# === Run code ===
def run(code, env=None):
    if env is None:
        env = standard_env()
    tokens = tokenize(code)
    ast = parse(tokens)
    return eval_expr(ast, env)

def print_ast(ast, indent=0):
    if isinstance(ast, list):
        print('  ' * indent + '(')
        for elem in ast:
            print_ast(elem, indent + 1)
        print('  ' * indent + ')')
    else:
        print('  ' * indent + str(ast))


env = standard_env()

while True:
    text = input('> ')
    run(text, env)