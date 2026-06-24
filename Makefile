NAME = webserv

CXX = c++
CXXFLAGS = -std=c++98

SRC = \
	main.cpp \
	src/Server/Server.cpp \
	src/Server/Core.cpp \
	src/Server/Client.cpp \
	src/request_parser/httpRequestParser.cpp \
	src/request_handler/HttpResponse.cpp \
	src/methods/Get.cpp \
	src/methods/Post.cpp \
	src/methods/Delete.cpp \
	src/config_file_parser/ConfigParser.cpp \
	src/config_file_parser/Lexer.cpp

OBJ = $(SRC:.cpp=.o)

RM = rm -f

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
