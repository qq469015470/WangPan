mongo <<EOF
use UnitTestDB; 
db.UserInfo.insert({name: 'testA', password: '123123', token: new ObjectId()});
db.UserInfo.insert({name: 'testB', password: '456456', token: new ObjectId('5349b4ddd2781d08c09890f3')});
db.UserInfo.insert({name: 'testDir', password: '456456', token: new ObjectId('5f719c3ceca2144f4b36b200')});
EOF

mkdir -p UserFile/qq469015470
mkdir -p UserFile/testB
mkdir -p UserFile/testDir
mkdir -p UserFile/testDir/removedir
mkdir -p UserFile/UnitTestDelete

touch UserFile/qq469015470/test.txt
touch UserFile/qq469015470/uploading.txt.upload
touch UserFile/testDir/testdir.upload
touch UserFile/testB/downfile.txt
echo "downcontent" > UserFile/testB/downfile.txt

mkdir -p UserFile/testDir/wenjianjia
mkdir -p UserFile/testDir/wenjianjia/wenjianjia2
touch UserFile/testDir/hello.txt
touch UserFile/testDir/abc.txt
