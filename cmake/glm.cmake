cmake_minimum_required(VERSION 3.11)

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	a532f5b1cf27d6a3c099437e6959cf7e398a0a67 # 1.0.2
)

FetchContent_MakeAvailable(glm)
