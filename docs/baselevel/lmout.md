# lmout() func documentation

lmout() - base function in lmnlang for user input

**format**

for using lmout() in code use similar format:

```lmnlang
lmout("inpline: ")
```

**real examples of using lmout():**

```lmnlang
lmport(system);
lmport(string);
inpline = lmout("> ");
inplined = str_split(inpline);
if(inplined[0] != "rm") { // will make it safe for Windows users to enter
	sys_exec(inpline)
} 
```

**Or for example, the game Guess the Number (without a random library):**

```lmnlang
secret_num = 67;
i = 0;
while(true) {
	input = lmtod(lmout("guess the number @> "));
	if(lmtype(input) == "NUM") {
		if(input == secret_num) {
			lmuck("YOU WIN!")
			break
		}
		else{
			lmuck("try again,attempt: ",i,"...")
			i = i + 1;
		}
		if(i > 5) {
			lmuck("You fail...")
			break
		}
	}else {
		lmuck("[E]: unknown value")
	}
}
```

**Thanks for reading the lmout() documentation**

by [ruscmi](https://github.com/ruscmi)
