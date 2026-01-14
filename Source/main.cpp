/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2022 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "game.h"
#include "Constants.h"
#include "Window.h"
#include "Errors.h"
#include <print>

int main()
{
    try {
        Window window(Constant::Window::Config);
        Game game{ State::STARTSCREEN };

        while (!WindowShouldClose())
        {
            try {
                game.HandleInput();
                game.Update();

                BeginDrawing();
                ClearBackground(BLACK);
                game.Render();
                EndDrawing();
            }
            catch (const Errors::GameError& e) {
                std::print(stderr, "Error during game loop: {}\n", e.what());

                EndDrawing();

            }
        }

        return 0;
    }
    catch (const Errors::ResourceError& e) {
        std::print(stderr, "Fatal resource error: {}\n", e.what());
        return 1;
    }
    catch (const Errors::GameError& e) {
        std::print(stderr, "Fatal game error: {}\n", e.what());
        return 1;
    }
    catch (const std::exception& e) {
        std::print(stderr, "Fatal unexpected error: {}\n", e.what());
        return 1;
    }
    catch (...) {
        std::print(stderr, "Fatal unknown error occurred\n");
        return 1;
    }
}