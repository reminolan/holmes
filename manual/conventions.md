
# Project Holmes - Coding Conventions

----

This coding convention is built on two major foundations:

1. Readability
2. Self-Describing Code

All decisions made as part of this convention, both traditional and novel, should ask whether they adhere to these foundations.

## Line Length

No line in Project Holmes source code should exceed 80 characters long.
This is because long lines of code either overflow off the screen or wrap onto the line below.
Either way they hamper the readability.

The exact length of 80 characters was chosen in part because of tradition.
However it also means that a vertical split on a widescreen display maintains completely legible.

## Keywords

Keywords like `if`, `else`, `return`, et cetera, should always have padding on either side.
`else` should appear on the same line as the closing bracket of the associated `if` statement.


## Brackets, Braces, and Parentheses

Some people refer to these characters differently, so the follow block explains this convention's names:

```c
(Curly) Brackets = {}
Braces = []
Parentheses = ()
```

Braces should always be immediately adjacent to the token that they are modifying with no space or newline.

Parentheses should not be have interior or exterior padding of their own.

Brackets should only be separated from neighouring tokens by a space (not a newline).
The only exception being brackets that denote function bodies, these should be separate from the function declaration by a newline.

For example:

```c
int CountArrayZeros(int array_count, int* array_value)
{
  int num_zeros = 0;

  for (int index = 0; index < array_count; ++index) {
    if (array_value[index] == 0) {
      ++num_zeros;
    }
  }

  return num_zeros;
}
```

## Naming

Names in the Holmes Project should always be concise and explicit.
The name of any symbol should convey the usage in as few characters as possible (to best fit under 80 per line).

### Variables

Variable names are written in `snake_case`.
Variables should be a descriptive noun that explains their use in scope.

The only exception where single-letter variables are acceptable is when the function is a shorthand/utillity like the common `Swap` function.

Example:

```c
typedef struct String {
  int length;
  char* buffer;
} String;
```

### Types

Type names are written in `PascalCase`.
Types should given a descriptive noun as their name.
This noun should describe the role that the type plays.
For example, you could have the following two types:

```c
enum WindowOptions;
typedef void Window;
```

### Functions

Function names are written in `PascalCase`, with a prefixed 'namespace' and an underscore if they are exported.
The name of Functions should follow VerbSubjectObject.
Any function name should explicitly state what exact role of the function.

For example:

```c
EXPORT H_SetWindowFlag()
```

#### Parameters

Parameters should generally be named like variables, with one primary exception.
The parameter that the function primarily operates on should always be called `target`.
Especially if the function makes persistant changes to the parameter.

For example:

```
EXPORT H_SetWindowFlag(Window* target, WindowFlag flag);
```

## Complete Example Code

```c
int CountArrayZeros(int array_count, int* array_value)
{
  int num_zeros = 0;

  for (int index = 0; index < array_count; ++index) {
    if (array_value[index] == 0) {
      ++num_zeros;
    }
  }

  return num_zeros;
}


```

