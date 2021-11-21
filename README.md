# A Dataset of Logic Bombs
## Citation
```
@article{xu2018benchmarking,
  title={Benchmarking the Capability of Symbolic Execution Tools with Logic Bombs},
  author={Xu, Hui and Zhao, Zirui and Zhou, Yangfan and Lyu, Michael R},
  journal={IEEE Transactions on Dependable and Secure Computing},
  year={2018},
  publisher={IEEE}
}
```
```
@inproceedings{xu2017concolic,
  title={Concolic execution on small-size binaries: challenges and empirical study},
  author={Xu, Hui and Zhou, Yangfan and Kang, Yu and Lyu, Michael R},
  booktitle={Dependable Systems and Networks (DSN), 2017 47th Annual IEEE/IFIP International Conference on},
  pages={181--188},
  year={2017},
  organization={IEEE}
}
```
## Project Overview
This project includes a set of small programs with logic bombs.  The logic bomb can be triggered when certain conditions are met. 
We release the dataset for benchmarking purposes.  Any dynamic testing tools (especially symbolic execution) can employ the dataset to benchmark their capabilities. 
The dataset is originally realeased with our paper:

[1] "Concolic Execution on Small-Size Binary Codes: Challenges and Empirical Study," Hui Xu, Yangfan Zhou, Yu Kang, and Michael R. Lyu, in the 47th IEEE/IFIP International Conference on Dependable Systems and Networks (DSN 2017)
 

To find out more details, please visit our [wiki](https://github.com/hxuhack/logic_bombs/wiki).

## Dependencies
- Clang [How to install Clang](https://clang.llvm.org/get_started.html)
- Python3 [How to install python](https://www.python.org/)
    - termcolor
    - psutil
- Python
    - termcolor
    - psutil
 [How to install python packages](https://packaging.python.org/tutorials/installing-packages/)
 
As angr and triton scripts were written by Python 2, you should install `termcolor` and `psutil` for both Python 2 and Python 3.
 
Besides these dependencies, you should also build the environment for your target symbolic engines.
- [How to install angr](https://docs.angr.io/INSTALL.html)
- [How to install KLEE](http://klee.github.io/getting-started/)
- [How to install Triton](https://triton.quarkslab.com/documentation/doxygen/#install_sec)


## How to run it?
First, clone our repo by using  `git clone https://github.com/hxuhack/logic_bombs.git`.

Then, compile the dependencies: `python3 compile.py -l`

To change our default compiler, please visit `config/compile.json` to modify the `CC` to your preferred compiler.

To change the test cases' root folder, please visit `config/test_settings.py` to change the 4th element of corresponding record in `switches`.

**Pay attention!** If you are using Triton, please change the Triton installation path at the end of `config/test_settings.py`

To start the test:
`python3 run_tests.py -e YourEngineName -t MaxRunningTime`
The available engine names are:
 - angr
 - angr_cpp
 - triton
 - triton_cpp
 - klee

And the typical max running time setting is 60s or 300s.

## Details of the bombs
Below we list these programs and the conditions to trigger each bomb. 

| Type                          | Case                    | Trigger Condition                                                     | Our Result                                                        |
| ----------------------------- | ----------------------- | --------------------------------------------------------------------- | ----------------------------------------------------------------- |
| Symbolic Variable Declaration | stdin_svd.c             | user inputs 7                                                         | Pass                                                              |
|                               | cpu_svd.c               | the program runs on an Intel CPU                                      | Fail (We do not support file open)                                |
|                               | time_svd.c              | current time is after Jan 1st, 2050                                   | Pass (**Force int64_t for returned timestamp**)                   |
|                               | pid_svd.c               | the process id is 4096                                                | Wrong answer (Wasm hard encodes 42 for every pid)                 |
|                               | vm_svd.c                | the program runs on an virtual machine                                | Fail (We do not support file open)                                |
|                               | web_svd.c               | if a remote website contains the string "trigger the bomb"            | Fail (Wasm standalone does not support socket communication)      |
|                               | syscall_ls_svd.c        | the number of files under a current directory is 7                    | Fail (We do not support file read)                                |
| Contextual Symbolic Value     | file_csv.c              | if argv[1] points to a file                                           | Wrong answer (We do not support file read)                        |
|                               | pid_csv.c               | if argv[1][0]-48 equals the process id%78                             | Wrong answer (Wasm hard encodes 42 for every pid)                 |
|                               | ping_csv.c              | if argv[1] points to a live IP                                        | Fail (Wasm standalone does not support socket communication)      |
|                               | syscall_csv.c           | if argv[1] is a valid Linux command                                   | Pass (**only on emcc, as memory model problem on emcc-binaryen**) |
| Covert Propagation            | file_cp.c               | data propagate via a file (expected argv[1][0]: '7')                  | Wrong answer (We do not support file read)                        |
|                               | stack_cp.c              | data propagation via direct push/pop (expected argv[1][0]: '7')       | Inapplicable (Compiler cannot compile)                            |
|                               | echo_cp.c               | data propagation via echo (expected argv[1][0]: '7')                  | Fail (We do not support file read)                                |
|                               | df2cf_cp.c              | data propagation via control flow (expected argv[1][0]: '7')          | Pass                                                              |
|                               | echofile_cp.c           | data propagation via echo and file (expected argv[1][0]: '7')         | Fail (We do not support file read)                                |
|                               | socket_cp.c             | data propagation via socket (expected argv[1][0]: '7')                | Cannot compiled                                                   |
|                               | toy_eh_cp.cpp           | data propagation via exception handling (expected argv[1][0]: '7')    | Wrong answer (We cannot handle try-throw-catch normally)          |
|                               | div0_eh_cp.cpp          | raise an exceptions when divided by 0 (expected argv[1][0]: '7')      | Wrong answer (We cannot handle try-throw-catch normally)          |
|                               | file_eh_cp.cpp          | expected argv[1]: an existed file                                     | Timeout (We cannot handle try-throw-catch normally)               |
| Symbolic Memory               | stackarray_sm_l1.c      | if stdin points to an array element (expected argv[1][0]: '4')        | Compiled                                                          |
|                               | malloc_sm_l1.c          | allocate memory with malloc (expected argv[1][0]: '7')                | Compiled                                                          |
|                               | realloc_sm_l1.c         | allocate memory with realloc (expected argv[1][0]: '7')               | Compiled                                                          |
|                               | vector_sm_l1.cpp        | with std::vector (expected argv[1][0]: '7')                           | Compiled                                                          |
|                               | list_sm_l1.cpp          | with std::list (expected argv[1][0]: '7')                             | Compiled                                                          |
|                               | stackarray_sm_l2.c      | two arrays (expected argv[1][0]: '2')                                 | Compiled                                                          |
|                               | stackarray_sm_ln.c      | several levels of arrays (expected argv[1][0]: '6')                   | Compiled                                                          |
|                               | stackoutofbound_sm_l2.c | if array index equal array size (expected argv[1][0]: '7')            | Compiled                                                          |
|                               | heapoutofbound_sm_l2.c  | if array index equals array size (expected argv[1][0]: ':')           | Compiled                                                          |
| Parallel Program              | 2thread_pp_l1.c         | two-thread program (expected argv[1][0]: '7')                         | Inapplicable (Wasm does not support thread)                       |
|                               | forkpipe_pp_l1.c        | two-process program with pipe (expected argv[1][0]: '7')              | Inapplicable (Wasm does not support thread)                       |
|                               | forkshm_pp_l1.c         | two-process program with shared memory (expected argv[1][0]: '7')     | Inapplicable (Wasm does not support thread)                       |
|                               | 2thread_pp_l2.c         | two-thread program with random result (high chance argv[1][0]: '9')   | Inapplicable (Wasm does not support thread)                       |
|                               | mthread_pp_l2.c         | multi-thread program with random result (high chance argv[1][0]: '9') | Inapplicable (Wasm does not support thread)                       |
| Floating-point Number         | float1_fp_l1.c          | expected argv[1][0]: '7'                                              | Pass                                                              |
|                               | float2_fp_l1.c          | expected argv[1][0]: '7'                                              | Pass                                                              |
|                               | float3_fp_l2.c          | expected argv[1]: "0.1"                                               | Pass                                                              |
|                               | float4_fp_l2.c          | expected argv[1]: "-0.1"                                              | Pass                                                              |
|                               | float5_fp_l2.c          | expected argv[1]: "0.41421"                                           | Pass                                                              |
| Symbolic Jump                 | funcpointers_sj_l1.c    | call a function related to stdin (expected argv[1][0]: '5')           | Pass                                                              |
|                               | jmp_sj_l1.c             | jump to an address related to stdin (expected argv[1][0]: 'U')        | Inapplicable (Wasm does not support goto)                         |
|                               | arrayjmp_sj_l2.c        | a more complex case with an array (expected argv[1][0]: '7')          | Inapplicable (Wasm does not support goto)                         |
|                               | vectorjmp_sj_l2.c       | a more complex case with an vector (expected argv[1][0]: '7')         | Inapplicable (Wasm does not support goto)                         |
| Data Overflow                 | plus_do.c               | a + 2147483640 < 0 && a > 0 (expected argv[1][0]: '8')                | Pass                                                              |
|                               | multiply_do.c           | 254748364 * a < 0 && a > 0 (expected argv[1][0]: '9')                 | Pass                                                              |
| Buffer Overflow               | stacknocrash_bo_l1.c    | expected stdin: \`python -c 'print "AAAAAAAA\x01\x00\x00\x00"'\`      | Compiled                                                          |
|                               | stack_bo_l1.c           | expected stdin: \`python -c 'print "AAAAAAAA\x01\x00\x00\x00"'\`      | Compiled                                                          |
|                               | stack_bo_l2.c           | expected stdin: TO FIGURE OUT                                         | Compiled                                                          |
|                               | heap_bo_l1.c            | expected stdin: TO FIGURE OUT                                         | Compiled                                                          |
| External Function Call        | printint_ef_l1.c        | expected argv[1][0]: '7'                                              | Pass                                                              |
|                               | printfloat_ef_l1.c      | expected argv[1][0]: '7'                                              | Pass                                                              |
|                               | atoi_ef_l2.c            | expected argv[1][0]: '7'                                              | Pass                                                              |
|                               | atof_ef_l2.c            | expected argv[1][0]: '7'                                              | Pass                                                              |
|                               | rand_ef_l2.c            | rand()%100 == 7, random result                                        | Pass                                                              |
|                               | pow_ef_l2.c             | pow(i, 2) == 49, argv[1][0] = '7'                                     | Pass                                                              |
|                               | sin_ef_l2.c             | sin(i * PI / 30) > 0.5 argv[1][0] = '6'                               | Timeout (We have to step in to `sin` which is time consuming)     |
|                               | ln_ef_l2.c              | 1.94 < log(i) && log(i) < 1.95, argv[1][0] = '7'                      | Timeout (We have to step in to `log` which is time consuming)     |
| Crypto Function               | sha_cf.c                | if sha1(i) equals to a predefined value, expected argv[1][0] = '7'    | Timeout                                                           |
|                               | aes_cf.c                | if aes(i, plaintext) equals to a ciphertext                           | Timeout                                                           |
| Loop                          | collaz_lo_l1.c          | if it loops 25 times (example argv[1][0]: '7')                        | Timeout                                                           |
|                               | 5n+1_lo_l1.c            | if it loops 25 times (example argv[1][0]: '7')                        | Timeout                                                           |
|                               | 7n+1_lo_l1.c            | if it loops 50 times (example argv[1][0]: '7')                        | Timeout                                                           |
|                               | collaz_lo_l2.c          | if it loops 986 times (example argv[1][0]: '7')                       | Timeout                                                           |
|                               | paraloop_lo_l2.c        | a loop terminated by another thread (expected argv[1][0]: '7')        | Inapplicable (Wasm does not support thread)                       |
