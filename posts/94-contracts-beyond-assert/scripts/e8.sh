cd /src/posts/94-contracts-beyond-assert/examples
for opt in -O0 -O2; do
  g++ -std=c++26 -Wall -Wextra -fcontract-evaluation-semantic=observe $opt contracts_that_repeat.cpp -o /tmp/e8 && echo "== $opt" && /tmp/e8; echo "rc=$?"
done
