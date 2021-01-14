map : Point.o HSLAPixel.o RGB_HSL.o lodepng.o PNG.o Airport.o Route.o ColorPicker.o ImageTraversal.o Iterator.o Animation.o FloodFilledImage.o Graph.o main.o
	g++ main.o Graph.o Airport.o Route.o PNG.O HSLAPixel.o FloodFilledImage.o ColorPicker.o Point.o RGB_HSL.o Animation.o ImageTraversal.o Iterator.o lodepng.o -lc++abi -lm -o map

main.o : main.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic main.cpp

Graph.o : Graph.h Graph.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic Graph.cpp

Airport.o : Airport.h Airport.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic Airport.cpp

Route.o : Route.h Route.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic Route.cpp

PNG.o : PNG/PNG.h PNG/PNG.cpp 
	g++ -c -g -O0 -Wall -Wextra -pedantic PNG/PNG.cpp

HSLAPixel.o : PNG/HSLAPixel.h PNG/HSLAPixel.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic PNG/HSLAPixel.cpp

FloodFilledImage.o : FloodFill/FloodFilledImage.h FloodFill/FloodFilledImage.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic FloodFill/FloodFilledImage.cpp

ColorPicker.o : FloodFill/ColorPicker.h FloodFill/ColorPicker.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic FloodFill/ColorPicker.cpp
	
Point.o : FloodFill/Point.h FloodFIll/Point.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic FloodFill/Point.cpp

RGB_HSL.o : PNG/RGB_HSL.h PNG/RGB_HSL.cpp
	g++ -c PNG/RGB_HSL.cpp

Animation.o : FloodFill/Animation.h FloodFill/Animation.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic FloodFill/Animation.cpp

ImageTraversal.o : FloodFill/imageTraversal/ImageTraversal.h FloodFill/imageTraversal/ImageTraversal.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic FloodFill/imageTraversal/ImageTraversal.cpp

Iterator.o : FloodFill/imageTraversal/ImageTraversal.h FloodFill/imageTraversal/Iterator.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic FloodFill/imageTraversal/Iterator.cpp

lodepng.o : PNG/lodepng/lodepng.h PNG/lodepng/lodepng.cpp
	g++ -c -g -O0 -Wall -Wextra -pedantic PNG/lodepng/lodepng.cpp

clean:
	rm -rf *.o map test
