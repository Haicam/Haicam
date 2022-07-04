![Haicam Logo](https://haicam.tech/app/themes/haicam/dist/images/haicam-logo-black-250.png)

## Haicam generic camera software

Haicam introduces end-to-end encryption technology to home security cameras since 2017

“With cases of privacy invasion and security breaches involving home security systems on the rise, there has to be a better solution. I think this is it.”

Founder of Haicam

More information about the project is available [on the website](https://haicam.tech).

### Support

If you have any specific questions concerning our project, feel free to [contact us](mailto:support@haicam.tech).

### Participating and Contribution

If you like what we do, and willing to intensify the development, please consider participating.

You can improve existing code and send us patches. You can add new features missing from our code.

You can help us to write a better documentation.


### Welcome to contribute to Haicam project, please follow below steps for building and development of project 

1. Fork project https://github.com/Haicam/Haicam to your account

2. After fork is done. Need to add a public SSH key to your git account to get permission to clone the repository.
    - If the ssh key is already been added. Goto "step 3" to clone the repository.

3. clone the forked project to your local machine. 
    - Run Command  "git clone <url from Repository>"

4. After cloning is successfull switch to "develop" branch.
     - Run Command "git checkout develop"
   
   - To check the current branch you are present
   - Run Command "git branch" will give the name of the current branch with *mark.

5. Install docker in your machine to bulid the project.
    - Initially Run command "docker --version" to check current version if docker is already installed in your machine.
    - To install Docker follow the below steps.
    - Check if the system is up-to-date using the following command: "sudo apt-get update"
        
        - Install Docker using the following command: "sudo apt install docker.io"
          You’ll then get a prompt asking you to choose between y/n - choose y
       
       - Install all the dependency packages using the following command:"sudo snap install docker"
     
       - Pull an image from the Docker hub using the following command: "sudo docker run hello-world"

       - Check if the docker image has been pulled and is present in your system using the following command: "sudo docker images"

       - To display all the containers pulled, use the following command: "sudo docker ps -a"

       - To check for containers in a running state, use the following command: "sudo docker ps "
      
       - You’ve just successfully installed Docker on Ubuntu!

     
6. To install required toolchain to build project in your local machine.
    - Run Command "docker pull haicam/haicam-toolchain:latest"

7. Directories for development
    - ~/toolchain #location for any third party toolchain not installed in the docker
    - ~/go # golang modules directory, leave it empty, that will be used in the docker
     - Goto the current Haicam project directory  

8. To enter into the docker build enviroment.Run haicam toolchain docker by command:"./run_docker.sh" to enter the docker build enviroment.
    - If you are in user directory run: "sudo ./run_docker.sh"          
    - The output will be "haicam@43ffc785dd6e:~/workspace"

9. Build the project for all platforms, by run command: ./build/haicam.sh
    - You may get errors if you do not setup the third party toolchain correctly.

10. Build for Linux x86_64 only by command: ./build/apps/linux-x86_64.sh
    - After bulid is successful. The following files will be generated.
    - bin/linux/x86_64/generic/haicam-app
    - bin/linux/x86_64/generic/haicam-test

11. If 9 or 10 steps are failing either tool chain is not installed properly or "libs" are not added.
    - In that case check "CMakeOutput.log" ,"CMakeerror.log"
    - Try to pull the latest docker image for toolchain from docker. 

12. Once the build is successful.Run all testcases for Linux x86_64, by command:"./bin/linux/x86_64/generic/haicam-test"

13. If the test cases crash, run gdb command below for debug
    - gdb ./bin/linux/x86_64/generic/haicam-test
    - (gdb) r
    - (gdb) bt full
    - You will be able to find the crash location.

14. The memory leakage can be check as follow:
     - Run the Command "valgrind ./bin/linux/x86_64/generic/haicam-test"

15. You can check one test case by giving command below (For example)
   
     - ./bin/linux/x86_64/generic/haicam-test --gtest_filter=haicam_UDPTest.udp_test
     - gdb --args ./bin/linux/x86_64/generic/haicam-test --gtest_filter=haicam_UDPTest.udp_test
     - valgrind ./bin/linux/x86_64/generic/haicam-test --gtest_filter=haicam_UDPTest.udp_test

16. Build for GM8136 by command: build/apps/gm8136-armv5.sh
   - After successfull build.The following files will be generated
     - bin/gm8136/armv5/generic/haicam-app
      - bin/gm8136/armv5/generic/haicam-test
	
 17. Run in docker by qemu emulator: 
    -To run the hiacam-app generate after build. 
      - Run Command "./bin/gm8136/armv5/generic/haicam-app.sh"
      - The output will be "Hello,Haicam!"
    
         - To run the hiacam-test generate after build:"./bin/gm8136/armv5/generic/haicam-test.sh"
         - This haicam-test.sh consists of Test cases. 
         - If all are pass the project build is successfully done.

18. To debug remotley use gdb debug in docker.
     - Run Command "./bin/gm8136/armv5/generic/haicam-test-gdb-server.sh"
      - Then open another terminal to connect to the gdb server
      - Run command "docker exec -it haicam-docker bash"
      - The output will be "haicam@94ab16cc0682:~/workspace$"
      - Then run the command "./bin/gm8136/armv5/generic/haicam-test-gdb.sh"
      - You will be successfully eneter in to remote debug session.
      -The output will be: For help, type "help".
        - Type "apropos word" to search for commands related to "word"...
        - Reading symbols from ./bin/gm8136/armv5/generic/haicam-test...
        - (gdb)
     

19. Do the development in the directories below.
    - The header files are present in "include" folder.
    - The source codes are present in "src" folder. 
    - The test codes are present in "test" folder.

20. Always do the development in "develop" branch. 

21. After that run your test cases and do memory leak check. send the code for review.

22. Push only reviewed code into master, and do pull reqest to contribute your code.

### Thank You!
