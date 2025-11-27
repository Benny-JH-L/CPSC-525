### WARNING
Do not upload any files in this repository to public websites. If you clone this
repository, keep it private.

# A4secret

This is the starter code for Assignment 4. The only files you need to edit and 
submit for grading are:
  * `guess1.cpp` in `task1/` directory
  * `guess2.cpp` in `task2/` directory
  * `guess3.cpp` in `task3/` directory
  * `guess4.cpp` in `task4/` directory

Each `task<n>/` directory contains:
* `secret<n>.c`
  * vulnerable program that compares user input to a stored hard-coded password
  * the hard-coded password is supplied via compilation flag by setting
    `STORED_SECRET` macro variable
  * the input password is provided via environment variable `SECRET`
  * if the passwords match, the program returns with exit code (0)
  * otherwise it returns with non-zero exit code
    * hint: study the source code to find out what the non-zero values indicate
*  `guess<n>.c`
  * this contains an unfinished guessn() function that you need to implement
  * this is the only file you may edit
  * **Do not** edit any other files.
* `driver<n>.cpp`
  * driver code that calls the `guessn()` function in `guess<n>.cpp` file
* `Makefile`
  * compiles 3 executables: `secret<n>a`, `secretn<n>b` and `guess<n>`


## Optimal solution timings for the provided executables:

Please note that my implementations typically search for solutions by testing characters in this order: `'0', '1', '2' .. ,'9', 'a', 'b', 'c' .. 'z'`. If you test your characters in different order, your results will be likely very different. To account for differences in testing order of characters, during grading we will try to test each submission on the worst case scenario.

I obtained the following timings on `cslinux`:

### Task 1
Using `vfork() + fexecve() + waitpid()`:

```
$ time ./guess1 ~pfederl/public/cpsc525/a4/secret1a
guessing password in /home/profs/pfederl/public/cpsc525/a4/secret1a
password='987'

real    0m11.559s
user    0m4.949s
sys     0m6.279s

$ time ./guess1 ~pfederl/public/cpsc525/a4/secret1b
guessing password in /home/profs/pfederl/public/cpsc525/a4/secret1b
password='zo0'

real    0m42.350s
user    0m17.303s
sys     0m23.732s
```
### Task 2
Using `system(3)`
```
$ time ./guess2 ~pfederl/public/cpsc525/a4/secret2a
guessing password in /home/profs/pfederl/public/cpsc525/a4/secret2a
password='easy1'

real    0m3.046s
user    0m0.016s
sys     0m0.024s

$ time ./guess2 ~pfederl/public/cpsc525/a4/secret2b
guessing password in /home/profs/pfederl/public/cpsc525/a4/secret2b
password='d1ff1cult990z'

real    0m1.480s
user    0m0.033s
sys     0m0.047s
```
### Task 3
Using `system(3)`
```
$ time ./guess3 ~pfederl/public/cpsc525/a4/secret3a
guessing password in /home/profs/pfederl/public/cpsc525/a4/secret3a
password='cake'

real    0m4.333s
user    0m0.037s
sys     0m0.078s

$ time ./guess3 ~pfederl/public/cpsc525/a4/secret3b
guessing password in /home/profs/pfederl/public/cpsc525/a4/secret3b
password='much0harderz'

real    0m7.162s
user    0m0.107s
sys     0m0.235s
```

### Task 4
Using `system(3)`
```
$ time ./guess4 ~pfederl/public/cpsc525/a4/secret4a
guessing password in /home/profs/pfederl/public/cpsc525/a4/secret4a
password='easy123'

real    3m04.046s
user    0m0.187s
sys     0m0.387s

$ time ./guess4 ~pfederl/public/cpsc525/a4/secret4b
guessing password in /home/profs/pfederl/public/cpsc525/a4/secret4b
password='deadbeefc0decak3'

real    2m56.585s
user    0m0.498s
sys     0m0.890s
```
