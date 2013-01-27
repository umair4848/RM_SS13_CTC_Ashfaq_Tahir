Project Noise Reduction Grid Based Sound Localization 
Description: 
This project implement a sound localization algorithm with four microphone and one M-Audio fast track.  The project works with voice command, "johnny yes" begin 3 sec recording , please talk loud or use a small speaker for test. Some other speech commands can also be use for example, "johnny stop" stop all the performance. The programm works on real time and you can test several times the algorithm starts with "johnny yes".The noise reduction helps the localization algorithm choose the most proper frame, the grid based method improve the localization error . Final result will be displayed on the screen right after recording .

Setup Description:
1. Recording part:
One M-Audio fast track driver needs to be installed in ur computer, next the DirectSound library needs also to be installed. Strongly recommand use to already builed .vcproj to verify. If anything wrong , contact me please.
2. Speech Command part:
The Microsoft Speech SDK 5.1 needs to be installed in ur system. Plz build the .xml file first , notice to change the project path in the custum build section for the .xml file.
3. Algorithm parts:
One fftw library has been installed for fft and ifft usage in the program. More Over, gsl library has also been installed to behave spline interpolation and sort.

Code description:
1. Recording part:
The following file work for real time M-audio recording: all header files and .cpp files in the include folder,RtAudio.h, RtAudio.cpp,RtError.h. Most important, the jin_record_paper.h is a main wrap for recording, you can adjust the sampling rate with fs,recording time with t, numbers of channel with channels, the source  store the recording result with a rows(time*samples)*channels result. Notice, in order to avoid the windows delay of the soundcard driver, the program choose 2 sec data but from the 8000th sample to 4(no. of channes)*44100(sampling rate)*2(time of recording)+8000

2.Speech Command Part:
This part contains the paper_locjin_nr.xml and speech_reco_paper.h.
The xml file predifined keys words the robot can understand. The paper_locjin_nr.h file works with listening to the user, when it catch the key command it will do localize or stop all the work.

3. Noise Reduction part:
This part works with four subbranches, the segment2.h, vad.h, nr_algorithm1.h, overlapadd.h
This part begin with segment the source data from recording part, here i used hamming window for data processing. 
The noise reduction part works basically with segmentation , the decide the frame property (whether it is a speech frame or noise frame). Spectral subtract noise reduction algorithm then signal reconstruct with overlapadd and choose the roughly choose the most proper section of data for further localization use.

4.Grid Based Loclization part:
This part contains two file the Localize_alg.h and grid_base_fe.h . 
grid_base_fe.h generate grid based random points the Localize_alg.h analysis and do localization.

Parameter Description :
1. Recording part: recording time, sampling rate , number of channels can be changed
2. Speech command part: key words can be changed in the .xml file 
3. Noise reduction part: window length can be changed, but better keep the parameters now. 
4. Localization part: bstart or bend start or end of the search position can be changed, better keep with x,y to the same value. Microphone postion must be changed when make experiment, unit in meter. 