docker stop clion_remote_env
docker build -t clion/remote-cpp-env:0.5 -f Dockerfile .
docker run -ti --sysctl net.core.somaxconn=10000 --rm -d --cap-add sys_ptrace -p127.0.0.1:2222:22 -p127.0.0.1:8080:8080 --name clion_remote_env clion/remote-cpp-env:0.5
