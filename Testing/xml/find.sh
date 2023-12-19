make findxml
echo 1
cat data/multi.dat | ./findxml "<map><key>seed-capability</key><string>"
echo 2
cat data/login_s2v.xml |./findxml "<member><name>seed_capability</name><value><string>"
echo 3
cat data/login_s2v.xml |./findxml "<member><name>agent_id</name><value><string></string></value></member>"
echo 4
cat data/sim_name.xml |./findxml  "<llsd><map><key>region_name</key><string></string></map></llsd>"
echo 5
cat data/map_layer.xml |./findxml  "<llsd><map><key>MapLayer</key><string>"
