echo "compiling"
cc -c -std=c99 -O0 -g -DNDEBUG -o dr.o dr.c 
cc -c -std=c99 -O0 -g -DNDEBUG -o drinit.o drinit.c
cc -c -std=c99 -O0 -g -DNDEBUG -o drprint.o drprint.c
cc -c -std=c99 -O0 -g -DNDEBUG -o o.o o.m

echo "linking"
ld -dynamic -arch arm64 -map dr.map -o dr dr.o drinit.o drprint.o o.o \
    -syslibroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk \
    -framework System \
    -framework Foundation

rm ./*.o