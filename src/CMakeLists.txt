cmake_minimum_required(VERSION 3.10)

project(black_5wk_transport_r)

set(CMAKE_CXX_STANDARD 17)  # Испльзуем C++ 17



find_package(Protobuf REQUIRED) # Команда находит пути к компилятору protoc и библиотеке libprotobuf
include_directories(${Protobuf_INCLUDE_DIRS}) # Добавляем в include path пути к библиотеке protobuf
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # В ${CMAKE_CURRENT_BINARY_DIR} будут файлы, сгенерированные компилятором protoс, путь к ним надо добавить в include path

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS working_time.proto transport_catalog.proto transport_router.proto url.proto graph.proto map_renderer.proto descriptions.proto company.proto database.proto name.proto sphere.proto svg.proto rubric.proto phone.proto address.proto ) # Здесь надо перечислить все ваши .proto-файлы
add_executable(main main.cpp sphere.cpp json.cpp requests.cpp transport_router.cpp transport_catalog.cpp map_renderer.cpp utils.cpp svg_serialize.cpp descriptions.cpp svg.cpp transport_catalog.h requests.h sphere.h svg.h test_runner.h router.h svg_serialize.h descriptions.h utils.h graph.h json.h transport_router.h map_renderer.h tests.h tests.cpp yellow_pages_private.h yellow_pages_private.cpp filters.h filters.cpp
test_descriptions.h
test_descriptions.cpp
test_filters.h
test_filters.cpp 
test_requests.h
test_requests.cpp
test_yellow_pages_private.h
test_yellow_pages_private.cpp
data_generator.h
data_generator.cpp
test_data_generator.h
test_data_generator.cpp
test_ostreams.h
test_ostreams.cpp
test_main.h
test_main.cpp
test_map.h
test_map.cpp
${PROTO_SRCS} ${PROTO_HDRS}) # Здесь надо перечислить все ваши .cpp-файлы, в том числе и сгенерированные protoc'ом
target_link_libraries(main ${Protobuf_LIBRARIES}) # компонуем наш исполняемый файл с библиотекой libprotobuf 