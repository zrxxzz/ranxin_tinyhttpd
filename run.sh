#! /bin/bash
if [ ! -e "CMakeLists.txt" ]; then
echo "CMakeLists.txt is not found"
else 
    if [ ! -d "build" ]; then
        mkdir build
    else
        cd build && rm -rf *
        cmake ..
        if [ $? -eq 0 ]; then 
            echo "Cmake success!"
            make
            if [ $? -eq 0 ]; then
                echo "Make success!"
                ./ranxin_tinyhttpd
            else 
                echo "Make failed!"
            fi
        else 
            echo "Cmake failed!"
        fi
    fi
fi