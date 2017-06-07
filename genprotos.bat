@echo off
SET ORIGINAL=%CD%
cd .\hapis\protos\
..\..\Libraries\protobuf-2.5.0\protoc.exe main.proto --cpp_out=.
cd %ORIGINAL%