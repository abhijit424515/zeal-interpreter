# Monkey Language

We can bind values to names like this:

```
let x = 10;
```

In Monkey, everything is an object, including functions. 

The language supports the following object types:
- `int`
- `double`
- `bool`
- `string`
- `null`
- `err`
- `func`

> The names/identifiers binded to these objects support dynamic typing, and so a different object can be binded to the same name at a later point in the program.

The language supports the following operators:
- `+`
- `-`
- `*`
- `/`
- `==`
- `!=`
- `>`
- `<`
- `>=`
- `<=`
- `!`
- `&&`
- `||`

Comments can be written using the `//` syntax.

---

Function objects can be created using the `fn` keyword.

```
let add = fn(x, y) { return x + y; };
```

The language supports implicit return inside functions, so the above function can also be written like this.

```
let add = fn(x, y) { x + y; };
```

Functions can be called like this:

```
add(5, 10);
```

Of course, Monkey supports recursive functions as well.

```
let fibonacci = fn(x) {
	if (x == 0) {
		return 0;
	} else {
		if (x == 1) {
			return 1;
		} else {
			fibonacci(x - 1) + fibonacci(x - 2);
		}
	}
};
```

Additionally, Monkey supports higher order functions.

```
let twice = fn(f, x) {
	return f(f(x));
};

let addTwo = fn(x) {
	return x + 2;
};

twice(addTwo, 2); // -> 6
```

And to support higher order functions, Monkey also supports closures.

```
let makeAdder = fn(x) {
	return fn(y) { x + y; };
};

let addTwo = makeAdder(2);
addTwo(3); // -> 5
```

This also works

```
let const_adder = fn() {
	let x = 2;
	return fn(y) { x + y; };
};

let increment = const_adder();
increment(3); // -> 5
```

> To implement closures, one way would be to copy a shared pointer for the function's local scope, and attach it to the returned function object. This way, the returned function object can access that scope, even though that scope has been relinquished by the parent function that created it. 

---

The language has a built-in function named `print`, which can be used to print values to the console. 

```
print("Hello, World!");
```

Additionally, it also has the built-in function `len`, which can be used to get the length of a string.