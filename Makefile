CC := clang++
SRCDIR := src
INCLUDEDIR := include
UTIL := util
BUILDDIR := build
CFLAGS := -Wall -std=c++0x -g
LIBS := `pkg-config opencv --libs` -lm
TARGET := bin/process_image
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

DEPS := $(OBJECTS:.o=.deps)
 
$(TARGET): $(OBJECTS)
	@echo " Linking..."; $(CC) $^ -o $(TARGET) $(LIBS)
 
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) 
	@mkdir -p $(BUILDDIR)
	@echo " CC $<"; $(CC) $(CFLAGS) -I$(INCLUDEDIR) -I$(UTIL) -MD -MF $(@:.o=.deps) -o $@ -c $< $(LIBS)
 
clean:
	@echo " Cleaning..."; $(RM) -r $(BUILDDIR) $(TARGET)


-include $(DEPS)

.PHONY: clean

project3: experiment1 experiment2 experiment3

experiment1:
	./bin/process_image 1 a
	./bin/process_image 1 b
	./bin/process_image 1 c
	./bin/process_image 1 d
	./bin/process_image 1 e ./bin/assets/rect.dat

experiment2:
	./bin/process_image 2 a
	./bin/process_image 2 b
	./bin/process_image 2 c

experiment3:
	./bin/process_image 3


project2: correlation smoothing median averaging unsharpening sharpening

correlation:
	./bin/process_image ./bin/assets/Image.pgm corr 1 ./bin/assets/Pattern.pgm
	./bin/process_image ./bin/assets/Image2.pgm corr2 1 ./bin/assets/Pattern2.pgm

smoothing:
	./bin/process_image ./bin/assets/lenna.pgm lenna15smth 2 15
	./bin/process_image ./bin/assets/lenna.pgm lenna7smth 2 7
	./bin/process_image ./bin/assets/f_16.pgm f_16-15smth 2 15
	./bin/process_image ./bin/assets/f_16.pgm f_16-7smth 2 7

median:
	./bin/process_image ./bin/assets/lenna.pgm lenna15med30 3 15 30
	./bin/process_image ./bin/assets/lenna.pgm lenna7med30 3 7 30
	./bin/process_image ./bin/assets/lenna.pgm lenna15med50 3 15 50
	./bin/process_image ./bin/assets/lenna.pgm lenna7med50 3 7 50
	./bin/process_image ./bin/assets/boat.pgm boat7med30 3 7 30
	./bin/process_image ./bin/assets/boat.pgm boat15med30 3 15 30
	./bin/process_image ./bin/assets/boat.pgm boat7med50 3 7 50
	./bin/process_image ./bin/assets/boat.pgm boat15med50 3 15 50

averaging:
	./bin/process_image ./bin/assets/lenna.pgm lenna15avg 4 15 0
	./bin/process_image ./bin/assets/lenna.pgm lenna7avg 4 7 0
	./bin/process_image ./bin/assets/f_16.pgm f_16-15avg 4 15 0
	./bin/process_image ./bin/assets/f_16.pgm f_16-7avg 4 7 0
	./bin/process_image ./bin/assets/lenna.pgm lenna15avg30 4 15 30
	./bin/process_image ./bin/assets/lenna.pgm lenna7avg30 4 7 30
	./bin/process_image ./bin/assets/lenna.pgm lenna15avg50 4 15 50
	./bin/process_image ./bin/assets/lenna.pgm lenna7avg50 4 7 50
	./bin/process_image ./bin/assets/boat.pgm boat7avg30 4 7 30
	./bin/process_image ./bin/assets/boat.pgm boat15avg30 4 15 30
	./bin/process_image ./bin/assets/boat.pgm boat7avg50 4 7 50
	./bin/process_image ./bin/assets/boat.pgm boat15avg50 4 15 50

sharpening:
	./bin/process_image ./bin/assets/lenna.pgm lennasobel 5 0
	./bin/process_image ./bin/assets/lenna.pgm lennaprewitt 5 1
	./bin/process_image ./bin/assets/lenna.pgm lennalaplace 5 2
	./bin/process_image ./bin/assets/f_16.pgm f_16sobel 5 0
	./bin/process_image ./bin/assets/f_16.pgm f_16prewitt 5 1
	./bin/process_image ./bin/assets/f_16.pgm f_16laplace 5 2
	
unsharpening:
	./bin/process_image ./bin/assets/lenna.pgm lennaunshrpA0-5 6 0.5
	./bin/process_image ./bin/assets/lenna.pgm lennaunshrpA1 6 1
	./bin/process_image ./bin/assets/lenna.pgm lennaunshrpA1-5 6 1.5
	./bin/process_image ./bin/assets/lenna.pgm lennaunshrpA2 6 2
	./bin/process_image ./bin/assets/lenna.pgm lennaunshrpA2-5 6 2.5
	./bin/process_image ./bin/assets/lenna.pgm lennaunshrpA4 6 4
	./bin/process_image ./bin/assets/f_16.pgm f_16unshrpA0-5 6 0.5
	./bin/process_image ./bin/assets/f_16.pgm f_16unshrpA1 6 1
	./bin/process_image ./bin/assets/f_16.pgm f_16unshrpA1-5 6 1.5
	./bin/process_image ./bin/assets/f_16.pgm f_16unshrpA2 6 2
	./bin/process_image ./bin/assets/f_16.pgm f_16unshrpA2-5 6 2.5
	./bin/process_image ./bin/assets/f_16.pgm f_16unshrpA3 6 3
	./bin/process_image ./bin/assets/f_16.pgm f_16unshrpA4 6 4
	

project1: quantization equalization specification interpolation

quantization:
	./bin/process_image ./bin/assets/lenna.pgm lenna 1 1
	./bin/process_image ./bin/assets/lenna.pgm lenna 1 3
	./bin/process_image ./bin/assets/lenna.pgm lenna 1 5
	./bin/process_image ./bin/assets/lenna.pgm lenna 1 7
	./bin/process_image ./bin/assets/peppers.pgm peppers 1 1
	./bin/process_image ./bin/assets/peppers.pgm peppers 1 3
	./bin/process_image ./bin/assets/peppers.pgm peppers 1 5
	./bin/process_image ./bin/assets/peppers.pgm peppers 1 7

equalization:
	./bin/process_image ./bin/assets/boat.pgm boat 2
	./bin/process_image ./bin/assets/f_16.pgm f_16 2

specification:
	./bin/process_image ./bin/assets/boat.pgm boatspf_16 3 ./bin/assets/f_16.pgm 
	./bin/process_image ./bin/assets/f_16.pgm spf_16peppers 3 ./bin/assets/peppers.pgm 

interpolation:
	./bin/process_image ./bin/assets/lenna.pgm lennasmpl2 4 1 2 2
	./bin/process_image ./bin/assets/lenna.pgm lennasmpl2 4 2 2 2
	./bin/process_image ./bin/assets/lenna.pgm lennasmpl2 4 3 2 2
	./bin/process_image ./bin/assets/lenna.pgm lennasmpl4 4 1 4 4
	./bin/process_image ./bin/assets/lenna.pgm lennasmpl4 4 2 4 4
	./bin/process_image ./bin/assets/lenna.pgm lennasmpl4 4 3 4 4
 

