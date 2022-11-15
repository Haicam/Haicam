
#ios on Mac
git clone git@github.com:ezhomelabs/VLCKit.git
cd VLCKit

git checkout haicam
./compileAndBuildVLCKit.sh -f -r -b

git checkout 3.5.0
./buildMobileVLCKit.sh -f -b