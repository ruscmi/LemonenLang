# documentation ansi library on lmnlang

The ansi library allows you to decorate the look of your terminal, 
under the hood it uses ansi calls

**Base colors 0 to 7 enscape: **

There are a total of 7 colors in the base colors:

black,red,green,yellow,blue,magenta,cyan,white.

example using one of 7 colors

```lmnlang
lmport(ansi);
paint_red("Hello,world!")
```

if your terminal is friendly with UTF8, then you will see a beautiful red text 'Hello,world!'

cleaning Functions:

in total, there are 2 such function in the library:

```lmnlang
lmport(time);
lmport(ansi);
lmuck("ruscmi");
time_lmit(1); // for beauty
clear_screen() // will clear your terminal from the text
```

and

```lmnlang
lmport(ansi);
lmport(time);
lmuck(5,time_lmit(1),clear_line(),"hello") // First it will output 5, then wait a second, clear the screen and print 'hello'
```

**Next, let's move on to the functions that change the very appearance of the text**

There are 8 of them in total (from 2 to 9 in ANSI escapes)

List of them:

dim_write,italic_write,underlined_write,neon_write,blink_write,invert_write,hidden_write,through_write.

```lmnlang
lmport(ansi);
invert_write("HELLO")
```

must print black lettering on a white background.

In general, this is all, test the capabilities of the library, thank you for reading the documentation.

by [@ruscmi](https://github.com/ruscmi)
