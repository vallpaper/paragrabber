# Para Grabber

This is a project to qualify for interview.

## Make and run
```shell
mkdir build
cd build
cmake ..
make
cd ../bin
./grabber
```

## Arguments
-u URL = URL adress to download [default: meetangee.com]

-d DIR = local dir where to downloaded files are stored
	 - must exist! (if not, no files are stored)

-t NUM = number of threads to run
