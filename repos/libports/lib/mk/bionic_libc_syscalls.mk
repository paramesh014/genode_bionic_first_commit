include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc
LOCAL_PATH := $(BIONIC_DIR)/libc/
 
CC_OPT += -fPIC
 
SRC_FILES :=  arch-x86_64/syscalls/__accept4.S   arch-x86_64/syscalls/fchmodat.S   arch-x86_64/syscalls/__getpid.S  \
              arch-x86_64/syscalls/madvise.S    arch-x86_64/syscalls/readv.S   arch-x86_64/syscalls/setitimer.S \
              arch-x86_64/syscalls/tgkill.S   arch-x86_64/syscalls/acct.S  arch-x86_64/syscalls/fchmod.S \
              arch-x86_64/syscalls/getppid.S  arch-x86_64/syscalls/mincore.S  arch-x86_64/syscalls/__reboot.S \
              arch-x86_64/syscalls/__arch_prctl.S  arch-x86_64/syscalls/bind.S    arch-x86_64/syscalls/__brk.S  \
              arch-x86_64/syscalls/capget.S    arch-x86_64/syscalls/capset.S     arch-x86_64/syscalls/chdir.S   \
              arch-x86_64/syscalls/chroot.S    arch-x86_64/syscalls/clock_getres.S   arch-x86_64/syscalls/__clock_gettime.S \
              arch-x86_64/syscalls/clock_nanosleep.S   arch-x86_64/syscalls/clock_settime.S   arch-x86_64/syscalls/close.S  \
              arch-x86_64/syscalls/__connect.S  arch-x86_64/syscalls/delete_module.S   \
               arch-x86_64/syscalls/dup.S arch-x86_64/syscalls/dup3.S  \
              arch-x86_64/syscalls/epoll_create1.S  arch-x86_64/syscalls/epoll_ctl.S  arch-x86_64/syscalls/__epoll_pwait.S \
              arch-x86_64/syscalls/eventfd.S   arch-x86_64/syscalls/execve.S \
                arch-x86_64/syscalls/__exit.S  arch-x86_64/syscalls/_exit.S \
              arch-x86_64/syscalls/faccessat.S  arch-x86_64/syscalls/fallocate.S  \
               arch-x86_64/syscalls/fchdir.S   arch-x86_64/syscalls/fchownat.S \
                arch-x86_64/syscalls/fchown.S   arch-x86_64/syscalls/fcntl.S   arch-x86_64/syscalls/fdatasync.S   \
              arch-x86_64/syscalls/fgetxattr.S   arch-x86_64/syscalls/flistxattr.S   arch-x86_64/syscalls/flock.S  \
              arch-x86_64/syscalls/fsetxattr.S   arch-x86_64/syscalls/fstat64.S    arch-x86_64/syscalls/fstatat64.S  \
               arch-x86_64/syscalls/fremovexattr.S  arch-x86_64/syscalls/fstatfs64.S  arch-x86_64/syscalls/ftruncate.S  \
              arch-x86_64/syscalls/fsync.S     arch-x86_64/syscalls/__getdents64.S  arch-x86_64/syscalls/getegid.S \
               arch-x86_64/syscalls/__getpriority.S   arch-x86_64/syscalls/getresgid.S   arch-x86_64/syscalls/getresuid.S  \
               arch-x86_64/syscalls/getrlimit.S   arch-x86_64/syscalls/getrusage.S \
                arch-x86_64/syscalls/getsid.S   arch-x86_64/syscalls/getsockopt.S \
               arch-x86_64/syscalls/getsockname.S   arch-x86_64/syscalls/__gettimeofday.S  \
                arch-x86_64/syscalls/getuid.S   arch-x86_64/syscalls/getxattr.S \
               arch-x86_64/syscalls/geteuid.S    arch-x86_64/syscalls/getgid.S  \
                 arch-x86_64/syscalls/getgroups.S    arch-x86_64/syscalls/getitimer.S \
               arch-x86_64/syscalls/getpeername.S    arch-x86_64/syscalls/getpgid.S \
                 arch-x86_64/syscalls/mkdirat.S   arch-x86_64/syscalls/setns.S  \
               arch-x86_64/syscalls/__timer_create.S     arch-x86_64/syscalls/recvfrom.S   arch-x86_64/syscalls/setpgid.S  \
               arch-x86_64/syscalls/__timer_delete.S    arch-x86_64/syscalls/mknodat.S    arch-x86_64/syscalls/recvmmsg.S  \
               arch-x86_64/syscalls/setpriority.S    arch-x86_64/syscalls/timerfd_create.S \
               arch-x86_64/syscalls/mlockall.S   arch-x86_64/syscalls/recvmsg.S \
                 arch-x86_64/syscalls/setregid.S  arch-x86_64/syscalls/timerfd_gettime.S \
                arch-x86_64/syscalls/mlock.S  arch-x86_64/syscalls/removexattr.S \
                arch-x86_64/syscalls/setresgid.S   arch-x86_64/syscalls/timerfd_settime.S \
                arch-x86_64/syscalls/mmap.S   arch-x86_64/syscalls/renameat.S \
                 arch-x86_64/syscalls/setresuid.S   arch-x86_64/syscalls/__timer_getoverrun.S \
                arch-x86_64/syscalls/mount.S  arch-x86_64/syscalls/__rt_sigaction.S \
                arch-x86_64/syscalls/setreuid.S  arch-x86_64/syscalls/__timer_gettime.S \
           arch-x86_64/syscalls/mprotect.S  arch-x86_64/syscalls/__rt_sigpending.S \
                arch-x86_64/syscalls/setrlimit.S  arch-x86_64/syscalls/__timer_settime.S \
                arch-x86_64/syscalls/mremap.S   arch-x86_64/syscalls/__rt_sigprocmask.S  \
                arch-x86_64/syscalls/setsid.S  arch-x86_64/syscalls/times.S \
                arch-x86_64/syscalls/msync.S   arch-x86_64/syscalls/__rt_sigsuspend.S \
                arch-x86_64/syscalls/setsockopt.S  arch-x86_64/syscalls/truncate.S \
                arch-x86_64/syscalls/munlockall.S  arch-x86_64/syscalls/__rt_sigtimedwait.S \
                arch-x86_64/syscalls/__set_tid_address.S \
                arch-x86_64/syscalls/umask.S   arch-x86_64/syscalls/munlock.S   arch-x86_64/syscalls/__sched_getaffinity.S \
                arch-x86_64/syscalls/settimeofday.S    arch-x86_64/syscalls/umount2.S \
                arch-x86_64/syscalls/init_module.S  arch-x86_64/syscalls/munmap.S \
                arch-x86_64/syscalls/sched_getparam.S   arch-x86_64/syscalls/setuid.S \
                arch-x86_64/syscalls/uname.S   arch-x86_64/syscalls/inotify_add_watch.S   \
                arch-x86_64/syscalls/nanosleep.S  arch-x86_64/syscalls/sched_get_priority_max.S \
                arch-x86_64/syscalls/setxattr.S   arch-x86_64/syscalls/unlinkat.S \
                arch-x86_64/syscalls/inotify_init1.S   arch-x86_64/syscalls/__openat.S \
                arch-x86_64/syscalls/sched_get_priority_min.S \
                arch-x86_64/syscalls/shutdown.S  arch-x86_64/syscalls/unshare.S \
                arch-x86_64/syscalls/__getcpu.S  arch-x86_64/syscalls/inotify_rm_watch.S \
                arch-x86_64/syscalls/personality.S      arch-x86_64/syscalls/sched_getscheduler.S \
                arch-x86_64/syscalls/sigaltstack.S   arch-x86_64/syscalls/utimensat.S  \
                arch-x86_64/syscalls/__getcwd.S   arch-x86_64/syscalls/__ioctl.S  \
                arch-x86_64/syscalls/pipe2.S    arch-x86_64/syscalls/sched_rr_get_interval.S \
                arch-x86_64/syscalls/__signalfd4.S    arch-x86_64/syscalls/vmsplice.S \
                arch-x86_64/syscalls/kill.S    arch-x86_64/syscalls/__ppoll.S  \
                 arch-x86_64/syscalls/sched_setaffinity.S  \
               arch-x86_64/syscalls/socketpair.S   arch-x86_64/syscalls/wait4.S   \
                arch-x86_64/syscalls/klogctl.S    arch-x86_64/syscalls/prctl.S \
                arch-x86_64/syscalls/sched_setparam.S   arch-x86_64/syscalls/__socket.S \
                arch-x86_64/syscalls/__waitid.S   \
                arch-x86_64/syscalls/lgetxattr.S    arch-x86_64/syscalls/pread64.S \
                   arch-x86_64/syscalls/dup3.S  arch-x86_64/syscalls/sched_setscheduler.S \
                arch-x86_64/syscalls/splice.S    arch-x86_64/syscalls/write.S \
                 arch-x86_64/syscalls/linkat.S    arch-x86_64/syscalls/prlimit64.S \
                arch-x86_64/syscalls/sched_yield.S    arch-x86_64/syscalls/statfs64.S  \
                arch-x86_64/syscalls/writev.S   arch-x86_64/syscalls/listen.S  \
                arch-x86_64/syscalls/__pselect6.S  arch-x86_64/syscalls/sendfile.S   \
                 arch-x86_64/syscalls/swapoff.S   arch-x86_64/syscalls/listxattr.S \
                arch-x86_64/syscalls/__ptrace.S   arch-x86_64/syscalls/sendmmsg.S  \
                   arch-x86_64/syscalls/swapon.S   arch-x86_64/syscalls/llistxattr.S \
                arch-x86_64/syscalls/pwrite64.S   arch-x86_64/syscalls/sendmsg.S  \
                 arch-x86_64/syscalls/symlinkat.S    arch-x86_64/syscalls/lremovexattr.S \
                arch-x86_64/syscalls/readahead.S   arch-x86_64/syscalls/sendto.S   \
                arch-x86_64/syscalls/sync.S   arch-x86_64/syscalls/lseek.S   \
                arch-x86_64/syscalls/readlinkat.S   arch-x86_64/syscalls/setgid.S \
                arch-x86_64/syscalls/sysinfo.S  arch-x86_64/syscalls/lsetxattr.S \
                arch-x86_64/syscalls/read.S    arch-x86_64/syscalls/dup3.S \
                 arch-x86_64/syscalls/setgroups.S  \
                arch-x86_64/syscalls/tee.S
                    
                                       
#LIBS += bionic_libc_x86
 
 
SRC_S = $(SRC_FILES)
#SHARED_LIB = yes


vpath %.S $(LOCAL_PATH) 
