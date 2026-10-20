cd /src/posts/94-contracts-beyond-assert/examples
for opt in -O0 -O2; do
  g++ -std=c++26 $opt e7_assume.cpp -o /tmp/e7 && printf '%s: prints ' $opt && /tmp/e7
done
echo "-- asm of checked() at -O2:"
g++ -std=c++26 -O2 -S e7_assume.cpp -o /tmp/e7.s
awk '/^_Z7checkedi:/{on=1} on && /\.size/{on=0} on' /tmp/e7.s | grep -E '^\s+[a-z]' | sed 's/^\s*//' | tr '\n' ';'; echo
