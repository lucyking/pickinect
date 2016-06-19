main:main.cpp
    g++ `pkg-config --cflags opencv` -o main main.cpp `pkg-config --libs opencv`

clean:
    rm main 
