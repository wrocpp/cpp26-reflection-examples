cd /src/posts/94-contracts-beyond-assert/examples
O=/tmp/e2; mkdir -p $O
body() { awk -v f="$1" '$0 ~ "^"f":" {on=1} on && /\.size/ {on=0} on' $2; }
echo "form    inline const_port semantic | in main: predicate calls, instructions | in make_address/connect_to: predicate calls"
for inl in 0 1; do
  for cp in 0 1; do
    for form in OPAQUE PURE INLINE; do
      for sem in ignore enforce; do
        g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=$sem -O2 -DPRED_$form=1 -DINLINE_ALL=$inl -DCONST_PORT=$cp -S e2_chain.cpp -o $O/t.s
        calls=$(body main $O/t.s | grep -cE "(call|bl)[[:space:]].*_Z1[03]valid_")
        instr=$(body main $O/t.s | grep -cE '^\s+[a-z]')
        other=$( { body _Z12make_addressi $O/t.s; body _Z10connect_toRK7Address $O/t.s; } | grep -cE "(call|bl)[[:space:]].*_Z1[03]valid_")
        printf '%-7s %-6s %-10s %-8s | main: calls=%s instr=%s | callees: calls=%s\n' $form $inl $cp $sem $calls $instr $other
      done
    done
  done
done
