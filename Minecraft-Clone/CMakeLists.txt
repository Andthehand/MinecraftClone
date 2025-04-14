cmake_minimum_required(VERSION 3.20)

project(Minecraft-Clone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Minecraft-Clone "src/MinecraftAPP.cpp" "src/MinecraftLayer.h" "src/MinecraftLayer.cpp" "src/Chunk.h" "src/Chunk.cpp" "src/Block.cpp" "src/ChunkRenderer.h" "src/ChunkRenderer.cpp" "src/ChunkManager.h" "src/ChunkManager.cpp" "src/GameCamera.h")

include_directories(Minecraft-Clone PRIVATE
  ${CMAKE_SOURCE_DIR}/RealEngine/src
  ${CMAKE_SOURCE_DIR}/RealEngine/vendor/ImGui
  ${CMAKE_SOURCE_DIR}/RealEngine/vendor/rapidyaml/src
  src
)

target_link_libraries(Minecraft-Clone RealEngine)

file(COPY assets DESTINATION "${CMAKE_BINARY_DIR}/Minecraft-Clone")
