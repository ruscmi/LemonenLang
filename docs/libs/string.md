# string library documentation

string lib - standart library in lmnlang to manage rows

**functional**

 str_invertion(str) - invertion string in parentheses

 example:

 ```lmnlang
 lmport(string);
 str_invertion("Hello")
 ```

 execute 'olleH'
 
 str_repeat(str,count) - repeat string 
 as many times as you type in instead of count

 example:

 ```lmnlang
 lmport(string);
 str_repeat("lmn",5)
 ```

 execute 'lmnlmnlmnlmnlmn'
 
 str_substring(str,start,len) - will cut off the part from the start
 counter from the row to the length you want to cut 

 example:

```lmnlang
lmport(string);
str_substring("HELLO",1,2)
```

 execute 'EL'

 str_join(arr,text) - joins all the elements of
 the array into a single line 
 You can also enter an argument between
 the attached elements of the
 array instead of the text:

 example:

 ```lmnlang
 lmport(string);
 str_join(["+7",777,77,77],"-")
 ```

 execute '+7-777-77-77'
 
 str_split(string) - slices the string by spaces 
 and places each word in the string in a 
 separate array element.

 example:

 ```lmnlang
 lmport(string);
 str_split("hello world !")
 ```

 execute '[hello, world, !]'

by [@kkod85853-ux](https://github.com/kkod85853-ux)
