# lmuck() documentation

lmuck() - base function in LemonenLang it is capable of displaying text, variables, numbers, arrays and a bunch of other things.

**examples**

The basic output of the strings is:

```lmnlang
lmuck("Hello,world!")
```

string concatenation:

```lmnlang
lmuck("Hello","World","!")
```

output variables and numbers:

```lmnlang
x = 5;
lmuck(x)
```

```lmnlang
lmuck(10)
```

output expressions:

```lmnlang
lmuck(10+5/6)
```

output variables + strings:

```lmnlang
name = "ruscmi";
lmuck("My name is ",name)
```

output arrays and dictionaries:

```lmnlang
arr = [5,10];
lmuck(arr);
lmuck(arr[0]) // output index[0] - 5
```

```lmnlang
dict = { "name" : "ruscmi", "age" : 14 };
lmuck(dict);
lmuck(dict[0]) // output index[0] - ruscmi
```

** Thanks for reading the documentation for the base function from lmnlang - lmuck() **
