# Tlang compiler

Компилятор небольшого тьюринг-полного языка программирования.

## Примеры

[**Factorial**](/examples/factorial.tlang):

```Rust
fn Factorial(num: i64): i64 {
  if (num == 1 || num == 0) {
    return 1;
  } else {
    return num * Factorial(num - 1);
  }
}
```

AST этой функции:

![ast_fact](/img/factorial_ast.png)

## Архитектура компилятора

Компиляция проходит в 3 этапа: `frontend`, `middleend`, `backend`.

`frontend` токинизирует файл с исходным кодом, после чего строится `AST` алгоритмом рекурсивного спуска, затем `AST` транслируется в `IR`.

`middleend` оптимизирует `IR`.

`backend` принимает `IR` и транслирует его в assembly для целевой архитектуры.

![compiler_arch](/img/compiler_arch.jpg)

## Флаги компиляции

| флаг | описание |
|------|----------|
| --ast-dump=<file_name.dot> | файл для вывода AST в формате .dot |
| --ir-dump=<file_name> | файл для вывода IR |
| --asm-dump=<file_name.asm> | файл для вывода assembly в формате .asm |
| --march=<arch_name> | целевая архитектура |
| --source-file=<file_name.tlang> | исходный файл в формате .tlang |

## Ключевые слова и операторы

| keyword | описание | пример |
|---------|----------|--------|
| `let`     | объявление переменных | `lex x: i64 = 0;` |
| `fn`      | объявление функций | `fn Main(): i64 {}` |
| `if else` | условное ветвление | `if (x == 1) {} else {}` |
| `while` | цикл while | `while (x == 1) {}` |
| `return` | возвращаемое значение | `return 3;` |
| `i64` | тип 64-битного целого числа | `lex x: i64 = 0;` |

| оператор | описание | пример |
|----------|----------|--------|
| `*`      | умножение | `x = a * b;` |
| `+`      | сложение | `x = a + b;` |
| `-`      | вычитание | `x = a - b;` |
| `%`      | остаток от деления | `x = a % b;` |
| `/`      | деление | `x = a / b;` |
| `==`     | равенстов | `if (a == b) {}` |
| `!=`     | неравенство | `if (a != b) {}` |
| `\|\|`     | логическое или | `if (a == 1 \|\| a == 0) {}` |
| `&&`     | логическое и | `if (a == 1 && b == 0) {}` |
| `=`      | присвоение | `x = 1;` |

## Сборка компилятора

Для генерации makefile
```bash
python make_make.py
```

После

Для сборки debug версии
```bash
make debug
```

Для сборки release версии
```bash
make release
```

Для очистки .o файлов
```bash
make clean
```