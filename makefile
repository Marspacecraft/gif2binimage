
TARGET=gif2binimage	
SRCS=binimagearray.c gif2binimage.c ./gifdec/gifdec.c 
OBJS=$(SRCS:%.c=%.o)
	

TARGET_SHOW=binimageshow
SRCS_SHOW=binimageshow.c binimagearray.c ./gifdec/gifdec.c 
OBJS_SHOW=$(SRCS_SHOW:%.c=%.o)

CFLAG= -g -O0
INCLUDE= -I /usr/local/include/SDL2 -I ./gifdec
LDD= -lSDL2main -lSDL2 -lm
CC= gcc

TESTFILE= ginisobeautiful.gif
CMD= ./$(TARGET)

SHOWFILE= binimage.bin
SHOW=./$(TARGET_SHOW)

all:$(TARGET) $(TARGET_SHOW)
	
$(TARGET):$(OBJS)
	$(CC) $(OBJS) $(CFLAG) $(LDD) -o $(TARGET) 
	
$(TARGET_SHOW):$(OBJS_SHOW)
	$(CC) $(OBJS_SHOW) $(CFLAG) $(LDD) -o $(TARGET_SHOW) 

%.o:%.c
	$(CC) -c $< $(INCLUDE) $(CFLAG) -o $@

clean:
	@rm -f $(OBJS) $(TARGET) $(OBJS_SHOW) $(TARGET_SHOW) binimage.*
	
clear:
	@rm -f $(OBJS) $(OBJS_SHOW) binimage.*

test:
	@echo $(SRCS)
	@echo $(OBJS)
	
output:
	$(CC) -c ./binimage.c
	
help:
	$(shell $(CMD))
	
exp_1:
	$(info "mode:1 msb:YES rotate:0 flip:NO")
	$(shell $(CMD) $(TESTFILE) 1 1 0 0 32 128)
exp_2:
	$(info "mode:2 msb:NO rotate:90 flip:YES")
	$(shell $(CMD) $(TESTFILE) 2 0 90 1 32 128)
exp_3:
	$(info "mode:3 msb:NO rotate:180 flip:YES")
	$(shell $(CMD) $(TESTFILE) 3 0 180 1 32 128)
exp_4:
	$(info "mode:4 msb:YES rotate:270 flip:NO")
	$(shell $(CMD) $(TESTFILE) 4 1 270 0 32 128)
exp_5:
	$(info "mode:3 msb:YES rotate:0 flip:YES")
	$(shell $(CMD) $(TESTFILE) 3 1 0 1 32 128)
exp_6:
	$(info "mode:2 msb:NO rotate:90 flip:YES")
	$(shell $(CMD) $(TESTFILE) 2 0 90 1 32 128)
exp_7:
	$(info "mode:4 msb:YES rotate:180 flip:NO")
	$(shell $(CMD) $(TESTFILE) 4 1 180 0 32 128)
exp_8:
	$(info "mode:1 msb:NO rotate:270 flip:NO")
	$(shell $(CMD) $(TESTFILE) 1 0 270 0 32 128)
	
show:
	$(shell $(SHOW) $(SHOWFILE))


.PHONY:all clean clear test help show output


