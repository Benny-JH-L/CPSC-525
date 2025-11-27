# Collaboration statement

I worked on this assignment alone.


### Benny Liang | UCID: 30192142 | Assignment 4

For each task, include a paragraph in the README.md describing your solution for each task. The description of your exploit MUST be SHORT and CONCISE, while clearly demonstrating you know what you exploited and how you exploited it. You will lose marks for superfluous prose. For example, do not write about approaches that failed.

### Task 1
The `str2long()` function only copies the first 3 bytes (first 3 characters) of
its input string, then the function `compare_secrets_1()` only compares the first 3 characters of each string
(my input and the hardcoded password strings). Thus, the exploit I used was to run the vulnerable program and 
go though every possible 3 character long valid password until the program returned '0' (ie until it thought 
I gave it the hardcoded password).

### Task 2
In `compare_secrets_2()` function, it returns the result of `strcmp()`; where if a character at some index `i`
doesn't match for `s1` and `s2` then it return's how "far away" s2[i] is from s1[i] (in ascii).
For example, if s1 = "easy1" (hardcoded password) and s2 = "x" (our input), then the return of `strcmp(s1, s2)` is -19, beacuse the ascii of 'e' is 101 and the ascii of 'x' is 120, so 'x' is -19 (101 - 120) "far away" from 'e'. Using this, method we can determine the character at index `i` of the hardcoded password (given the characters of our input from index 0 to `i - 1` match the hardcoded password). My exploit starts the vulnerable program and sends an arbitrary password of length 1 and retrieves the hardcoded character at index 0 based on the return value. Then it starts it again and sends the password: <character(s) found> + <arbitrary char>, and finds the character at index 1 based on the return value. We keep repeating this process until we encounter the terminating character, `\0`, when this is found we can drop the last chacter in the password we sent, which will result in the hardcoded password.

### Task 3
`compare_secrets_3()` is very similar to `compare_secrets_2()`, but instead of returning the result of `strcmp(s1, s2)` it returns if
the differing character, with some index `i`, at s2[i] is "larger" or "smaller" than the character at s1[i], ie returns if the ascii of s2[i] is <, or > the ascii of s1[i], and if they are the same it returns `0`. Knowing this I made a vector<char> containing the valid password characters, then I sorted them (based on their ascii values) from least to greatest (because I will be binary searching for the hardcoded password's characters). Initially the `password_found` = `""` (empty), I run the vulnerable program to find the 1st character of the hardcoded password, using binary search to find the character in vector<char> to test (ie the "local middle"), I send the password `password_found` (ie `""`) + <char from binary search>, based on the return I adjust the search range on the vector<char> and repeat until the program return's 0 (ie until we've found the hardcoded character, then `password_found` = `password_found` + `<hardcoded char>`). This proccecss is repeated for each index until we've recovered the entire hardcoded password.

### Task 4
The vulnerable program employs a delaying mechanism in `compare_secrets_4()` when it compares each character of the input string with the hardcoded password. We can determine a character of the hardcoded password from how long it takes the program to return based on our input. For example, if the hardcoded password is "001" and we input "a", then "b", ..., and let's say it takes on average ~203ms until it returns for those guesses, and if we send "0" we will get a longer return time beacuse it will need to do another loop-iteration in `compare_secrets_4()`, then its highly likely that "0" is the hardcoded character at that index. The longer it takes the program to return after a `guess` character, the more probable it is the hardcoded character. Then to find the hardcoded password I do this guessing strategy for each index, and only taking the character that resulted in the longest return time as the hardcoded character at that index. If the average return time of all the characters of some index is within some single digit %, then we've encountered the terminating character `\0` of the hardcoded password which means we found the hard coded password and can terminate. Note: to guess an index `i` of the hardcoded password, we need the characters from index `0` to `i-1` to be hardcoded characters we've found.

