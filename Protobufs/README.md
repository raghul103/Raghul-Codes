# Protocol-buffers

### Generating Protobuf files for python:

To generate protobuf file, run ```protoc --python_out=. file_name.proto``` in the terminal of the directory where the proto file is present.

### Generating Protobuf files for .ino files:
   1. Clone nanopb repository ```git clone https://github.com/nanopb/nanopb```
   2. Place your .proto file in generator-bin directory.
   3. On running ```protoc --nanopb_out=. file_name.proto``` in the terminal of this directory, it will generate file_name.h and file_name.c files.
   4. Include file_name.h and file_name.c along with the necessary nanopb header files required for your project.
  

