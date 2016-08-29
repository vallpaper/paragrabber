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
mkdir download
```

## Arguments
-u URL = URL adress to download [default: meetangee.com]

-d DIR = local dir where to downloaded files are stored
	 - must exist! (if not, program exits) [default: download/]

-t NUM = number of threads to run

## Dependencies
```
libcurl-openssl-dev 7.47.0
zlib1g-dev
pthreads
```
