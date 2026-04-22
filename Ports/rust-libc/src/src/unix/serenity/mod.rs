use crate::prelude::*;

// Types derived from SerenityOS Kernel/API/POSIX/sys/types.h and LibC headers.

pub type wchar_t = i32;
pub type pthread_attr_t = *mut c_void;
pub type pthread_rwlock_t = u64;
pub type pthread_rwlockattr_t = *mut c_void;
pub type fsid_t = [c_int; 2];

pub type blkcnt_t = u64;
pub type blksize_t = u64;
pub type clock_t = u64;
pub type clockid_t = c_int;
pub type dev_t = u64;
pub type fsblkcnt_t = u64;
pub type fsfilcnt_t = u64;
pub type gid_t = u32;
pub type id_t = c_int;
pub type ino_t = u64;
pub type mode_t = u16;
pub type nfds_t = c_ulong;
pub type nlink_t = u64;
pub type off_t = i64;
pub type pid_t = i32;
pub type pthread_t = c_int;
pub type pthread_key_t = c_int;
pub type rlim_t = usize;
pub type sa_family_t = u16;
pub type sigset_t = u32;
pub type socklen_t = u32;
pub type speed_t = u32;
pub type suseconds_t = i64;
pub type tcflag_t = u32;
pub type time_t = i64;
pub type uid_t = u32;
pub type useconds_t = u32;
pub type cc_t = u8;

s_no_extra_traits! {
    pub struct dirent {
        pub d_ino: crate::ino_t,
        pub d_off: crate::off_t,
        pub d_reclen: c_ushort,
        pub d_type: c_uchar,
        pub d_name: [c_char; 256],
    }

    pub struct sockaddr_un {
        pub sun_family: crate::sa_family_t,
        pub sun_path: [c_char; 108],
    }

    pub struct sockaddr_storage {
        pub ss_family: crate::sa_family_t,
        __ss_padding: [u8; 128 - size_of::<sa_family_t>() - size_of::<c_ulong>()],
        __ss_align: c_ulong,
    }

    pub struct utsname {
        pub sysname: [c_char; 65],
        pub nodename: [c_char; 65],
        pub release: [c_char; 65],
        pub version: [c_char; 65],
        pub machine: [c_char; 65],
        pub domainname: [c_char; 65],
    }

    // From Kernel/API/POSIX/termios.h: NCCS = 32
    pub struct termios {
        pub c_iflag: crate::tcflag_t,
        pub c_oflag: crate::tcflag_t,
        pub c_cflag: crate::tcflag_t,
        pub c_lflag: crate::tcflag_t,
        pub c_cc: [crate::cc_t; 32],
        pub c_ispeed: crate::speed_t,
        pub c_ospeed: crate::speed_t,
    }

    pub struct winsize {
        pub ws_row: c_ushort,
        pub ws_col: c_ushort,
        pub ws_xpixel: c_ushort,
        pub ws_ypixel: c_ushort,
    }

    // SerenityOS uses statvfs; statfs is mapped to the same layout for compatibility
    pub struct statfs {
        pub f_type: c_long,
        pub f_bsize: c_long,
        pub f_blocks: fsblkcnt_t,
        pub f_bfree: fsblkcnt_t,
        pub f_bavail: fsblkcnt_t,
        pub f_files: fsfilcnt_t,
        pub f_ffree: fsfilcnt_t,
        pub f_fsid: fsid_t,
        pub f_namelen: c_long,
        pub f_frsize: c_long,
        pub f_flags: c_long,
        pub f_spare: [c_long; 4],
    }

    // From Kernel/API/POSIX/fcntl.h
    pub struct flock {
        pub l_type: c_short,
        pub l_whence: c_short,
        pub l_start: crate::off_t,
        pub l_len: crate::off_t,
        pub l_pid: crate::pid_t,
    }

    // From Kernel/API/POSIX/sys/types.h
    pub struct pthread_mutex_t {
        pub lock: u32,
        pub owner: crate::pthread_t,
        pub level: c_int,
        pub type_: c_int,
    }

    pub struct pthread_mutexattr_t {
        pub type_: c_int,
    }

    pub struct pthread_cond_t {
        pub mutex: *mut pthread_mutex_t,
        pub value: u32,
        pub clockid: c_int,
    }

    pub struct pthread_condattr_t {
        pub clockid: c_int,
    }

    // From LibC/semaphore.h
    pub struct sem_t {
        pub magic: u32,
        pub value: u32,
        pub flags: u8,
    }
}

s! {
    // From LibC/time.h
    pub struct tm {
        pub tm_sec: c_int,
        pub tm_min: c_int,
        pub tm_hour: c_int,
        pub tm_mday: c_int,
        pub tm_mon: c_int,
        pub tm_year: c_int,
        pub tm_wday: c_int,
        pub tm_yday: c_int,
        pub tm_isdst: c_int,
    }

    // From LibC/sys/time.h
    pub struct timezone {
        pub tz_minuteswest: c_int,
        pub tz_dsttime: c_int,
    }

    // From Kernel/API/POSIX/sys/statvfs.h
    pub struct statvfs {
        pub f_bsize: c_ulong,
        pub f_frsize: c_ulong,
        pub f_blocks: crate::fsblkcnt_t,
        pub f_bfree: crate::fsblkcnt_t,
        pub f_bavail: crate::fsblkcnt_t,
        pub f_files: crate::fsfilcnt_t,
        pub f_ffree: crate::fsfilcnt_t,
        pub f_favail: crate::fsfilcnt_t,
        pub f_fsid: c_ulong,
        pub f_flag: c_ulong,
        pub f_namemax: c_ulong,
        pub f_basetype: [c_char; 16],
    }

    // From Kernel/API/POSIX/sys/stat.h
    pub struct stat {
        pub st_dev: crate::dev_t,
        pub st_ino: crate::ino_t,
        pub st_mode: crate::mode_t,
        pub st_nlink: crate::nlink_t,
        pub st_uid: crate::uid_t,
        pub st_gid: crate::gid_t,
        pub st_rdev: crate::dev_t,
        pub st_size: crate::off_t,
        pub st_blksize: crate::blksize_t,
        pub st_blocks: crate::blkcnt_t,
        pub st_atime: crate::time_t,
        pub st_atime_nsec: i64,
        pub st_mtime: crate::time_t,
        pub st_mtime_nsec: i64,
        pub st_ctime: crate::time_t,
        pub st_ctime_nsec: i64,
    }

    pub struct addrinfo {
        pub ai_flags: c_int,
        pub ai_family: c_int,
        pub ai_socktype: c_int,
        pub ai_protocol: c_int,
        pub ai_addrlen: crate::socklen_t,
        pub ai_canonname: *mut c_char,
        pub ai_addr: *mut crate::sockaddr,
        pub ai_next: *mut crate::addrinfo,
    }

    pub struct sockaddr_in {
        pub sin_family: crate::sa_family_t,
        pub sin_port: crate::in_port_t,
        pub sin_addr: crate::in_addr,
        pub sin_zero: [c_char; 8],
    }

    pub struct sockaddr_in6 {
        pub sin6_family: crate::sa_family_t,
        pub sin6_port: crate::in_port_t,
        pub sin6_flowinfo: u32,
        pub sin6_addr: crate::in6_addr,
        pub sin6_scope_id: u32,
    }

    pub struct fd_set {
        fds_bits: [c_ulong; 16],
    }

    pub struct in_addr {
        pub s_addr: crate::in_addr_t,
    }

    pub struct ip_mreq {
        pub imr_multiaddr: crate::in_addr,
        pub imr_interface: crate::in_addr,
    }

    pub struct msghdr {
        pub msg_name: *mut c_void,
        pub msg_namelen: crate::socklen_t,
        pub msg_iov: *mut crate::iovec,
        pub msg_iovlen: size_t,
        pub msg_control: *mut c_void,
        pub msg_controllen: size_t,
        pub msg_flags: c_int,
    }

    pub struct cmsghdr {
        pub cmsg_len: size_t,
        pub cmsg_level: c_int,
        pub cmsg_type: c_int,
    }

    pub struct lconv {
        pub currency_symbol: *const c_char,
        pub decimal_point: *const c_char,
        pub frac_digits: c_char,
        pub grouping: *const c_char,
        pub int_curr_symbol: *const c_char,
        pub int_frac_digits: c_char,
        pub mon_decimal_point: *const c_char,
        pub mon_grouping: *const c_char,
        pub mon_thousands_sep: *const c_char,
        pub negative_sign: *const c_char,
        pub n_cs_precedes: c_char,
        pub n_sep_by_space: c_char,
        pub n_sign_posn: c_char,
        pub positive_sign: *const c_char,
        pub p_cs_precedes: c_char,
        pub p_sep_by_space: c_char,
        pub p_sign_posn: c_char,
        pub thousands_sep: *const c_char,
    }

    pub struct passwd {
        pub pw_name: *mut c_char,
        pub pw_passwd: *mut c_char,
        pub pw_uid: crate::uid_t,
        pub pw_gid: crate::gid_t,
        pub pw_gecos: *mut c_char,
        pub pw_dir: *mut c_char,
        pub pw_shell: *mut c_char,
    }

    pub struct sigaction {
        pub sa_sigaction: crate::sighandler_t,
        pub sa_mask: crate::sigset_t,
        pub sa_flags: c_int,
        pub sa_restorer: Option<extern "C" fn()>,
    }

    pub struct siginfo_t {
        pub si_signo: c_int,
        pub si_errno: c_int,
        pub si_code: c_int,
        pub si_pid: crate::pid_t,
        pub si_uid: crate::uid_t,
        _pad: [u8; 128 - 5 * size_of::<c_int>() - size_of::<uid_t>()],
    }

    pub struct sockaddr {
        pub sa_family: crate::sa_family_t,
        pub sa_data: [c_char; 14],
    }

    pub struct Dl_info {
        pub dli_fname: *const c_char,
        pub dli_fbase: *mut c_void,
        pub dli_sname: *const c_char,
        pub dli_saddr: *mut c_void,
    }
}

// Errno constants from Kernel/API/POSIX/errno.h (sequential enum starting at 0)
pub const EPERM: c_int = 1;
pub const ENOENT: c_int = 2;
pub const ESRCH: c_int = 3;
pub const EINTR: c_int = 4;
pub const EIO: c_int = 5;
pub const ENXIO: c_int = 6;
pub const E2BIG: c_int = 7;
pub const ENOEXEC: c_int = 8;
pub const EBADF: c_int = 9;
pub const ECHILD: c_int = 10;
pub const EAGAIN: c_int = 11;
pub const ENOMEM: c_int = 12;
pub const EACCES: c_int = 13;
pub const EFAULT: c_int = 14;
pub const ENOTBLK: c_int = 15;
pub const EBUSY: c_int = 16;
pub const EEXIST: c_int = 17;
pub const EXDEV: c_int = 18;
pub const ENODEV: c_int = 19;
pub const ENOTDIR: c_int = 20;
pub const EISDIR: c_int = 21;
pub const EINVAL: c_int = 22;
pub const ENFILE: c_int = 23;
pub const EMFILE: c_int = 24;
pub const ENOTTY: c_int = 25;
pub const ETXTBSY: c_int = 26;
pub const EFBIG: c_int = 27;
pub const ENOSPC: c_int = 28;
pub const ESPIPE: c_int = 29;
pub const EROFS: c_int = 30;
pub const EMLINK: c_int = 31;
pub const EPIPE: c_int = 32;
pub const ERANGE: c_int = 33;
pub const ENAMETOOLONG: c_int = 34;
pub const ELOOP: c_int = 35;
pub const EOVERFLOW: c_int = 36;
pub const EOPNOTSUPP: c_int = 37;
pub const ENOSYS: c_int = 38;
pub const ENOTIMPL: c_int = 39;
pub const EAFNOSUPPORT: c_int = 40;
pub const ENOTSOCK: c_int = 41;
pub const EADDRINUSE: c_int = 42;
pub const ENOTEMPTY: c_int = 43;
pub const EDOM: c_int = 44;
pub const ECONNREFUSED: c_int = 45;
pub const EHOSTDOWN: c_int = 46;
pub const EADDRNOTAVAIL: c_int = 47;
pub const EISCONN: c_int = 48;
pub const ECONNABORTED: c_int = 49;
pub const EALREADY: c_int = 50;
pub const ECONNRESET: c_int = 51;
pub const EDESTADDRREQ: c_int = 52;
pub const EHOSTUNREACH: c_int = 53;
pub const EILSEQ: c_int = 54;
pub const EMSGSIZE: c_int = 55;
pub const ENETDOWN: c_int = 56;
pub const ENETUNREACH: c_int = 57;
pub const ENETRESET: c_int = 58;
pub const ENOBUFS: c_int = 59;
pub const ENOLCK: c_int = 60;
pub const ENOMSG: c_int = 61;
pub const ENOPROTOOPT: c_int = 62;
pub const ENOTCONN: c_int = 63;
pub const ESHUTDOWN: c_int = 64;
pub const ETOOMANYREFS: c_int = 65;
pub const ESOCKTNOSUPPORT: c_int = 66;
pub const EPROTONOSUPPORT: c_int = 67;
pub const EDEADLK: c_int = 68;
pub const ETIMEDOUT: c_int = 69;
pub const EPROTOTYPE: c_int = 70;
pub const EINPROGRESS: c_int = 71;
pub const ENOTHREAD: c_int = 72;
pub const EPROTO: c_int = 73;
pub const ENOTSUP: c_int = 74;
pub const EPFNOSUPPORT: c_int = 75;
pub const EDIRINTOSELF: c_int = 76;
pub const EDQUOT: c_int = 77;
pub const ENOTRECOVERABLE: c_int = 78;
pub const ECANCELED: c_int = 79;
pub const EPROMISEVIOLATION: c_int = 80;
pub const ESTALE: c_int = 81;
pub const EWOULDBLOCK: c_int = EAGAIN;

// File I/O flags from Kernel/API/POSIX/fcntl.h
pub const O_RDONLY: c_int = 1;
pub const O_WRONLY: c_int = 2;
pub const O_RDWR: c_int = 3;
pub const O_ACCMODE: c_int = 3;
pub const O_CREAT: c_int = 8;
pub const O_EXCL: c_int = 16;
pub const O_NOCTTY: c_int = 32;
pub const O_TRUNC: c_int = 64;
pub const O_APPEND: c_int = 128;
pub const O_NONBLOCK: c_int = 256;
pub const O_DIRECTORY: c_int = 512;
pub const O_NOFOLLOW: c_int = 1024;
pub const O_CLOEXEC: c_int = 2048;
pub const O_DIRECT: c_int = 4096;
pub const O_SYNC: c_int = 8192;
pub const F_DUPFD: c_int = 0;
pub const F_GETFD: c_int = 1;
pub const F_SETFD: c_int = 2;
pub const F_GETFL: c_int = 3;
pub const F_SETFL: c_int = 4;
pub const F_GETLK: c_int = 6;
pub const F_SETLK: c_int = 7;
pub const F_SETLKW: c_int = 8;
pub const F_DUPFD_CLOEXEC: c_int = 9;
pub const FD_CLOEXEC: c_int = 1;
pub const F_RDLCK: c_short = 0;
pub const F_WRLCK: c_short = 1;
pub const F_UNLCK: c_short = 2;

// File permission bits from Kernel/API/POSIX/sys/stat.h
pub const S_IFMT: crate::mode_t = 0o170000;
pub const S_IFDIR: crate::mode_t = 0o040000;
pub const S_IFCHR: crate::mode_t = 0o020000;
pub const S_IFBLK: crate::mode_t = 0o060000;
pub const S_IFREG: crate::mode_t = 0o100000;
pub const S_IFIFO: crate::mode_t = 0o010000;
pub const S_IFLNK: crate::mode_t = 0o120000;
pub const S_IFSOCK: crate::mode_t = 0o140000;
pub const S_IRUSR: crate::mode_t = 0o400;
pub const S_IWUSR: crate::mode_t = 0o200;
pub const S_IXUSR: crate::mode_t = 0o100;
pub const S_IRWXU: crate::mode_t = 0o700;
pub const S_IRGRP: crate::mode_t = 0o040;
pub const S_IWGRP: crate::mode_t = 0o020;
pub const S_IXGRP: crate::mode_t = 0o010;
pub const S_IRWXG: crate::mode_t = 0o070;
pub const S_IROTH: crate::mode_t = 0o004;
pub const S_IWOTH: crate::mode_t = 0o002;
pub const S_IXOTH: crate::mode_t = 0o001;
pub const S_IRWXO: crate::mode_t = 0o007;
pub const S_ISUID: crate::mode_t = 0o4000;
pub const S_ISGID: crate::mode_t = 0o2000;
pub const S_ISVTX: crate::mode_t = 0o1000;

// AT_* constants from Kernel/API/POSIX/fcntl.h
pub const AT_FDCWD: c_int = -100;
pub const AT_SYMLINK_NOFOLLOW: c_int = 0x100;
pub const AT_REMOVEDIR: c_int = 0x200;
pub const AT_EACCESS: c_int = 0x400;
pub const AT_SYMLINK_FOLLOW: c_int = 0x800;
pub const UTIME_OMIT: c_long = -1;
pub const UTIME_NOW: c_long = -2;

// ioctl constants from Kernel/API/Ioctl.h (sequential enum starting at 0)
pub const TIOCSWINSZ: c_ulong = 11;
pub const FIONBIO: c_ulong = 47;
pub const FIONREAD: c_ulong = 48;
pub const FIOCLEX: c_ulong = 49;
pub const TIOCGWINSZ: c_ulong = 7;
// TIOCEXCL/TIOCNXCL not in SerenityOS; use placeholder values
pub const TIOCEXCL: c_ulong = 200;
pub const TIOCNXCL: c_ulong = 201;

// File locking constants from LibC/sys/file.h
pub const LOCK_SH: c_int = 0;
pub const LOCK_EX: c_int = 1;
pub const LOCK_UN: c_int = 2;
pub const LOCK_NB: c_int = 1 << 2;

// File flags from Kernel/API/POSIX/fcntl.h
// O_ASYNC and O_DSYNC not in SerenityOS; use harmless placeholder values
pub const O_ASYNC: c_int = 0;
pub const O_DSYNC: c_int = 0;

// posix_fadvise constants from LibC/fcntl.h
pub const POSIX_FADV_NORMAL: c_int = 3;
pub const POSIX_FADV_RANDOM: c_int = 4;
pub const POSIX_FADV_SEQUENTIAL: c_int = 5;
pub const POSIX_FADV_WILLNEED: c_int = 6;
pub const POSIX_FADV_DONTNEED: c_int = 1;
pub const POSIX_FADV_NOREUSE: c_int = 2;

// statfs/statvfs flags (POSIX)
pub const ST_RDONLY: c_ulong = 1;
pub const ST_NOSUID: c_ulong = 2;

pub const RTLD_DEFAULT: *mut c_void = core::ptr::null_mut();
pub const RTLD_NEXT: *mut c_void = -1isize as *mut c_void;

// fallocate flags (Linux-specific; SerenityOS has posix_fallocate only)
pub const FALLOC_FL_KEEP_SIZE: c_int = 0x01;
pub const FALLOC_FL_PUNCH_HOLE: c_int = 0x02;
pub const FALLOC_FL_NO_HIDE_STALE: c_int = 0x04;
pub const FALLOC_FL_COLLAPSE_RANGE: c_int = 0x08;
pub const FALLOC_FL_ZERO_RANGE: c_int = 0x10;
pub const FALLOC_FL_INSERT_RANGE: c_int = 0x20;
pub const FALLOC_FL_UNSHARE_RANGE: c_int = 0x40;

// Linux-specific errno codes not present in SerenityOS; assigned out-of-range
// values so they never match real errno values but allow rustix to compile.
pub const EADV: c_int = 200;
pub const EBADE: c_int = 201;
pub const EBADFD: c_int = 202;
pub const EBADMSG: c_int = 203;
pub const EBADR: c_int = 204;
pub const EBADRQC: c_int = 205;
pub const EBADSLT: c_int = 206;
pub const EBFONT: c_int = 207;
pub const ECHRNG: c_int = 208;
pub const ECOMM: c_int = 209;
pub const EDEADLOCK: c_int = 210;
pub const EDOTDOT: c_int = 211;
pub const EHWPOISON: c_int = 212;
pub const EIDRM: c_int = 213;
pub const EISNAM: c_int = 214;
pub const EKEYEXPIRED: c_int = 215;
pub const EKEYREJECTED: c_int = 216;
pub const EKEYREVOKED: c_int = 217;
pub const EL2HLT: c_int = 218;
pub const EL2NSYNC: c_int = 219;
pub const EL3HLT: c_int = 220;
pub const EL3RST: c_int = 221;
pub const ELIBACC: c_int = 222;
pub const ELIBBAD: c_int = 223;
pub const ELIBEXEC: c_int = 224;
pub const ELIBMAX: c_int = 225;
pub const ELIBSCN: c_int = 226;
pub const ELNRNG: c_int = 227;
pub const EMEDIUMTYPE: c_int = 228;
pub const EMULTIHOP: c_int = 229;
pub const ENAVAIL: c_int = 230;
pub const ENOANO: c_int = 231;
pub const ENOCSI: c_int = 232;
pub const ENODATA: c_int = 233;
pub const ENOKEY: c_int = 234;
pub const ENOLINK: c_int = 235;
pub const ENOMEDIUM: c_int = 236;
pub const ENONET: c_int = 237;
pub const ENOPKG: c_int = 238;
pub const ENOSR: c_int = 239;
pub const ENOSTR: c_int = 240;
pub const ENOTNAM: c_int = 241;
pub const ENOTUNIQ: c_int = 242;
pub const EOWNERDEAD: c_int = 243;
pub const EREMCHG: c_int = 244;
pub const EREMOTE: c_int = 245;
pub const EREMOTEIO: c_int = 246;
pub const ERESTART: c_int = 247;
pub const ERFKILL: c_int = 248;
pub const ESRMNT: c_int = 249;
pub const ESTRPIPE: c_int = 250;
pub const ETIME: c_int = 251;
pub const EUCLEAN: c_int = 252;
pub const EUNATCH: c_int = 253;
pub const EUSERS: c_int = 254;
pub const EXFULL: c_int = 255;

// Socket family constants from Kernel/API/POSIX/sys/socket.h
pub const AF_LOCAL: c_int = 1;
pub const AF_UNIX: c_int = AF_LOCAL;
pub const AF_INET: c_int = 2;
pub const AF_INET6: c_int = 3;
pub const PF_INET: c_int = AF_INET;
pub const PF_INET6: c_int = AF_INET6;
pub const SOCK_STREAM: c_int = 1;
pub const SOCK_DGRAM: c_int = 2;
pub const SOCK_NONBLOCK: c_int = 0o4000;
pub const SOCK_CLOEXEC: c_int = 0o2000000;
pub const SOL_SOCKET: c_int = 1;
pub const SOMAXCONN: c_int = 128;
pub const IPPROTO_IP: c_int = 0;
pub const IPPROTO_TCP: c_int = 6;
pub const IPPROTO_UDP: c_int = 17;
pub const IPPROTO_IPV6: c_int = 41;

// MSG_* flags from Kernel/API/POSIX/sys/socket.h
pub const MSG_TRUNC: c_int = 0x1;
pub const MSG_CTRUNC: c_int = 0x2;
pub const MSG_PEEK: c_int = 0x4;
pub const MSG_OOB: c_int = 0x8;
pub const MSG_DONTROUTE: c_int = 0x10;
pub const MSG_WAITALL: c_int = 0x20;
pub const MSG_DONTWAIT: c_int = 0x40;
pub const MSG_NOSIGNAL: c_int = 0x80;
pub const MSG_EOR: c_int = 0x100;

// SO_* options (sequential enum starting at 0)
pub const SO_RCVTIMEO: c_int = 0;
pub const SO_SNDTIMEO: c_int = 1;
pub const SO_TYPE: c_int = 2;
pub const SO_ERROR: c_int = 3;
pub const SO_PEERCRED: c_int = 4;
pub const SO_RCVBUF: c_int = 5;
pub const SO_SNDBUF: c_int = 6;
pub const SO_DEBUG: c_int = 7;
pub const SO_REUSEADDR: c_int = 8;
pub const SO_BINDTODEVICE: c_int = 9;
pub const SO_KEEPALIVE: c_int = 10;
pub const SO_TIMESTAMP: c_int = 11;
pub const SO_BROADCAST: c_int = 12;
pub const SO_LINGER: c_int = 13;
pub const SO_ACCEPTCONN: c_int = 14;
pub const SO_DONTROUTE: c_int = 15;
pub const SO_OOBINLINE: c_int = 16;
pub const SO_SNDLOWAT: c_int = 17;
pub const SO_RCVLOWAT: c_int = 18;

// IP constants from Kernel/API/POSIX/netinet/in.h
pub const IP_TOS: c_int = 1;
pub const IP_TTL: c_int = 2;
pub const IP_MULTICAST_LOOP: c_int = 3;
pub const IP_ADD_MEMBERSHIP: c_int = 4;
pub const IP_MULTICAST_IF: c_int = 6;
pub const IP_MULTICAST_TTL: c_int = 7;
pub const IPV6_UNICAST_HOPS: c_int = 1;
pub const IPV6_MULTICAST_HOPS: c_int = 2;
pub const IPV6_MULTICAST_LOOP: c_int = 3;
pub const IPV6_MULTICAST_IF: c_int = 4;
pub const IPV6_ADD_MEMBERSHIP: c_int = 5;
pub const IPV6_DROP_MEMBERSHIP: c_int = 6;
pub const IPV6_V6ONLY: c_int = 9;
pub const IPV6_JOIN_GROUP: c_int = IPV6_ADD_MEMBERSHIP;
pub const IPV6_LEAVE_GROUP: c_int = IPV6_DROP_MEMBERSHIP;

// TCP constants from Kernel/API/POSIX/netinet/tcp.h
pub const TCP_NODELAY: c_int = 10;

// Address info error constants from LibC/netdb.h
pub const EAI_ADDRFAMILY: c_int = 1;
pub const EAI_AGAIN: c_int = 2;
pub const EAI_BADFLAGS: c_int = 3;
pub const EAI_FAIL: c_int = 4;
pub const EAI_FAMILY: c_int = 5;
pub const EAI_MEMORY: c_int = 6;
pub const EAI_NODATA: c_int = 7;
pub const EAI_NONAME: c_int = 8;
pub const EAI_SERVICE: c_int = 9;
pub const EAI_SOCKTYPE: c_int = 10;
pub const EAI_SYSTEM: c_int = 11;
pub const EAI_OVERFLOW: c_int = 12;

// sysconf constants from Kernel/API/POSIX/unistd.h (sequential enum)
pub const _SC_MONOTONIC_CLOCK: c_int = 0;
pub const _SC_NPROCESSORS_CONF: c_int = 1;
pub const _SC_NPROCESSORS_ONLN: c_int = 2;
pub const _SC_OPEN_MAX: c_int = 3;
pub const _SC_HOST_NAME_MAX: c_int = 4;
pub const _SC_TTY_NAME_MAX: c_int = 5;
pub const _SC_PAGESIZE: c_int = 6;
pub const _SC_PAGE_SIZE: c_int = _SC_PAGESIZE;
pub const _SC_GETPW_R_SIZE_MAX: c_int = 7;
pub const _SC_GETGR_R_SIZE_MAX: c_int = 8;
pub const _SC_CLK_TCK: c_int = 9;
pub const _SC_SYMLOOP_MAX: c_int = 10;
pub const _SC_MAPPED_FILES: c_int = 11;
pub const _SC_ARG_MAX: c_int = 12;
pub const _SC_IOV_MAX: c_int = 13;
pub const _SC_PHYS_PAGES: c_int = 14;

// Wait/exit constants
pub const WNOHANG: c_int = 1;
pub const EXIT_SUCCESS: c_int = 0;
pub const EXIT_FAILURE: c_int = 1;

// File seek constants from LibC/unistd.h
pub const SEEK_SET: c_int = 0;
pub const SEEK_CUR: c_int = 1;
pub const SEEK_END: c_int = 2;

// Socket shutdown constants from Kernel/API/POSIX/sys/socket.h
pub const SHUT_RD: c_int = 0;
pub const SHUT_WR: c_int = 1;
pub const SHUT_RDWR: c_int = 2;

// Poll constants from Kernel/API/POSIX/poll.h
pub const POLLIN: c_short = 1 << 0;
pub const POLLOUT: c_short = 1 << 2;
pub const POLLERR: c_short = 1 << 3;
pub const POLLHUP: c_short = 1 << 4;
pub const POLLNVAL: c_short = 1 << 5;
pub const POLLRDNORM: c_short = POLLIN;
pub const POLLWRNORM: c_short = POLLOUT;

// IP multicast - add missing drop constant
pub const IP_DROP_MEMBERSHIP: c_int = 5;

// Additional clock IDs from Kernel/API/POSIX/time.h
pub const CLOCK_REALTIME_COARSE: crate::clockid_t = 3;
pub const CLOCK_MONOTONIC_COARSE: crate::clockid_t = 4;

// pthread constants from LibC/pthread.h and bits/pthread_integration.h
pub const PTHREAD_MUTEX_NORMAL: c_int = 0;
pub const PTHREAD_MUTEX_RECURSIVE: c_int = 1;
pub const PTHREAD_MUTEX_DEFAULT: c_int = PTHREAD_MUTEX_NORMAL;
pub const PTHREAD_STACK_MIN: size_t = 64 * 1024; // 64 KiB

pub const PTHREAD_MUTEX_INITIALIZER: crate::pthread_mutex_t = crate::pthread_mutex_t {
    lock: 0,
    owner: 0,
    level: 0,
    type_: 0,
};

pub const PTHREAD_COND_INITIALIZER: crate::pthread_cond_t = crate::pthread_cond_t {
    mutex: core::ptr::null_mut(),
    value: 0,
    clockid: 4, // CLOCK_MONOTONIC_COARSE
};

// Standard file descriptors from Kernel/API/POSIX/unistd.h
pub const STDIN_FILENO: c_int = 0;
pub const STDOUT_FILENO: c_int = 1;
pub const STDERR_FILENO: c_int = 2;
pub const R_OK: c_int = 4;
pub const W_OK: c_int = 2;
pub const X_OK: c_int = 1;
pub const F_OK: c_int = 0;

// fd_set constants from LibC/fd_set.h
pub const FD_SETSIZE: usize = 1024;

// termios constants from Kernel/API/POSIX/termios.h
pub const TCSANOW: c_int = 0;
pub const TCSADRAIN: c_int = 1;
pub const TCSAFLUSH: c_int = 2;

// c_cc indices
pub const NCCS: usize = 32;
pub const VINTR: usize = 0;
pub const VQUIT: usize = 1;
pub const VERASE: usize = 2;
pub const VKILL: usize = 3;
pub const VEOF: usize = 4;
pub const VTIME: usize = 5;
pub const VMIN: usize = 6;
pub const VSWTC: usize = 7;
pub const VSTART: usize = 8;
pub const VSTOP: usize = 9;
pub const VSUSP: usize = 10;
pub const VEOL: usize = 11;
pub const VREPRINT: usize = 12;
pub const VDISCARD: usize = 13;
pub const VWERASE: usize = 14;
pub const VLNEXT: usize = 15;
pub const VEOL2: usize = 16;

// c_iflag bits
pub const IGNBRK: crate::tcflag_t = 0o000001;
pub const BRKINT: crate::tcflag_t = 0o000002;
pub const IGNPAR: crate::tcflag_t = 0o000004;
pub const PARMRK: crate::tcflag_t = 0o000010;
pub const INPCK: crate::tcflag_t = 0o000020;
pub const ISTRIP: crate::tcflag_t = 0o000040;
pub const INLCR: crate::tcflag_t = 0o000100;
pub const IGNCR: crate::tcflag_t = 0o000200;
pub const ICRNL: crate::tcflag_t = 0o000400;
pub const IXON: crate::tcflag_t = 0o002000;
pub const IXANY: crate::tcflag_t = 0o004000;
pub const IXOFF: crate::tcflag_t = 0o010000;
pub const IMAXBEL: crate::tcflag_t = 0o020000;
pub const IUTF8: crate::tcflag_t = 0o040000;

// c_oflag bits
pub const OPOST: crate::tcflag_t = 0o000001;
pub const OLCUC: crate::tcflag_t = 0o000002;
pub const ONLCR: crate::tcflag_t = 0o000004;
pub const OCRNL: crate::tcflag_t = 0o000010;
pub const ONOCR: crate::tcflag_t = 0o000020;
pub const ONLRET: crate::tcflag_t = 0o000040;
pub const OFILL: crate::tcflag_t = 0o000100;
pub const OFDEL: crate::tcflag_t = 0o000200;
pub const NLDLY: crate::tcflag_t = 0o000400;
pub const NL0: crate::tcflag_t = 0o000000;
pub const NL1: crate::tcflag_t = 0o000400;
pub const CRDLY: crate::tcflag_t = 0o003000;
pub const CR0: crate::tcflag_t = 0o000000;
pub const CR1: crate::tcflag_t = 0o001000;
pub const CR2: crate::tcflag_t = 0o002000;
pub const CR3: crate::tcflag_t = 0o003000;
pub const TABDLY: crate::tcflag_t = 0o014000;
pub const TAB0: crate::tcflag_t = 0o000000;
pub const TAB1: crate::tcflag_t = 0o004000;
pub const TAB2: crate::tcflag_t = 0o010000;
pub const TAB3: crate::tcflag_t = 0o014000;
pub const BSDLY: crate::tcflag_t = 0o020000;
pub const BS0: crate::tcflag_t = 0o000000;
pub const BS1: crate::tcflag_t = 0o020000;
pub const FFDLY: crate::tcflag_t = 0o100000;
pub const FF0: crate::tcflag_t = 0o000000;
pub const FF1: crate::tcflag_t = 0o100000;
pub const VTDLY: crate::tcflag_t = 0o040000;
pub const VT0: crate::tcflag_t = 0o000000;
pub const VT1: crate::tcflag_t = 0o040000;
pub const XTABS: crate::tcflag_t = 0o014000;

// c_cflag bits
pub const CSIZE: crate::tcflag_t = 0o000060;
pub const CS5: crate::tcflag_t = 0o000000;
pub const CS6: crate::tcflag_t = 0o000020;
pub const CS7: crate::tcflag_t = 0o000040;
pub const CS8: crate::tcflag_t = 0o000060;
pub const CSTOPB: crate::tcflag_t = 0o000100;
pub const CREAD: crate::tcflag_t = 0o000200;
pub const PARENB: crate::tcflag_t = 0o000400;
pub const PARODD: crate::tcflag_t = 0o001000;
pub const HUPCL: crate::tcflag_t = 0o002000;
pub const CLOCAL: crate::tcflag_t = 0o004000;
pub const CRTSCTS: crate::tcflag_t = 0o020_000_000_000;
pub const CMSPAR: crate::tcflag_t = 0o010_000_000_000;

// Baud rate constants
pub const B0: crate::speed_t = 0o000000;
pub const B50: crate::speed_t = 0o000001;
pub const B75: crate::speed_t = 0o000002;
pub const B110: crate::speed_t = 0o000003;
pub const B134: crate::speed_t = 0o000004;
pub const B150: crate::speed_t = 0o000005;
pub const B200: crate::speed_t = 0o000006;
pub const B300: crate::speed_t = 0o000007;
pub const B600: crate::speed_t = 0o000010;
pub const B1200: crate::speed_t = 0o000011;
pub const B1800: crate::speed_t = 0o000012;
pub const B2400: crate::speed_t = 0o000013;
pub const B4800: crate::speed_t = 0o000014;
pub const B9600: crate::speed_t = 0o000015;
pub const B19200: crate::speed_t = 0o000016;
pub const B38400: crate::speed_t = 0o000017;
pub const B57600: crate::speed_t = 0o010001;
pub const B115200: crate::speed_t = 0o010002;
pub const B230400: crate::speed_t = 0o010003;
pub const B460800: crate::speed_t = 0o010004;
pub const B500000: crate::speed_t = 0o010005;
pub const B576000: crate::speed_t = 0o010006;
pub const B921600: crate::speed_t = 0o010007;
pub const B1000000: crate::speed_t = 0o010010;
pub const B1152000: crate::speed_t = 0o010011;
pub const B1500000: crate::speed_t = 0o010012;
pub const B2000000: crate::speed_t = 0o010013;
pub const B2500000: crate::speed_t = 0o010014;
pub const B3000000: crate::speed_t = 0o010015;
pub const B3500000: crate::speed_t = 0o010016;
pub const B4000000: crate::speed_t = 0o010017;

// c_lflag bits
pub const ISIG: crate::tcflag_t = 0o000001;
pub const ICANON: crate::tcflag_t = 0o000002;
pub const ECHO: crate::tcflag_t = 0o000010;
pub const ECHOE: crate::tcflag_t = 0o000020;
pub const ECHOK: crate::tcflag_t = 0o000040;
pub const ECHONL: crate::tcflag_t = 0o000100;
pub const NOFLSH: crate::tcflag_t = 0o000200;
pub const TOSTOP: crate::tcflag_t = 0o000400;
pub const ECHOCTL: crate::tcflag_t = 0o001000;
pub const ECHOPRT: crate::tcflag_t = 0o002000;
pub const ECHOKE: crate::tcflag_t = 0o004000;
pub const FLUSHO: crate::tcflag_t = 0o010000;
pub const PENDIN: crate::tcflag_t = 0o040000;
pub const IEXTEN: crate::tcflag_t = 0o100000;
pub const EXTPROC: crate::tcflag_t = 0o200000;

// tcflow constants
pub const TCOOFF: c_int = 0;
pub const TCOON: c_int = 1;
pub const TCIOFF: c_int = 2;
pub const TCION: c_int = 3;

// tcflush constants
pub const TCIFLUSH: c_int = 0;
pub const TCOFLUSH: c_int = 1;
pub const TCIOFLUSH: c_int = 2;

// Memory mapping constants from Kernel/API/POSIX/mman.h
pub const PROT_NONE: c_int = 0x0;
pub const PROT_READ: c_int = 0x1;
pub const PROT_WRITE: c_int = 0x2;
pub const PROT_EXEC: c_int = 0x4;
pub const MAP_FILE: c_int = 0x00;
pub const MAP_SHARED: c_int = 0x01;
pub const MAP_PRIVATE: c_int = 0x02;
pub const MAP_FIXED: c_int = 0x10;
pub const MAP_ANONYMOUS: c_int = 0x20;
pub const MAP_ANON: c_int = MAP_ANONYMOUS;
pub const MAP_STACK: c_int = 0x40;
pub const MAP_FAILED: *mut c_void = !0usize as *mut c_void;

// Signal constants from Kernel/API/POSIX/signal_numbers.h
pub const SIGHUP: c_int = 1;
pub const SIGINT: c_int = 2;
pub const SIGQUIT: c_int = 3;
pub const SIGILL: c_int = 4;
pub const SIGTRAP: c_int = 5;
pub const SIGABRT: c_int = 6;
pub const SIGBUS: c_int = 7;
pub const SIGFPE: c_int = 8;
pub const SIGKILL: c_int = 9;
pub const SIGUSR1: c_int = 10;
pub const SIGSEGV: c_int = 11;
pub const SIGUSR2: c_int = 12;
pub const SIGPIPE: c_int = 13;
pub const SIGALRM: c_int = 14;
pub const SIGTERM: c_int = 15;
pub const SIGCHLD: c_int = 17;
pub const SIGCONT: c_int = 18;
pub const SIGSTOP: c_int = 19;
pub const SIGTSTP: c_int = 20;
pub const SIGTTIN: c_int = 21;
pub const SIGTTOU: c_int = 22;
pub const SIGURG: c_int = 23;
pub const SIGXCPU: c_int = 24;
pub const SIGXFSZ: c_int = 25;
pub const SIGVTALRM: c_int = 26;
pub const SIGPROF: c_int = 27;
pub const SIGWINCH: c_int = 28;
pub const SIGIO: c_int = 29;
pub const SIGSYS: c_int = 31;
pub const NSIG: c_int = 33;
pub const SIG_BLOCK: c_int = 0;
pub const SIG_UNBLOCK: c_int = 1;
pub const SIG_SETMASK: c_int = 2;

// Resource usage from sys/resource.h
pub const RUSAGE_SELF: c_int = 1;
pub const RUSAGE_CHILDREN: c_int = 2;
pub const RLIMIT_NOFILE: c_int = 5;
pub const RLIMIT_STACK: c_int = 6;
pub const RLIMIT_AS: c_int = 7;
pub const RLIM_INFINITY: crate::rlim_t = usize::MAX;

// Clock IDs from Kernel/API/POSIX/time.h
pub const CLOCK_REALTIME: crate::clockid_t = 0;
pub const CLOCK_MONOTONIC: crate::clockid_t = 1;
pub const CLOCK_MONOTONIC_RAW: crate::clockid_t = 2;

pub const PRIO_PROCESS: c_int = 0;
pub const PRIO_PGRP: c_int = 1;
pub const PRIO_USER: c_int = 2;

pub const DT_UNKNOWN: u8 = 0;
pub const DT_FIFO: u8 = 1;
pub const DT_CHR: u8 = 2;
pub const DT_DIR: u8 = 4;
pub const DT_BLK: u8 = 6;
pub const DT_REG: u8 = 8;
pub const DT_LNK: u8 = 10;
pub const DT_SOCK: u8 = 12;

safe_f! {
    pub const fn WIFSTOPPED(status: c_int) -> bool {
        (status & 0xff) == 0x7f
    }
    pub const fn WSTOPSIG(status: c_int) -> c_int {
        (status >> 8) & 0xff
    }
    pub const fn WIFCONTINUED(status: c_int) -> bool {
        status == 0xffff
    }
    pub const fn WIFSIGNALED(status: c_int) -> bool {
        ((status & 0x7f) + 1) as i8 >= 2
    }
    pub const fn WTERMSIG(status: c_int) -> c_int {
        status & 0x7f
    }
    pub const fn WIFEXITED(status: c_int) -> bool {
        (status & 0x7f) == 0
    }
    pub const fn WEXITSTATUS(status: c_int) -> c_int {
        (status >> 8) & 0xff
    }
    pub const fn WCOREDUMP(status: c_int) -> bool {
        (status & 0x80) != 0
    }
}

f! {
    pub fn FD_CLR(fd: c_int, set: *mut fd_set) -> () {
        let fd = fd as usize;
        let size = size_of_val(&(*set).fds_bits[0]) * 8;
        (*set).fds_bits[fd / size] &= !(1 << (fd % size));
    }
    pub fn FD_ISSET(fd: c_int, set: *const fd_set) -> bool {
        let fd = fd as usize;
        let size = size_of_val(&(*set).fds_bits[0]) * 8;
        ((*set).fds_bits[fd / size] & (1 << (fd % size))) != 0
    }
    pub fn FD_SET(fd: c_int, set: *mut fd_set) -> () {
        let fd = fd as usize;
        let size = size_of_val(&(*set).fds_bits[0]) * 8;
        (*set).fds_bits[fd / size] |= 1 << (fd % size);
    }
    pub fn FD_ZERO(set: *mut fd_set) -> () {
        for slot in (*set).fds_bits.iter_mut() {
            *slot = 0;
        }
    }
    pub fn CMSG_ALIGN(len: size_t) -> size_t {
        (len + size_of::<size_t>() - 1) & !(size_of::<size_t>() - 1)
    }
    pub fn CMSG_NXTHDR(
        mhdr: *const crate::msghdr,
        cmsg: *const crate::cmsghdr,
    ) -> *mut crate::cmsghdr {
        let next = (cmsg as usize) + CMSG_ALIGN((*cmsg).cmsg_len);
        let end = ((*mhdr).msg_control as usize) + (*mhdr).msg_controllen;
        if next + size_of::<crate::cmsghdr>() > end {
            core::ptr::null_mut()
        } else {
            next as *mut crate::cmsghdr
        }
    }
    pub fn CMSG_FIRSTHDR(mhdr: *const crate::msghdr) -> *mut crate::cmsghdr {
        if (*mhdr).msg_controllen >= size_of::<crate::cmsghdr>() {
            (*mhdr).msg_control as *mut crate::cmsghdr
        } else {
            core::ptr::null_mut()
        }
    }
}

// makedev/major/minor as safe Rust functions (macros in SerenityOS LibC)
#[inline]
pub fn makedev(major: c_uint, minor: c_uint) -> crate::dev_t {
    let major = major as crate::dev_t;
    let minor = minor as crate::dev_t;
    (minor & 0xff) | (major << 8) | ((minor & !0xff) << 12)
}

#[inline]
pub fn major(dev: crate::dev_t) -> c_uint {
    ((dev >> 8) & 0xfff) as c_uint
}

#[inline]
pub fn minor(dev: crate::dev_t) -> c_uint {
    ((dev & 0xff) | ((dev >> 12) & !0xff)) as c_uint
}

extern "C" {
    pub fn __errno_location() -> *mut c_int;
    pub fn strerror_r(errnum: c_int, buf: *mut c_char, buflen: size_t) -> c_int;
    pub fn dirfd(dirp: *mut crate::DIR) -> c_int;
    pub fn pipe2(fds: *mut c_int, flags: c_int) -> c_int;
    pub fn getdtablesize() -> c_int;
    pub fn tcgetattr(fd: c_int, termios: *mut crate::termios) -> c_int;
    pub fn tcsetattr(fd: c_int, optional_actions: c_int, termios: *const crate::termios) -> c_int;
    pub fn tcflush(fd: c_int, queue_selector: c_int) -> c_int;
    pub fn tcflow(fd: c_int, action: c_int) -> c_int;
    pub fn cfgetispeed(termios: *const crate::termios) -> crate::speed_t;
    pub fn cfgetospeed(termios: *const crate::termios) -> crate::speed_t;
    pub fn cfsetispeed(termios: *mut crate::termios, speed: crate::speed_t) -> c_int;
    pub fn cfsetospeed(termios: *mut crate::termios, speed: crate::speed_t) -> c_int;
    pub fn cfmakeraw(termios: *mut crate::termios);
    pub fn dup3(oldfd: c_int, newfd: c_int, flags: c_int) -> c_int;
    pub fn faccessat(dirfd: c_int, pathname: *const c_char, mode: c_int, flags: c_int) -> c_int;
    pub fn fdatasync(fd: c_int) -> c_int;
    pub fn fstatfs(fd: c_int, buf: *mut crate::statfs) -> c_int;
    pub fn statfs(path: *const c_char, buf: *mut crate::statfs) -> c_int;
    pub fn mknodat(dirfd: c_int, pathname: *const c_char, mode: crate::mode_t, dev: crate::dev_t) -> c_int;
    pub fn posix_fadvise(fd: c_int, offset: crate::off_t, len: crate::off_t, advice: c_int) -> c_int;
    pub fn posix_fallocate(fd: c_int, offset: crate::off_t, len: crate::off_t) -> c_int;
    pub fn seekdir(dirp: *mut crate::DIR, loc: c_long);
    pub fn sync();
    pub fn getgrgid_r(
        gid: crate::gid_t,
        grp: *mut crate::group,
        buf: *mut c_char,
        buflen: size_t,
        result: *mut *mut crate::group,
    ) -> c_int;
    pub fn getgrnam_r(
        name: *const c_char,
        grp: *mut crate::group,
        buf: *mut c_char,
        buflen: size_t,
        result: *mut *mut crate::group,
    ) -> c_int;
    pub fn getpwnam_r(
        name: *const c_char,
        pwd: *mut passwd,
        buf: *mut c_char,
        buflen: size_t,
        result: *mut *mut passwd,
    ) -> c_int;
    pub fn getpwuid_r(
        uid: crate::uid_t,
        pwd: *mut passwd,
        buf: *mut c_char,
        buflen: size_t,
        result: *mut *mut passwd,
    ) -> c_int;
    pub fn memalign(align: size_t, size: size_t) -> *mut c_void;
    pub fn reallocarray(ptr: *mut c_void, nmemb: size_t, size: size_t) -> *mut c_void;
    pub fn pthread_atfork(
        prepare: Option<unsafe extern "C" fn()>,
        parent: Option<unsafe extern "C" fn()>,
        child: Option<unsafe extern "C" fn()>,
    ) -> c_int;
    pub fn pthread_create(
        tid: *mut crate::pthread_t,
        attr: *const crate::pthread_attr_t,
        start: extern "C" fn(*mut c_void) -> *mut c_void,
        arg: *mut c_void,
    ) -> c_int;
    pub fn pthread_condattr_setclock(
        attr: *mut crate::pthread_condattr_t,
        clock_id: crate::clockid_t,
    ) -> c_int;
    pub fn pthread_sigmask(
        how: c_int,
        set: *const crate::sigset_t,
        oldset: *mut crate::sigset_t,
    ) -> c_int;
    pub fn pthread_cancel(thread: crate::pthread_t) -> c_int;
    pub fn pthread_kill(thread: crate::pthread_t, sig: c_int) -> c_int;
    pub fn madvise(addr: *mut c_void, len: size_t, advice: c_int) -> c_int;
    pub fn msync(addr: *mut c_void, len: size_t, flags: c_int) -> c_int;
    pub fn mprotect(addr: *mut c_void, len: size_t, prot: c_int) -> c_int;
    pub fn getpriority(which: c_int, who: crate::id_t) -> c_int;
    pub fn setpriority(which: c_int, who: crate::id_t, prio: c_int) -> c_int;
    pub fn getrlimit(resource: c_int, rlim: *mut crate::rlimit) -> c_int;
    pub fn setrlimit(resource: c_int, rlim: *const crate::rlimit) -> c_int;
    pub fn gettimeofday(tp: *mut crate::timeval, tz: *mut crate::timezone) -> c_int;
    pub fn clock_gettime(clk_id: crate::clockid_t, tp: *mut crate::timespec) -> c_int;
    pub fn uname(utsname: *mut utsname) -> c_int;
    pub fn readv(fd: c_int, iov: *const crate::iovec, iovcnt: c_int) -> ssize_t;
    pub fn writev(fd: c_int, iov: *const crate::iovec, iovcnt: c_int) -> ssize_t;
    pub fn preadv(fd: c_int, iov: *const crate::iovec, iovcnt: c_int, offset: off_t) -> ssize_t;
    pub fn pwritev(fd: c_int, iov: *const crate::iovec, iovcnt: c_int, offset: off_t) -> ssize_t;
    pub fn futimens(fd: c_int, times: *const crate::timespec) -> c_int;
    pub fn getnameinfo(
        addr: *const crate::sockaddr,
        addrlen: crate::socklen_t,
        host: *mut c_char,
        hostlen: crate::socklen_t,
        serv: *mut c_char,
        servlen: crate::socklen_t,
        flags: c_int,
    ) -> c_int;
    pub fn ioctl(fd: c_int, request: c_ulong, ...) -> c_int;
    pub fn arc4random_buf(buf: *mut c_void, size: size_t);
    pub fn bind(socket: c_int, address: *const crate::sockaddr, address_len: crate::socklen_t) -> c_int;
    pub fn recvfrom(
        socket: c_int,
        buf: *mut c_void,
        len: size_t,
        flags: c_int,
        addr: *mut crate::sockaddr,
        addrlen: *mut crate::socklen_t,
    ) -> ssize_t;
    pub fn setgroups(ngroups: size_t, ptr: *const crate::gid_t) -> c_int;
    pub fn utimensat(
        dirfd: c_int,
        pathname: *const c_char,
        times: *const crate::timespec,
        flags: c_int,
    ) -> c_int;
}
