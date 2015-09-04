#------------------------------------------------------------------------------
# Dr. Juan Gonzalez-Gomez. July 2010
# juan@iearobotics.com                          
#------------------------------------------------------------------------------
# GPL LICENSE                                                               
#------------------------------------------------------------------------------

CC = g++
CPPFLAGS =  -Iserial

#-- Example: send_receive
NAME1 = send_receive
DEP1 = $(NAME1).o 

all: $(NAME1) 

#---------------------------
#-- Example: send_receive
#---------------------------
$(NAME1): $(DEP1) 
	$(CC) -o $@ $< -lserial
  
clean::
	  rm -f $(NAME1) $(DEP1) *.bak *~

