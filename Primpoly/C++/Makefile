
CXX = g++

# 定义编译选项（例如：-Wall 表示显示所有警告）
CXXFLAGS = -Wall -g

# 定义链接选项（例如：-lpthread 如果程序使用了线程）
LDFLAGS = -lpthread

# 定义需要编译的源文件
SRC = Primpoly.cpp ppUnitTest.cpp ppPolynomial.cpp ppParser.cpp ppOperationCount.cpp ppFactor.cpp ppBigInt.cpp ppArith.cpp
# 定义生成的可执行文件
TARGET = Primpoly

# 定义对象文件
OBJ = $(SRC:.cpp=.o)

# 默认目标
all: $(TARGET)

# 链接目标
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# 编译源文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理构建文件
clean:
	rm -f $(OBJ) $(TARGET)

# 伪目标，避免与同名文件冲突
.PHONY: all clean