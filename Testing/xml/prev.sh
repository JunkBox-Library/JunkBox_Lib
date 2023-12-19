make prev

echo 1
cat data/prev_z.dat |./prev "<k>X</k><z>"
echo "ANS = z    OK?"
echo

echo 2
cat data/prev_x.dat |./prev "<k>X</k><k>Z</k><b>"
echo "ANS = b    OK?"
echo

echo 3
cat data/prev_y.dat |./prev "<k>Y</k><m><k>Z</k><b>"
echo "ANS = b    OK?"
echo

echo 4
cat data/change.dat |./prev "<key>SimulatorInfo</key><array><map><key>IP</key><binary>"
echo "ANS = binary     OK?"
