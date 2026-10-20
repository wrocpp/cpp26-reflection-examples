cd /src/posts/94-contracts-beyond-assert/examples
O=/tmp/e5; mkdir -p $O
for port in 8080 70000; do
  for sem in ignore observe enforce quick_enforce; do
    printf 'PORT=%-6s %-14s: ' $port $sem
    if g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=$sem -O0 -DPORT=$port e5_constant_port.cpp -o $O/e5 2>$O/err.txt; then
      $O/e5; echo "compiles and runs, rc=$?  $(grep -c warning $O/err.txt) warning(s)  $(grep -m1 warning $O/err.txt | cut -c1-140)"
    else
      echo "COMPILE ERROR: $(grep -m1 -E 'error' $O/err.txt | sed 's/^.*e5_constant_port.cpp://' | cut -c1-200)"
    fi
  done
done
