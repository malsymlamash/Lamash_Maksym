cmake_minimum_required(VERSION 3.30)
project(RL_GAME)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.1
)
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_USE_STATIC_STD_LIBS OFF)
set(BUILD_SHARED_LIBS ON)

FetchContent_MakeAvailable(SFML)


add_executable(RL_GAME main.cpp
        EnvironmentState.cpp
        EnvironmentState.h
        EnvironmentGUI.cpp
        EnvironmentGUI.h
        IAgent.h
        EnvUtils.cpp
        EnvUtils.h
        Perc_Critic.cpp
        Perc_Critic.h
        Perc_Sector.cpp
        Perc_Sector.h
        Perc_XY_coo.cpp
        Perc_XY_coo.h
        Perc_Vector.cpp
        Perc_Vector.h
        Net_Victim.cpp
        Net_Victim.h
        Net_Hunter.cpp
        Net_Hunter.h
        SimulationUtils.cpp
        SimulationUtils.h
        NeuroCore.h
        Trainer.cpp
        Trainer.h
        GUI.cpp
        GUI.h
)

target_link_libraries(RL_GAME PRIVATE sfml-graphics sfml-window sfml-system)
