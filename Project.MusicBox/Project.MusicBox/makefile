#makefile for Project.MusicBox

#Link
BW_MusicBox : main.o CommandInterpreter.o ext_lib.o File_IO.o TrackDataEditer.o WaveSynthesizer.o
	gcc -o BW_MusicBox main.o CommandInterpreter.o ext_lib.o File_IO.o TrackDataEditer.o WaveSynthesizer.o -lm


#Compile module
#Compile main.c
main.o : main.c main.h CommandInterpreter.h File_IO.h TrackDataEditer.h WaveSynthesizer.h Global.h ext_lib.h
	gcc -c main.c

#Compile CommandInterpreter.c
CommandInterpreter.o : CommandInterpreter.c CommandInterpreter.h Global.h ext_lib.h
	gcc -c CommandInterpreter.c

#Compile ext_lib.c
ext_lib.o : ext_lib.c ext_lib.h
	gcc -c ext_lib.c

#Compile File_IO.c
File_IO.o : File_IO.c File_IO.h Global.h ext_lib.h
	gcc -c File_IO.c

#Compile TrackDataEditer.c
TrackDataEditer.o : TrackDataEditer.c TrackDataEditer.h Global.h ext_lib.h
	gcc -c TrackDataEditer.c

#Compile WaveSynthesizer.c
WaveSynthesizer.o : WaveSynthesizer.c WaveSynthesizer.h Global.h ext_lib.h
	gcc -c WaveSynthesizer.c


#clean
clean :
	rm main.o CommandInterpreter.o ext_lib.o File_IO.o TrackDataEditer.o WaveSynthesizer.o

