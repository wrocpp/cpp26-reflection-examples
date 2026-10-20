cd /src/posts/94-contracts-beyond-assert/examples
O=/tmp/e6; mkdir -p $O
for typed in 0 1; do
  for sem in ignore enforce; do
    g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=$sem -O2 -DTYPED=$typed -S e6_types.cpp -o $O/t.s 2>$O/err.txt || { echo "ERROR typed=$typed $sem"; head -5 $O/err.txt; continue; }
    sites=$(grep -cE "(call|bl)[[:space:]].*handle_contract_violation" $O/t.s)
    total=$(grep -cE '^\s+[a-z]' $O/t.s)
    g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=$sem -O2 -DTYPED=$typed e6_types.cpp -o $O/e6 2>/dev/null || g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=$sem -O2 -DTYPED=$typed e6_types.cpp -o $O/e6 -lstdc++exp
    $O/e6; rc=$?
    printf 'typed=%s %-8s: violation-handler call sites in the file=%s, instructions in the file=%s, runs rc=%s\n' $typed $sem $sites $total $rc
  done
done
