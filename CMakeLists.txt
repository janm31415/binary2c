project(binary2c)

set(SRCS
main.c
)

add_executable(binary2c ${SRCS})
source_group("Source Files" FILES ${SRCS})