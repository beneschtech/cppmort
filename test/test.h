#include <iostream>
#include <stdio.h>

class outEmitter
{
	public:
		outEmitter();
		void emit_out();
		bool emit_out(const char *message)
		{
			if (!message)
				return false;
			std::cout << message << std::endl;
			return true;
		}
};

