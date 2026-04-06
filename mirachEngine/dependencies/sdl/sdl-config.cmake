# Fetch and configure SDL3.
set(SDL_STATIC ON)
set(SDL_SHARED OFF)

CPMAddPackage(
        NAME sdl
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG f6864924f76e1a0b4abaefc76ae2ed22b1a8916e # v3.2.8
)