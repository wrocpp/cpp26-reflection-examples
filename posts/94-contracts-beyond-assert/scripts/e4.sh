cd /src/posts/94-contracts-beyond-assert/examples
O=/tmp/e4; mkdir -p $O
echo "== e4_switch.cpp: -Wall -Wextra -Wswitch -Wswitch-enum"
g++ -std=c++26 -fcontracts -Wall -Wextra -Wswitch -c e4_switch.cpp -o $O/a.o 2>&1 | grep -E "warning|error" | sed 's/^.*e4_switch.cpp://'
echo "== e3_result.cpp (declared-only callee): -fanalyzer -Wall -Wextra -Wtype-limits -Wtautological-compare"
g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=enforce -O2 -fanalyzer -Wall -Wextra -Wtype-limits -Wtautological-compare -c e3_result.cpp -o $O/b.o 2>&1 | head -10
echo "(end)"
echo "== e3_result.cpp (visible callee): -fanalyzer"
g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=enforce -O2 -fanalyzer -Wall -Wextra -DVISIBLE=1 -c e3_result.cpp -o $O/c.o 2>&1 | head -10
echo "(end)"
