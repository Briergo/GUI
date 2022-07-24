# C source files
PROJECT_CSRC 	= main.c \
				  src/serial.c \
				  src/modbusFunc.c \
				  src/modbusGet.c \
				  src/modbusRegister.c \
				  src/modbusTCP.c 
				  
# C++ source files
PROJECT_CPPSRC 	= 
# Directories to search headers in
PROJECT_INCDIR	= include
# Additional libraries
PROJECT_LIBS	=
# Compiler options
PROJECT_OPT     = -specs=nano.specs -specs=nosys.specs

# Additional .mk files are included here
