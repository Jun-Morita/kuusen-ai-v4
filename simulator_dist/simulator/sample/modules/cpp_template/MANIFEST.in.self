include builder.sh
include builder.bat
include CMakeLists.txt
include MANIFEST.in.self
graft {packageName}
graft include
graft src
global-exclude *.pyc
prune build
graft thirdParty/scripts
