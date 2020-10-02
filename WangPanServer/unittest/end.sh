mongo <<EOF
use UnitTestDB; 
db.dropDatabase();
EOF

rm UserFile/* -r
