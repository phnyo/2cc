#!/bin/bash

assert() {
  expected="$1"
  input="$2"

  ./2cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but instead got $actual"
    exit 1
  fi
}

assert 0 "0;"
assert 42 "42;"
assert 42 "1+2+3+6+24+6;"
assert 42 " 1 + 2          + 3 +6+24+6;"
assert 42 "6 * 7;"
assert 42 "(2 + 2 + 2) * (2 + 5);"
assert 42 "50 - 8;"
assert 42 "- 8 + 50;"
assert 42 "+8 + 34;"
assert 1  "42 == 42;"
assert 1 "42 > 0;"
assert 0 "42 < 0;"
assert 1 "42 >= 42;"
assert 1 "42 <= 42;"
assert 1 "6 * 7 == 38 + 4;"
assert 1 "a = 1;"
assert 1 "z = 1;"
assert 2 "a = 1; a = 1 + 1;"
assert 1 "hello = 1;"


echo OK
