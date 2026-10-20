cd /src/posts/94-contracts-beyond-assert/examples
O=/tmp/e3; mkdir -p $O
body() { awk -v f="$1" '$0 ~ "^"f":" {on=1} on && /\.size/ {on=0} on' $2 | grep -E '^\s+[a-z]' | sed 's/^\s*//' | tr '\n' ';' ; }
for vis in 0 1; do
  for sem in ignore enforce; do
    g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=$sem -O2 -DVISIBLE=$vis -S e3_result.cpp -o $O/t.s 2>$O/warn.txt
    echo "== visible=$vis semantic=$sem   (warnings: $(grep -c warning $O/warn.txt))"
    for fn in lt eq ne le; do
      echo "  failed_$fn: $(body _Z9failed_${fn}RK7Address $O/t.s)"
    done
    echo "  ok_le: $(body _Z5ok_leRK7Address $O/t.s)"
  done
done
echo "== warnings with -Wall -Wextra -Wtype-limits -Wtautological-compare (visible=1, enforce)"
g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=enforce -O2 -Wall -Wextra -Wtype-limits -Wtautological-compare -DVISIBLE=1 -c e3_result.cpp -o $O/t.o 2>&1 | head -20
echo "(end of warnings)"
