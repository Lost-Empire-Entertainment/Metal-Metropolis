//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>

#include "core_utils.hpp"

namespace MetalMetropolis::Test
{
    using std::cout;

    enum class AccessViolationType : u8
    {
        A_INVALID = 0u,

        A_WRITE = 1u,
        A_READ = 2u,
        A_EXECUTE = 3u
    };

    class Crash
    {
    public:
        static void Crash_AccessViolation(AccessViolationType type)
        {
            switch (type)
            {
            default:
            //Attempted to read invalid memory
            case AccessViolationType::A_READ:
            {
                int* crashPtr = nullptr;
                int _ = *crashPtr;
            }
            //Attempted to write to invalid memory
            case AccessViolationType::A_WRITE:
            {
                volatile int* crashPtr = nullptr;
                *crashPtr = 42;

                break;
            }
            //Attempted to execute invalid memory
            case AccessViolationType::A_EXECUTE:
            {
                void (*crashFunc)() = nullptr;
                crashFunc();
            }
            }
        }

        static void Crash_StackOverflow()
        {
            for (int i = 0; i < 1000; i++)
            {
                void* space = alloca(1024 * 1024);

                cout << "Allocated stack frame at: " << space << "\n";
            }
        }

        static void Crash_DivideByZero()
        {
            //does not trigger correctly in MINSIZEREL

            int zero = time(nullptr) % 1;
            int result = 42 / zero;

            cout << result << "\n";
        }  
    };
}