cd /src/posts/94-contracts-beyond-assert/examples
g++ --version | head -1
for sem in ignore observe enforce quick_enforce; do
  for opt in -O0 -O2; do
    printf '%-14s %s: ' $sem $opt
    if g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=$sem $opt e1_bodyless_predicate.cpp -o /tmp/e1 2>/tmp/e1.err; then
      /tmp/e1; echo "links, runs rc=$?"
    else
      echo "FAILS: $(grep -o 'undefined reference to .*' /tmp/e1.err | head -2 | tr '\n' ' ')$(grep -m1 'error' /tmp/e1.err)"
    fi
  done
done
