LIBS = -lglfw -lGL -ldl -lX11 -lpthread -lXrandr -std=c++11 -lGLU -w
CFLAGS = -Wall -Iinclude
GLADH = include/glad.h
GLADC = include/glad.c
GLAD = glad.o $(GLADH)
shader = include/shader.h
convexhull = include/ConvexHull.h
medianh = include/Median.h
medianc = include/median.cpp
median = median.o $(medianh)
point = include/Point.h
main: main.cpp $(medianh) $(convexhull) $(point) $(GLAD) $(shader)
	g++ -o draw main.cpp $(GLAD) $(LIBS) $(CFLAGS)
mediantest: median.cpp $(median)
	g++ median.cpp $(median)
glad.o: $(GLADH)
	g++ -c $(GLADC)
median.o: $(medianh)
	g++ -c $(medianc)
ConvexHull: Point.h
	g++ $(convexhull)

