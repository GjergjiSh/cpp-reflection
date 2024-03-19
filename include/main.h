#include <iostream>

class Main
{
public:
    Main() { m_message = "Hello, world!\n"; };
    ~Main() = default;
    int8_t run() { std::cout << m_message << std::endl; return 0; };

private:
    std::string m_message;
};