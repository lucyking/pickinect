main:main.cpp
	echo `rm *.png`
	g++ `pkg-config --cflags opencv` -o main main.cpp `pkg-config --libs opencv`

clean:
	rm main 
	mv *.png ~/tmp/
