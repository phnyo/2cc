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

assert 0 0
assert 42 42
assert 42 "1+2+3+6+24+6"
assert 42 " 1 + 2          + 3 +6+24+6"

echo OK
